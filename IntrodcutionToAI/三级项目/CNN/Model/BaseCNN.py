#69%
import torch
from torch import nn
from torchvision import datasets, transforms
from torch.utils.data import DataLoader
import matplotlib.pyplot as plt

# 定义 CNN 模型
class BaseCNN(nn.Module):
    def __init__(self):
        super(BaseCNN, self).__init__()
        self.layer1 = nn.Sequential(
            nn.Conv2d(3, 16, kernel_size=3),  # CIFAR-10 数据集是 3 通道（RGB）
            nn.BatchNorm2d(16),
            nn.ReLU(inplace=True))
        self.layer2 = nn.Sequential(
            nn.Conv2d(16, 32, kernel_size=3),
            nn.BatchNorm2d(32),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.layer3 = nn.Sequential(
            nn.Conv2d(32, 64, kernel_size=3),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True))
        self.layer4 = nn.Sequential(
            nn.Conv2d(64, 128, kernel_size=3),
            nn.BatchNorm2d(128),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.fc = nn.Sequential(
            nn.Linear(128 * 5 * 5, 1024),
            nn.ReLU(inplace=True),
            nn.Linear(1024, 128),
            nn.ReLU(inplace=True),
            nn.Linear(128, 10))

    def forward(self, x):
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        x = x.view(x.size(0), -1)  # 展平成一维向量，适合全连接层
        return self.fc(x)

# 设置超参数
learning_rate = 1e-2
batch_size = 128
num_epochs = 11

# 数据集与数据加载器
transform = transforms.Compose([transforms.ToTensor()])
train_dataset = datasets.CIFAR10(root='./data', train=True, transform=transform, download=True)
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
test_dataset = datasets.CIFAR10(root='./data', train=False, transform=transform)
test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)

# 初始化模型、损失函数和优化器
model = BaseCNN()
criterion = nn.CrossEntropyLoss()
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

# 检查是否有 GPU
if torch.cuda.is_available():
    model = model.cuda()

# 记录每个 epoch 的损失和准确率
train_losses = []
train_accuracies = []
test_losses = []
test_accuracies = []

# 训练模型
for epoch in range(num_epochs):
    model.train()
    train_loss, train_correct = 0.0, 0

    for images, labels in train_loader:
        if torch.cuda.is_available():
            images, labels = images.cuda(), labels.cuda()

        # 前向传播
        outputs = model(images)
        loss = criterion(outputs, labels)

        # 反向传播和优化
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        train_loss += loss.item() * labels.size(0)
        _, predicted = outputs.max(1)
        train_correct += predicted.eq(labels).sum().item()

    # 计算并记录训练集的平均损失和准确率
    train_loss /= len(train_dataset)
    train_acc = train_correct / len(train_dataset)
    train_losses.append(train_loss)
    train_accuracies.append(train_acc)

    # 在测试集上评估模型
    model.eval()
    test_loss, test_correct = 0.0, 0
    with torch.no_grad():
        for images, labels in test_loader:
            if torch.cuda.is_available():
                images, labels = images.cuda(), labels.cuda()
            outputs = model(images)
            loss = criterion(outputs, labels)

            test_loss += loss.item() * labels.size(0)
            _, predicted = outputs.max(1)
            test_correct += predicted.eq(labels).sum().item()

    test_loss /= len(test_dataset)
    test_acc = test_correct / len(test_dataset)
    test_losses.append(test_loss)
    test_accuracies.append(test_acc)

    print(f"Epoch {epoch+1}/{num_epochs}, Train Loss: {train_loss:.4f}, Train Acc: {train_acc:.4f}, Test Loss: {test_loss:.4f}, Test Acc: {test_acc:.4f}")
print(train_losses)
print(test_losses)
print(train_accuracies)
print(test_accuracies)
# 可视化训练过程
plt.figure(figsize=(12, 5))

plt.subplot(1, 3, 1)
plt.plot(range(1, num_epochs+1), train_losses, label='Train Loss')
plt.plot(range(1, num_epochs+1), test_losses, label='Test Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.legend()
plt.title(f"BaseCNN Train and Test Loss")

plt.subplot(1, 3, 2)
plt.plot(range(1, num_epochs+1), train_accuracies, label='Train Accuracy',color='orange')
plt.plot(range(1, num_epochs+1), test_accuracies, label='Test Accuracy',color='purple')
plt.xlabel('Epoch')
plt.ylabel('Accuracy')
plt.legend()
plt.title('BaseCNN Train and Test Accuracy')

plt.subplot(1,3,3)
plt.plot(range(1, num_epochs+1), [0.01 for i in range(11)], label="Learning Rate",color='green')
plt.xlabel("Epochs")
plt.ylabel("Learning Rate")
plt.legend()
plt.title(f"BaseCNN Learning Rate Schedule")

plt.show()
