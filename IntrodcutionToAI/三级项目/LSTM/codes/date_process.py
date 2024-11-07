import os
import jieba
import pickle
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from collections import Counter
import numpy as np
import torch.nn as nn

# 加载数据
train_data = pd.read_csv('../cnews/cnews.train.txt', sep='\t', header=None, names=["label", "content"])
test_data = pd.read_csv('../cnews/cnews.test.txt', sep='\t', header=None, names=["label", "content"])
train_data_path, test_data_path = '../train.txt', '../test.txt'
# 获取内容和标签
train_texts = train_data['content'].values
train_labels = train_data['label'].values
test_texts = test_data['content'].values
test_labels = test_data['label'].values
# 分词
def tokenize(text):
    return list(jieba.cut(text))


def drop_stopword(datas):
    try:
        with open('../stopwords-master/cn_stopwords.txt', 'r', encoding='UTF-8') as f:
            stop_words = [word.strip() for word in f.readlines()]
        print('Successfully loaded stopwords')
    except Exception as e:
        print(f"Error loading stopwords: {e}")
    return [[word for word in data if word not in stop_words] for data in datas]


# 加载数据
def load_saved_data(path):
    data_list = []
    with open(path, 'r', encoding='UTF-8') as lines:
        for line in lines:
            # 移除行尾的换行符，并按逗号分割
            line_data = line.strip().split(',')
            # 将分割后的数据添加到二维列表中
            data_list.append(line_data)
        return data_list

# 保存数据
def save_data(datax, path):
    with open(path, 'w', encoding='UTF-8') as f:
        for lines in datax:
            f.write(','.join(map(str, lines)) + '\n')
    print('Successfully saved data')


if os.path.exists(train_data_path) and os.path.exists(test_data_path):
    print('Loading preprocessed data...')
    train_texts = load_saved_data(train_data_path)
    test_texts = load_saved_data(test_data_path)
else:


    # 分词
    train_texts = [tokenize(text) for text in train_texts]
    test_texts = [tokenize(text) for text in test_texts]
    # 筛选
    train_texts = drop_stopword(train_texts)
    test_texts = drop_stopword(test_texts)
    save_data(train_texts, train_data_path)
    save_data(test_texts, test_data_path)
    print('Successfully saved data')
