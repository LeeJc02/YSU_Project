import torch
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import numpy as np
from torch.utils.data import DataLoader, TensorDataset
import matplotlib.pyplot as plt

class Boston(torch.nn.Module):
    def __init__(self, feature_col) -> None:
        super().__init__()
        # 构建神经网络
        self.fc = torch.nn.Sequential(
            # torch.nn.Linear(in_features=feature_col, out_features=128),
            # torch.nn.ReLU(),
            # torch.nn.Linear(in_features=128, out_features=64),
            # torch.nn.ReLU(),
            # torch.nn.Linear(in_features=64, out_features=32),
            # torch.nn.ReLU(),
            # torch.nn.Linear(in_features=32, out_features=1),
            torch.nn.Linear(in_features=feature_col, out_features=64),
            torch.nn.ReLU(),
            torch.nn.Linear(in_features=64, out_features=32),
            torch.nn.ReLU(),
            torch.nn.Linear(in_features=32, out_features=16),
            torch.nn.ReLU(),
            torch.nn.Linear(in_features=16, out_features=1),
        )
        
    def getData(self, data_url, feature_col, test_size, batch_size):
        raw_df = pd.read_csv(data_url, sep="\s+", header=None)
        x_data = raw_df.iloc[:, :feature_col].values
        y_data = raw_df.iloc[:, -1].values
        
        # 归一化
        scalar = StandardScaler().fit(x_data)
        x_data = scalar.transform(x_data)
        
        # 拆分数据为训练集和测试集
        x_train, x_test, y_train, y_test = train_test_split(x_data, y_data, test_size=test_size, random_state=56)

        x_train = torch.from_numpy(x_train.astype(np.float32))
        y_train = torch.from_numpy(y_train.astype(np.float32)).view(-1, 1)
        self.x_test = torch.from_numpy(x_test.astype(np.float32))
        self.y_test = torch.from_numpy(y_test.astype(np.float32)).view(-1, 1)

        # 封装进数据集加载器
        self.train_data = TensorDataset(x_train, y_train)
        self.train_loader = DataLoader(dataset=self.train_data, batch_size=batch_size, shuffle=True, drop_last=True)
    
    # 前向传递函数
    def forward(self, x):
        return self.fc(x)
    
    def trainData(self, epochs, learn_rate, number_of_sig_losser):
        # 使用Adam优化器
        # optimizer = torch.optim.SGD(self.parameters(), lr=learn_rate)
        optimizer = torch.optim.Adam(self.parameters(), lr=learn_rate)
        loss_fun = torch.nn.MSELoss()
        self.train()
        self.loss_values = [] 
        self.epochs = epochs
        
        for epoch in range(epochs):
            epoch_loss = 0
            for step, (x, y) in enumerate(self.train_loader):
                loss = loss_fun(self(x), y) # 计算损失
                loss.backward() # 反向传播
                optimizer.step() # 步进更新参数
                optimizer.zero_grad() # 梯度清零
                epoch_loss += loss.item()
                
            self.loss_values.append(epoch_loss / len(self.train_loader))
            
            if (epoch + 1) % 100 == 0:
                print(f"epoch:{epoch + 1}/{epochs}, loss:{epoch_loss / len(self.train_loader)}")
                
        self.max_loss = max(self.loss_values)
        # print(f"Final average loss:: {sum(self.loss_values[-number_of_sig_losser:-10]) / (number_of_sig_losser):.4f}")
        # return sum(self.loss_values[-number_of_sig_losser:]) / (number_of_sig_losser)
        print(f"Final average loss:: {min(self.loss_values[-number_of_sig_losser:]):.4f}")
        return min(self.loss_values[-number_of_sig_losser:])
        
    def drawCurve(self, save_model_path, save_img_path, local_loss_img_path, number_of_sig_losser):
        torch.save(self.state_dict(), save_model_path)
        
        plt.figure(figsize=(10, 5))
        plt.plot(range(self.epochs), self.loss_values, label='Loss')
        plt.xlabel('Epochs')
        plt.ylabel('Loss')
        plt.title('Loss Value Change')
        plt.legend()
        plt.savefig(save_img_path)
        # plt.show()     
        plt.close()
        
        plt.figure(figsize=(10, 5))
        plt.plot(range(self.epochs - number_of_sig_losser, self.epochs), self.loss_values[-number_of_sig_losser:], label='Loss')
        plt.xlabel('Epochs')
        plt.ylabel('Local Loss')
        plt.title('Local Loss Value Change')
        plt.legend()
        plt.savefig(local_loss_img_path)
        # plt.show()     
        plt.close()
            
    def calMSE(self, scatter_diagram_path, regression_graph_path):
        self.eval()
        with torch.no_grad(): # 评估模式禁止梯度计算
            y_pred = self(self.x_test)  # 获取预测结果
            mse = torch.nn.functional.mse_loss(y_pred, self.y_test)  # 计算均方误差
            print(f'The Test MSE: {mse.item():.4f}')
            
            # 测试散点图
            indices = range(len(self.y_test))
            plt.figure(figsize=(8, 6))
            plt.scatter(indices, y_pred.numpy(), color='blue', label='Predicted Values')
            plt.scatter(indices, self.y_test.numpy(), color='red', label='True Values')
            
            plt.xlabel('Data Point Index')
            plt.ylabel('Values')
            plt.title('Predicted vs True Values')
            plt.legend()
            plt.grid(True)
            plt.savefig(scatter_diagram_path)  
            plt.close()
            
            # 回归散点图
            plt.figure(figsize=(8, 6))
            plt.scatter(self.y_test.numpy(), y_pred.numpy(), color='blue', label='Predicted Values')

            # 绘制理想线
            plt.plot([self.y_test.min(), self.y_test.max()], [self.y_test.min(), self.y_test.max()], 
                    color='red', linewidth=2, label='Ideal Fit')  # 理想线

            # 设置图表的标签和标题
            plt.xlabel('True Values')
            plt.ylabel('Predicted Values')
            plt.title('Predicted vs True Scatter Plot')
            plt.legend()

            # 添加网格线
            plt.grid(True)

            # 保存图像
            plt.savefig(regression_graph_path)
            plt.close()
            
            return mse.item()
            
            
            