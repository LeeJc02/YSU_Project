import torch
import torchvision
from torch import nn
from torch.utils.data import DataLoader
import matplotlib.pyplot as plt

# CIFAR-10 数据集及其增强
stats = ((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010))
train_data = torchvision.datasets.CIFAR10("./data", train=True, transform=torchvision.transforms.Compose([
    torchvision.transforms.ColorJitter(0.5),
    torchvision.transforms.RandomHorizontalFlip(),
    torchvision.transforms.ToTensor(),
    torchvision.transforms.Normalize(*stats)
]))
test_data = torchvision.datasets.CIFAR10("./data", train=False, transform=torchvision.transforms.Compose([
    torchvision.transforms.ToTensor(),
    torchvision.transforms.Normalize(*stats)
]))
train_dataloader = DataLoader(train_data, batch_size=128, shuffle=True)
test_dataloader = DataLoader(test_data, batch_size=128)

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")


# 定义 Squeeze-and-Excitation (SE) 模块
class SEBlock(nn.Module):
    def __init__(self, in_channels, reduction_ratio=16):
        super(SEBlock, self).__init__()
        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Sequential(
            nn.Linear(in_channels, in_channels // reduction_ratio, bias=False),
            nn.ReLU(inplace=True),
            nn.Linear(in_channels // reduction_ratio, in_channels, bias=False),
            nn.Sigmoid()
        )

    def forward(self, x):
        batch, channels, _, _ = x.size()
        y = self.global_avg_pool(x).view(batch, channels)
        y = self.fc(y).view(batch, channels, 1, 1)
        return x * y.expand_as(x)


# 定义带 SE 模块的卷积块
def conv_block(in_channels, out_channels, pool=False, se_module=True):
    layers = [nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1),
              nn.BatchNorm2d(out_channels),
              nn.ReLU(inplace=True)]
    if pool: layers.append(nn.MaxPool2d(2))
    if se_module: layers.append(SEBlock(out_channels))  # 加入 SE 模块
    return nn.Sequential(*layers)


# 定义 SE_CNN 模型
class SE_CNN(nn.Module):
    def __init__(self):
        super(SE_CNN, self).__init__()
        self.conv1 = conv_block(3, 32)
        self.conv2 = conv_block(32, 64, pool=True)
        self.res1 = nn.Sequential(conv_block(64, 64), conv_block(64, 64))
        self.conv3 = conv_block(64, 128)
        self.conv4 = conv_block(128, 256, pool=True)
        self.res2 = nn.Sequential(conv_block(256, 256), conv_block(256, 256))
        self.conv5 = conv_block(256, 512)
        self.conv6 = conv_block(512, 1024, pool=True)
        self.res3 = nn.Sequential(conv_block(1024, 1024), conv_block(1024, 1024))
        self.linear1 = nn.Sequential(nn.MaxPool2d(4), nn.Flatten(), nn.Dropout(0.2), nn.Linear(1024, 10))

    def forward(self, x):
        out = self.conv1(x)
        out = self.conv2(out)
        out = self.res1(out) + out
        out = self.conv3(out)
        out = self.conv4(out)
        out = self.res2(out) + out
        out = self.conv5(out)
        out = self.conv6(out)
        out = self.res3(out) + out
        out = self.linear1(out)
        return out


# 实例化模型、损失函数、优化器和学习率调度器
model = SE_CNN().to(device)
loss_fn = nn.CrossEntropyLoss().to(device)
optimizer = torch.optim.Adam(model.parameters(), lr=0.01, weight_decay=1e-4)
sched = torch.optim.lr_scheduler.OneCycleLR(optimizer, max_lr=0.01, epochs=11, steps_per_epoch=len(train_dataloader))

# 存储训练过程数据
epochs = 11
train_losses, test_losses, train_accuracies,test_accuracies, learning_rates = [], [], [], [],[]

for epoch in range(epochs):
    model.train()
    total_train_loss, total_accuracy = 0, 0
    for imgs, targets in train_dataloader:
        imgs, targets = imgs.to(device), targets.to(device)
        optimizer.zero_grad()
        output = model(imgs)
        loss = loss_fn(output, targets)
        loss.backward()
        optimizer.step()
        sched.step()

        total_train_loss += loss.item()
        total_accuracy += (output.argmax(1) == targets).sum().item()
    train_losses.append(total_train_loss / len(train_dataloader))
    train_accuracies.append(total_accuracy/len(train_data))


    # 测试
    model.eval()
    total_test_loss, total_correct = 0, 0
    with torch.no_grad():
        for imgs, targets in test_dataloader:
            imgs, targets = imgs.to(device), targets.to(device)
            output = model(imgs)
            loss = loss_fn(output, targets)
            total_test_loss += loss.item()
            total_correct += (output.argmax(1) == targets).sum().item()
    test_losses.append(total_test_loss / len(test_dataloader))
    test_accuracies.append(total_correct / len(test_data))
    learning_rates.append(sched.get_last_lr()[0])

    print(
        f"Epoch [{epoch + 1}/{epochs}], Train Loss: {train_losses[-1]:.4f}, Test Loss: {test_losses[-1]:.4f}, Accuracy: {test_accuracies[-1]:.4f}")
print(train_losses)
print(test_losses)
print(train_losses)
print(test_accuracies)
print(learning_rates)

# 绘图并保存
def plot_metrics(epochs_range, train_losses, test_losses, train_accuracies,test_accuracies, learning_rates, model_name="SE_CNN"):
    plt.figure(figsize=(15, 7))

    # 训练和测试损失
    plt.subplot(1, 3, 1)
    plt.plot(epochs_range, train_losses, label="Train Loss", color='blue')
    plt.plot(epochs_range, test_losses, label="Test Loss", color='red')
    plt.xlabel("Epochs")
    plt.ylabel("Loss")
    plt.legend()
    plt.title(f"{model_name} Train and Test Loss")

    # 准确率
    plt.subplot(1, 3, 2)
    plt.plot(epochs_range, train_accuracies, label="Train Accuracy", color='orange')
    plt.plot(epochs_range, test_accuracies, label="Test Accuracy", color='purple')
    plt.xlabel("Epochs")
    plt.ylabel("Accuracy")
    plt.legend()
    plt.title(f"{model_name} Train and Test Accuracy")

    # 学习率
    plt.subplot(1, 3, 3)
    plt.plot(epochs_range, learning_rates, label="Learning Rate", color='green')
    plt.xlabel("Epochs")
    plt.ylabel("Learning Rate")
    plt.legend()
    plt.title(f"{model_name} Learning Rate Schedule")

    plt.tight_layout()
    plt.savefig(f"{model_name}_metrics1.png")
    print(f"{model_name}_metrics.png saved successfully.")
    plt.show()


# 调用绘图函数
epochs_range = range(1, epochs + 1)
plot_metrics(epochs_range, train_losses, test_losses,train_accuracies,test_accuracies, learning_rates, model_name="SE_CNN")
