from utils import *
import numpy as np

train_features = np.loadtxt('dataset/svm/svm_train_data.csv', delimiter=',', skiprows=1)
train_labels = np.loadtxt('dataset/svm/svm_train_label.csv', delimiter=',', skiprows=1, dtype=np.int32)
train_labels.reshape([len(train_labels),1])
print(train_labels)
print(train_labels.shape)

# print(train_labels.shape[1])

y = np.array([[-1],[-1],[-1],[-1],[1],[1],[1],[1]])
print(y.shape)

