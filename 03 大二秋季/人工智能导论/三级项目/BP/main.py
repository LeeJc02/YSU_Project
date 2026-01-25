from model import Boston
import sys
import matplotlib.pyplot as plt
import time
import gc
import torch

# 参数字典
# Best: epochs:5000, learn_rate:4/3 * 1e-3, batch_size:8
parameter = {
    "find_best_parameter" : [9, 5, 9],
    "feature_col" : 13,
    "data_file_path" : "./datas/boston.dat",
    "epochs" : 5000,
    "learn_rate" : 5e-3,
    "number_of_sig_losses" : 30,
    "test_size" : 0.407,
    "batch_size" : 16,
    # "device" : torch.device("cuda:0"),  # 启用gpu
    # "device" : torch.device("cpu"),   # 启用cpu
    "save_model_path" : "./model/boston.pt",
    "save_img_path" : "./imgs/loss_curve.png",
    "local_loss_img_path" : "./imgs/local_loss_img_path.png",
    "scatter_diagram_path" : "./imgs/scatter_diagram.png",
    "regression_graph_path" : "./imgs/Regression_graph.png",
    "value_change_img_path" : "./imgs/value_change.png",
    "train_times" : 10,  # 注意此处不能为1或0 !!!
    "train_model" : 1,  # 1:单次  
    # "train_model" : 2,  # 2:多次
    # "train_model" : 3   # 3:寻找最优参数
}

    
# 初始化-训练-评估
def modelTest(epochs=parameter["epochs"], learn_rate=parameter["learn_rate"], batch_size=parameter["batch_size"]):
    model = Boston(parameter["feature_col"])
    
    model.getData(parameter["data_file_path"], 
                  parameter["feature_col"],
                  parameter["test_size"],
                  batch_size)

    value_loss = model.trainData(epochs, 
                                 learn_rate,
                                 parameter["number_of_sig_losses"])
    
    value_MSE = model.calMSE(parameter["scatter_diagram_path"],
                             parameter["regression_graph_path"])
    
    model.drawCurve(parameter["save_model_path"], 
                    parameter["save_img_path"],
                    parameter["local_loss_img_path"],
                    parameter["number_of_sig_losses"])
    
    del model
    gc.collect()  # 强制进行垃圾回收
    
    return value_MSE, value_loss

# 获取最优及平均MSE和loss
def theBestMSEandLoss():
    ls_MSE = []
    ls_loss = []
    min_MSE = sys.maxsize
    min_loss = sys.maxsize
    max_MSE = -sys.maxsize
    max_loss = -sys.maxsize
    times = parameter["train_times"]
    
    for i in range(times):
        print(f"--------------------{i + 1}-----------------------")
        time_MSE, time_loss = modelTest()
        min_MSE = min(min_MSE, time_MSE)
        min_loss = min(min_loss, time_loss)
        max_MSE = max(max_MSE, time_MSE)
        max_loss = max(max_loss, time_loss)
        
        ls_MSE.append(min_MSE)
        ls_loss.append(min_loss)
        
    aver_MSE = sum(ls_MSE) / len(ls_MSE) 
    aver_loss = sum(ls_loss) / len(ls_loss) 
    
    # 归一化 MSE 和 Loss 值
    norm_MSE = [(mse - min_MSE) / (max_MSE - min_MSE) for mse in ls_MSE]
    norm_loss = [(loss - min_loss) / (max_loss - min_loss) for loss in ls_loss]
    
    # 绘制归一化后的 MSE 和 Loss 值的变化散点图
    plt.figure(figsize=(10, 5))
    plt.scatter(range(1, times + 1), norm_MSE, label='Normalized MSE', color='blue')
    plt.scatter(range(1, times + 1), norm_loss, label='Normalized Loss', color='red', alpha=0.5)
    plt.xlabel('Iteration')
    plt.ylabel('Normalized Value')
    plt.title('Normalized MSE and Loss Value Change')
    plt.legend()
    
    # 保存图片到指定路径
    plt.savefig(parameter["value_change_img_path"])
    # plt.show()
    plt.close()
    
    print()
    print(f"Run {times} times: ")
    print(f"The min loss:{min_loss:.4f}")
    print(f"The average loss:{aver_loss:.4f}")
    print(f"The min MSE:{min_MSE:.4f}")
    print(f"The average MSE:{aver_MSE:.4f}")
    
# 测试epochs, learn_rate, batch_size
def findBestParameter(times):
    min_loss = sys.maxsize
    min_MSE = sys.maxsize
    min_loss_MSE = sys.maxsize
    min_MSE_loss = sys.maxsize
    min_loss_par = []
    min_MSE_par = []
    epoch_unit = 1000
    learn_rate_unit = 1e-3
    
    for i in range(5, times[0] + 1, 2): # epochs i * epoch_unit
        for j in range(3, times[1] + 1): # batch_size pow(2, j)
            for k in range(1, times[2] + 1): # learn_rate k * learn_rate_unit 
                print(f"----epoch:{i * epoch_unit}--batch_size:{pow(2, j)}--lr:{k * learn_rate_unit}----")
                
                value_MSE, value_loss = modelTest(i * epoch_unit, k * learn_rate_unit, pow(2, j))
                
                if min_loss > value_loss:
                    min_loss, min_loss_MSE = value_loss, value_MSE
                    min_loss_par = [i * epoch_unit, k * learn_rate_unit, pow(2, j)]
                    
                if min_MSE > value_MSE:
                    min_MSE, min_MSE_loss = value_MSE, value_loss
                    min_MSE_par = [i * epoch_unit, k * learn_rate_unit, pow(2, j)]

    print(f"lowest loss: ({min_loss:.4f}, {min_loss_MSE:.4f}): {min_loss_par}")
    print(f"lowest MSE:  ({min_MSE_loss:.4f}, {min_MSE:.4f}): {min_MSE_par}")
   
if __name__ == "__main__":
    start_time = time.time()    
    
    if parameter["train_model"] == 1:
        modelTest()     
    elif parameter["train_model"] == 2:
        theBestMSEandLoss()
    else:
        findBestParameter(times=parameter["find_best_parameter"])
        
    end_time = time.time()
    print(f"Take time:{end_time - start_time:.4f} s")