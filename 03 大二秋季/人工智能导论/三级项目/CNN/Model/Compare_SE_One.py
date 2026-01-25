import torch
import torchvision
from torch import nn
from torch.utils.data import DataLoader
import matplotlib.pyplot as plt

# CIFAR-10 数据集及其增强
# stats 变量存储了标准化时的均值和标准差
stats = ((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010))
train_data = torchvision.datasets.CIFAR10("./data", train=True, transform=torchvision.transforms.Compose([
    torchvision.transforms.ColorJitter(0.5),  # 随机调整图像亮度
    torchvision.transforms.RandomHorizontalFlip(),  # 随机水平翻转
    torchvision.transforms.ToTensor(),  # 将图像转化为张量
    torchvision.transforms.Normalize(*stats)  # 应用均值和标准差进行标准化
]))
# 定义测试集，仅进行标准化处理
test_data = torchvision.datasets.CIFAR10("./data", train=False, transform=torchvision.transforms.Compose([
    torchvision.transforms.ToTensor(),
    torchvision.transforms.Normalize(*stats)
]))
train_dataloader = DataLoader(train_data, batch_size=128, shuffle=True)
test_dataloader = DataLoader(test_data, batch_size=128)

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")#使用gpu加速

# 定义 Squeeze-and-Excitation (SE) 模块，用于提高模型关注度
class SEBlock(nn.Module):
    def __init__(self, in_channels, reduction_ratio=16):
        super(SEBlock, self).__init__()
        # 全局平均池化层，将空间信息压缩成单个值
        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)
        # 两层全连接网络，逐层减少和增加通道数。降维再升维，学习各通道的“重要性”权重
        self.fc = nn.Sequential(
            nn.Linear(in_channels, in_channels // reduction_ratio, bias=False),  # 降维：减少通道数
            nn.ReLU(inplace=True),  # 激活函数
            nn.Linear(in_channels // reduction_ratio, in_channels, bias=False),  # 升维：还原通道数
            nn.Sigmoid()  # 使用 Sigmoid 将权重值限制在 (0,1) 范围内
        )

    def forward(self, x):
        # 执行前向传播，计算注意力系数并进行加权
        batch, channels, _, _ = x.size()  # 获取输入的维度信息
        y = self.global_avg_pool(x).view(batch, channels)  # 全局平均池化并展平为 (batch, channels)
        y = self.fc(y).view(batch, channels, 1, 1)  # 通过全连接层生成注意力权重，调整为 (batch, channels, 1, 1)
        return x * y.expand_as(x)  # 将权重扩展至输入张量形状，并与原输入相乘，赋予注意力权重


# 定义卷积块，将 SE 模块加入卷积块中
def conv_block(in_channels, out_channels, pool=False, se_module=True):
    layers = [nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1),
              nn.BatchNorm2d(out_channels),  # 批量标准化
              nn.ReLU(inplace=True)]  # 激活函数
    if pool: layers.append(nn.MaxPool2d(2))  # 选项：最大池化
    if se_module: layers.append(SEBlock(out_channels))  # 选项：加入 SE 模块
    return nn.Sequential(*layers) # 返回该卷积块的层序列


# 定义改进后的 SE-CNN 模型
class SEModel(nn.Module):
    def __init__(self):
        super(SEModel, self).__init__()
        # 定义每层卷积和残差块
        self.conv1 = conv_block(3, 32)  # 输入层，通道数从3扩展到32
        self.conv2 = conv_block(32, 64, pool=True)  # 第二层，通道数扩展至64，池化降低分辨率
        self.res1 = nn.Sequential(conv_block(64, 64), conv_block(64, 64))  # 第一个残差块
        self.conv3 = conv_block(64, 128)  # 第三层
        self.conv4 = conv_block(128, 256, pool=True)  # 第四层
        self.res2 = nn.Sequential(conv_block(256, 256), conv_block(256, 256))  # 第二个残差块
        self.conv5 = conv_block(256, 512)  # 第五层
        self.conv6 = conv_block(512, 1024, pool=True)  # 第六层
        self.res3 = nn.Sequential(conv_block(1024, 1024), conv_block(1024, 1024))  # 第三个残差块
        # 最后线性层，用于分类输出
        self.linear1 = nn.Sequential(nn.MaxPool2d(4),  # 池化将分辨率降到最低
                                     nn.Flatten(),  # 展平张量用于全连接层
                                     nn.Dropout(0.2),  # Dropout 防止过拟合
                                     nn.Linear(1024, 10))  # 分类为10类

    def forward(self, x):
        # 执行前向传播过程
        out = self.conv1(x)
        out = self.conv2(out)
        out = self.res1(out) + out  # 第一个残差连接,将残差块的输出与输入相加
        out = self.conv3(out)
        out = self.conv4(out)
        out = self.res2(out) + out  # 第二个残差连接
        out = self.conv5(out)
        out = self.conv6(out)
        out = self.res3(out) + out  # 第三个残差连接
        out = self.linear1(out)# 通过全连接层输出
        return out

# 定义 SimpleCNN 模型
class Onecyclelr_CNN(nn.Module):
    def __init__(self):
        super(Onecyclelr_CNN, self).__init__()
        self.layer1 = conv_block(3, 32,se_module=False)
        self.layer2 = conv_block(32, 64, pool=True,se_module=False)
        self.layer3 = conv_block(64, 128,se_module=False)
        self.layer4 = conv_block(128, 256, pool=True,se_module=False)
        self.fc = nn.Sequential(
            nn.MaxPool2d(8),
            nn.Flatten(),
            nn.Linear(256, 10)
        )

    def forward(self, x):
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        x = self.fc(x)
        return x

epochs=11

# 训练过程
def train_and_evaluate(model, name):
    # 将模型加载到指定设备 (GPU 或 CPU) 上
    model = model.to(device)

    # 定义损失函数为交叉熵损失，用于多分类任务
    loss_fn = nn.CrossEntropyLoss().to(device)

    # 定义优化器为 Adam，使用学习率 0.01 和权重衰减 1e-4 防止过拟合
    optimizer = torch.optim.Adam(model.parameters(), lr=0.01, weight_decay=1e-4)

    # 定义 OneCycleLR 学习率调度器，用于动态调整学习率，使其在训练早期快速上升
    sched = torch.optim.lr_scheduler.OneCycleLR(
        optimizer,  # 要应用调度的优化器
        max_lr=0.01,  # 最大学习率，即训练过程中的峰值学习率
        epochs=epochs,  # 训练的总周期数
        steps_per_epoch=len(train_dataloader)  # 每个周期的更新步数
    )

    # 初始化用于存储每个 epoch 的训练/测试损失和准确率的列表
    train_losses, test_losses, train_accuracies, test_accuracies, learning_rates = [], [], [], [], []

    # 开始循环训练每个 epoch
    for epoch in range(epochs):
        model.train()  # 设置模型为训练模式，以启用 Dropout 等训练特有的层
        total_train_loss = 0  # 用于累积训练损失
        total_accuracy = 0  # 用于累积训练准确率

        # 循环遍历每批训练数据
        for imgs, targets in train_dataloader:
            imgs, targets = imgs.to(device), targets.to(device)  # 将数据和标签加载到设备上
            optimizer.zero_grad()  # 清除上一步的梯度，以免梯度累积

            # 前向传播：将输入图像传入模型获取预测
            output = model(imgs)
            # 计算损失：预测输出和真实标签之间的交叉熵损失
            loss = loss_fn(output, targets)
            # 反向传播：计算每个参数的梯度
            loss.backward()
            # 参数更新：根据梯度和学习率更新参数
            optimizer.step()
            # 调度器步进：更新学习率，遵循 OneCycleLR 策略
            sched.step()

            # 累积训练损失：将当前批次的损失值加入到总损失中
            total_train_loss += loss.item()
            # 计算批次正确预测的数量并累加
            total_accuracy += (output.argmax(1) == targets).sum().item()

        # 计算当前 epoch 的平均训练损失
        avg_train_loss = total_train_loss / len(train_dataloader)
        train_losses.append(avg_train_loss)  # 记录当前 epoch 的平均训练损失

        # 计算训练集准确率：正确预测数 / 总样本数
        train_acc = total_accuracy / len(train_data)
        train_accuracies.append(train_acc)  # 记录当前 epoch 的训练准确率

        # 模型进入评估模式：禁用 Dropout 等训练特定层
        model.eval()
        total_test_loss = 0  # 用于累积测试损失
        correct_predictions = 0  # 用于累积正确的测试预测数

        # 禁用梯度计算，以节省内存和加快计算速度
        with torch.no_grad():
            for imgs, targets in test_dataloader:
                imgs, targets = imgs.to(device), targets.to(device)
                output = model(imgs)  # 前向传播获取输出
                loss = loss_fn(output, targets)  # 计算测试损失
                total_test_loss += loss.item()  # 累积测试损失
                correct_predictions += (output.argmax(1) == targets).sum().item()  # 计算正确预测数

        # 计算当前 epoch 的平均测试损失
        avg_test_loss = total_test_loss / len(test_dataloader)
        test_losses.append(avg_test_loss)  # 记录当前 epoch 的平均测试损失

        # 计算测试集准确率
        test_acc = correct_predictions / len(test_data)
        test_accuracies.append(test_acc)  # 记录当前 epoch 的测试准确率

        # 获取当前学习率并记录
        learning_rates.append(sched.get_last_lr())

        print(
            f"Epoch [{epoch + 1}/{epochs}], Train Loss: {avg_train_loss:.4f}, Test Loss: {avg_test_loss:.4f}, Train_Accuracy:{train_acc:.4f},Test_Accuracy: {test_acc:.4f}")

    plot_metrics(train_losses, test_losses, train_accuracies, test_accuracies, learning_rates, name)
def plot_metrics( train_losses, test_losses, train_accuracies,test_accuracies, learning_rates, model_name):
    # 使用 matplotlib 绘制损失和准确率图像
    epochs_range = range(1, epochs + 1)
    plt.figure(figsize=(14, 7))

    # 训练损失
    plt.subplot(1, 3, 1)
    plt.plot(epochs_range, train_losses, label="Train Loss")
    plt.plot(epochs_range, test_losses, label="Test Loss")
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
    plt.savefig(f"{model_name}_running.png")
    print(f"{model_name}_running.png saved successfully.")
    plt.show()

# 训练两种模型
se_model = SEModel()
onecyclelr_cnn = Onecyclelr_CNN()
train_and_evaluate(se_model, "SE_CNN")

train_and_evaluate(onecyclelr_cnn, "Onecyclelr_CNN")