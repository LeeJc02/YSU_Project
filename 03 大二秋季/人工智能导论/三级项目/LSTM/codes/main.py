import torch
import numpy as np
import torch.nn as nn
import torch.optim as optim
import matplotlib.pyplot as plt
from gensim.models import Word2Vec
from torch.nn.utils.rnn import pad_sequence
from sklearn.preprocessing import LabelEncoder
from torch.utils.data import DataLoader, TensorDataset
from sklearn.metrics import accuracy_score, mean_squared_error
from date_process import train_texts, test_texts, train_labels, test_labels

# NumPy：Python 中的数值运算库。
# PyTorch：一个深度学习框架，提供用于构建和训练神经网络的工具。
# Gensim：一个用于主题建模和文档相似性分析的库，此处用于 Word2Vec。
# Scikit-learn：一个机器学习库，提供用于预处理和评估模型的工具。
# DataLoader 和 TensorDataset：PyTorch 中用于处理批量数据的实用程序。


device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')


# 构建LSTM
# LSTMModel：nn.定义 LSTM 网络的模块。
# embedding_dim：输入嵌入的维数。
# hidden_dim：LSTM 处于隐藏状态的特征数。
# output_dim：输出类的数量。
class LSTMModel(nn.Module):
    def __init__(self, embedding_dim, hidden_dim, output_dim):
        super(LSTMModel, self).__init__()
        self.lstm = nn.LSTM(embedding_dim, hidden_dim, batch_first=True)
        self.fc = nn.Linear(hidden_dim, output_dim)

    # forward
    # method：定义输入数据如何通过模型。
    # LSTM
    # 处理输入序列并返回输出和隐藏状态。
    # 最后一个隐藏状态通过一个全连接层来产生最终输出。
    def forward(self, x):
        x, (hn, cn) = self.lstm(x)
        x = hn[-1]  # 取最后一个时间步的输出
        x = self.fc(x)
        return x


# 文本转向量
# 使用经过训练的 Word2Vec 模型将文本列表转换为向量。
# 对于文本中的每个单词，如果模型中存在该向量，则它会检索该向量。
# 如果文本短于 max_length，则填充向量为零;如果更长，它会截断。
# 返回 shape (num_samples, max_length, embedding_dim) 为 的张量。
def text_to_vector(texts, model, max_length):
    vectors = []
    for text in texts:
        vecs = [model.wv[word] for word in text if word in model.wv]
        # 填充和截断
        vecs = vecs[:max_length] + [np.zeros(model.vector_size)] * max(max_length - len(vecs), 0)
        vectors.append(vecs)
    vectors = np.array(vectors)
    return torch.tensor(vectors, dtype=torch.float32)


def train_model(model, train_loader, test_loader, epochs, output_dim):
    train_accuracies = []
    train_mses = []
    train_losses = []
    test_accuracies = []
    test_mses = []
    # 训练模型
    # 训练循环：运行指定数量的 epoch。
    # 将模型设置为训练模式 （model.train（））。
    # 对于每个批次：
    #   重置渐变。
    #   计算模型输出。
    #   计算损失。
    #   执行反向传播。
    #   更新模型参数。
    for epoch in range(epochs):
        model.train()
        all_preds = []
        all_labels = []
        epoch_loss = 0.0  # 初始化每个 epoch 的损失

        for inputs, labels in train_loader:
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            loss = criterion(outputs, labels)  # 计算损失
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            epoch_loss += loss.item()  # 累加损失
            _, predicted = torch.max(outputs, 1)
            all_preds.extend(predicted.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())

        # 计算训练集上的准确率和 MSE
        train_accuracy = accuracy_score(all_labels, all_preds)
        train_mse = mean_squared_error(all_labels, all_preds)  # 使用预测和真实标签计算 MSE

        # 记录训练集的指标
        train_accuracies.append(train_accuracy)
        train_mses.append(train_mse)
        train_losses.append(epoch_loss / len(train_loader))  # 计算平均损失

        # 测试过程
        test_accuracy, test_mse, all_test_labels, all_test_preds = evaluate_model(model, test_loader, output_dim)
        test_accuracies.append(test_accuracy)
        test_mses.append(test_mse)

        print(
            f'Epoch {epoch + 1}/{epochs},Train Loss: {epoch_loss / len(train_loader):.4f},Train MSE: {train_mse:.4f}, '
            f'Train Accuracy: {train_accuracy:.4f},   Test Accuracy: {test_accuracy:.4f}, Test MSE: {test_mse:.4f}')

    return train_accuracies, train_mses, train_losses, test_accuracies, test_mses, all_test_labels, all_test_preds


# 评估模型
# 在测试数据集上评估模型。
# 将模型设置为评估模式 （model.eval（）），这将禁用 dropout 和批量规范化。
# 迭代测试 DataLoader，计算预测，并收集真实标签和预测标签。
# 打印每个样本的真实标签和预测标签。
# 使用 Scikit-learn 中的 accuracy_score 返回准确率分数。
def evaluate_model(model, test_loader, output_dim):
    model.eval()
    all_preds = []
    all_labels = []
    all_probs = []  # 确保初始化为列表

    with torch.no_grad():
        for inputs, labels in test_loader:
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            _, predicted = torch.max(outputs, 1)
            all_preds.extend(predicted.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())
            all_probs.append(outputs.cpu().numpy())  # 收集输出概率

            # 打印每个样本的真实标签和预测标签
            # for true_label, pred_label in zip(labels.cpu().numpy(), predicted.cpu().numpy()):
            #     print(f"True label: {true_label}, Predicted label: {pred_label}")
        # 统计每个标签的真实数量和预测数量

    # 计算准确率
    accuracy = accuracy_score(all_labels, all_preds)

    # 计算均方误差 (MSE)
    all_probs = np.vstack(all_probs)  # 将所有概率输出堆叠成一个数组
    one_hot_labels = np.eye(output_dim)[all_labels]  # 将真实标签转换为独热编码
    mse = mean_squared_error(one_hot_labels, all_probs)  # 计算MSE

    return accuracy, mse, all_labels, all_preds


def plot_metrics(train_accuracies, train_mses, train_losses, test_accuracies, test_mses, all_test_labels,
                 all_test_preds):
    epochs = range(1, len(train_accuracies) + 1)

    plt.figure(figsize=(15, 5))

    # 准确率图
    plt.subplot(1, 3, 1)
    plt.plot(epochs, train_accuracies, label='Train Accuracy', marker='o')
    plt.plot(epochs, test_accuracies, label='Test Accuracy', marker='o')
    plt.title('Accuracy over Epochs')
    plt.xlabel('Epochs')
    plt.ylabel('Accuracy')
    plt.xticks(epochs)
    plt.legend()
    plt.grid()

    # MSE图
    plt.subplot(1, 3, 2)
    plt.plot(epochs, train_mses, label='Train MSE', marker='o')
    plt.plot(epochs, test_mses, label='Test MSE', marker='o')
    plt.title('MSE over Epochs')
    plt.xlabel('Epochs')
    plt.ylabel('MSE')
    plt.xticks(epochs)
    plt.legend()
    plt.grid()

    # 损失图
    plt.subplot(1, 3, 3)
    plt.plot(epochs, train_losses, label='Train Loss', marker='o', color='red')
    plt.title('Loss over Epochs')
    plt.xlabel('Epochs')
    plt.ylabel('Loss')
    plt.xticks(epochs)
    plt.legend()
    plt.grid()
    plt.tight_layout()
    plt.show()

    true_counts = np.bincount(all_test_labels, minlength=output_dim)
    pred_counts = np.bincount(all_test_preds, minlength=output_dim)

    # 绘制真实标签与预测标签数量的比较图
    labels = np.arange(output_dim)  # 标签的索引
    x = np.arange(len(labels))  # x轴位置

    plt.figure(figsize=(10, 5))
    width = 0.35  # 条形宽度

    # 绘制条形图
    plt.bar(x - width / 2, true_counts, width, label='True Counts', alpha=0.7, color='blue')
    plt.bar(x + width / 2, pred_counts, width, label='Predicted Counts', alpha=0.7, color='orange')

    plt.title('True vs Predicted Counts per Label')
    plt.xlabel('Labels')
    plt.ylabel('Counts')
    plt.xticks(x, labels)
    plt.legend(loc='best')
    plt.grid(axis='y')
    plt.tight_layout()
    plt.show()


# 标签编码
# 使用 LabelEncoder 将字符串标签编码为整数。
le = LabelEncoder()
train_labels_encoded = le.fit_transform(train_labels)
test_labels_encoded = le.transform(test_labels)
# 将编码后的标签转换为 PyTorch 张量，确保它们是 long 类型，以便与损失函数兼容。
Y_train_tensor = torch.tensor(train_labels_encoded, dtype=torch.long)  # 确保标签是 long 类型
Y_test_tensor = torch.tensor(test_labels_encoded, dtype=torch.long)  # 确保标签是 long 类型

# 训练Word2Vec模型
# 在训练文本上训练 Word2Vec 模型。
# vector_size：单词向量的维度。
# window：句子中当前单词和预测单词之间的最大距离。
# min_count：忽略总频率低于此频率的所有单词。
# workers：用于训练模型的工作线程数。
word2vec_model = Word2Vec(sentences=train_texts, vector_size=100, window=5, min_count=1, workers=8)

# 将文本转换为Word2Vec向量
# 使用先前训练的 Word2Vec 模型将训练和测试文本转换为矢量表示。
# 确保所有序列的长度相同 （max_length）
max_length = 100  # 最大序列长度
X_train_vec = text_to_vector(train_texts, word2vec_model, max_length)
X_test_vec = text_to_vector(test_texts, word2vec_model, max_length)

# 超参数
# 设置 LSTM 模型的超参数：
# embedding_dim：应与 Word2Vec 向量大小匹配。
# hidden_dim：处于 LSTM 隐藏状态的特征数。
# output_dim：训练标签中唯一类的数量。
embedding_dim = 100  # 与Word2Vec的vector_size一致
hidden_dim = 64
output_dim = len(np.unique(train_labels_encoded))

# 实例化模型
model = LSTMModel(embedding_dim, hidden_dim, output_dim).to(device)

# 创建数据加载器
# 将训练数据包装到 TensorDataset 中，以便于批处理。
# 为训练数据集创建 DataLoader，允许进行批处理和随机排序。
train_dataset = TensorDataset(X_train_vec.to(device), Y_train_tensor.to(device))
train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)



num_epochs = 10
# 评估模型
test_dataset = TensorDataset(X_test_vec, Y_test_tensor)
test_loader = DataLoader(test_dataset, batch_size=64, shuffle=True)
# 使用示例
train_accuracies, train_mses, train_losses, test_accuracies, test_mses, all_test_labels, all_test_preds = train_model(
    model, train_loader, test_loader,
    num_epochs, output_dim)

plot_metrics(train_accuracies, train_mses, train_losses, test_accuracies, test_mses, all_test_labels, all_test_preds)
