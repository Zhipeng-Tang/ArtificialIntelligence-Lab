import torch
import torchvision

import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

from torchvision import transforms
import numpy as np

def loss(self, inputs, labels):
    loss = 0.0
    for input, label in zip(inputs, labels):
        y = self.forward(input.reshape(len(input), 1))
        for i in range(len(y)):
            if label[i] == 1:
                loss += - np.log(y[i][0]) / np.log(2)
                break
    return loss
y = np.array([0.7,0.1,0.1,0.1])
label = np.array([1,0,0,0])

loss = 0.0
for i in range(len(y)):
    if label[i] == 1:
        loss += - np.log(y[i]) / np.log(2)
        break
print(loss)

print(np.log2(y[label.T==1]))
