import torch
import torch.nn as nn
import numpy as np
from matplotlib import pyplot as plt

class MLP:
    def __init__(self):
        # layer size = [10, 8, 8, 4]
        # 初始化所需参数
        self.W1 = np.random.randn(10, 10)
        self.b1 = np.random.randn(10, 1)
        self.h1 = np.random.randn(10, 1)
        self.W2 = np.random.randn(8, 10)
        self.b2 = np.random.randn(8, 1)
        self.h2 = np.random.randn(8, 1)
        self.W3 = np.random.randn(8, 8)
        self.b3 = np.random.randn(8, 1)
        self.h3 = np.random.randn(8, 1)
        self.W4 = np.random.randn(4, 8)
        self.b4 = np.random.randn(4, 1)
        pass

    def forward(self, x):
        # 前向传播
        self.h1 = self.tanh(np.dot(self.W1, x) + self.b1)
        self.h2 = self.tanh(np.dot(self.W2, self.h1) + self.b2)
        self.h3 = self.tanh(np.dot(self.W3, self.h2) + self.b3)
        z = self.softmax(np.dot(self.W4, self.h3) + self.b4)
        return z

    def backward(self, lr, x, y): # 自行确定参数表
        # 反向传播
        z = self.forward(x)

        self.partial_b4 = (z - y) * self.partial(z)
        self.partial_W4 = np.dot(self.partial_b4, self.h3.T)
        self.partial_b3 = np.dot(self.W4.T, self.partial_b4) * (1 - self.h3 * self.h3)
        self.partial_W3 = np.dot(self.partial_b3, self.h2.T)
        self.partial_b2 = np.dot(self.W3.T, self.partial_b3) * (1 - self.h2 * self.h2)
        self.partial_W2 = np.dot(self.partial_b2, self.h1.T)
        self.partial_b1 = np.dot(self.W2.T, self.partial_b2) * (1 - self.h1 * self.h1)
        self.partial_W1 = np.dot(self.partial_b1, x.T)

        self.W4 = self.W4 - lr * self.partial_W4
        self.b4 = self.b4 - lr * self.partial_b4
        self.W3 = self.W3 - lr * self.partial_W3
        self.b3 = self.b3 - lr * self.partial_b3
        self.W2 = self.W2 - lr * self.partial_W2
        self.b2 = self.b2 - lr * self.partial_b2
        self.W1 = self.W1 - lr * self.partial_W1
        self.b1 = self.b1 - lr * self.partial_b1

    def tanh(self, x):
        return np.tanh(x)
    
    def tanh_derivative(self, x):
        return 1 - np.power(self.tanh(x), 2)
    
    def softmax(self, x):
        return np.exp(x) / np.sum(np.exp(x))

    def partial(self, y):
        return y - y * y
    
    def acc(self, inputs, labels):
        count = 0
        for input, label in zip(inputs, labels):
            y = self.forward(input.reshape(len(input), 1)).T
            if np.argmax(y) == np.argmax(label):
                count += 1
        return count / len(inputs)

    def loss(self, inputs, labels):
        loss = 0.0
        for input, label in zip(inputs, labels):
            y = self.forward(input.reshape(len(input), 1))
            for i in range(len(y)):
                if label[i] == 1:
                    loss += - np.log(y[i][0]) / np.log(2)
                    break
        return loss

class MyNet():
    def __init__(self, mlp):
        self.W1 = torch.tensor(mlp.W1, requires_grad=True, dtype=torch.float64)
        self.W2 = torch.tensor(mlp.W2, requires_grad=True, dtype=torch.float64)
        self.W3 = torch.tensor(mlp.W3, requires_grad=True, dtype=torch.float64)
        self.W4 = torch.tensor(mlp.W4, requires_grad=True, dtype=torch.float64)
        self.b1 = torch.tensor(mlp.b1, requires_grad=True, dtype=torch.float64)
        self.b2 = torch.tensor(mlp.b2, requires_grad=True, dtype=torch.float64)
        self.b3 = torch.tensor(mlp.b3, requires_grad=True, dtype=torch.float64)
        self.b4 = torch.tensor(mlp.b4, requires_grad=True, dtype=torch.float64)

        self.tanh = nn.Tanh()
        self.softmax = nn.Softmax(dim=0)

    def forward(self,x):
        x = x.reshape((x.shape[0],1))
        self.x = torch.tensor(x)
        self.h1 = self.tanh(self.W1 @ self.x + self.b1)
        self.h2 = self.tanh(self.W2 @ self.h1 + self.b2)
        self.h3 = self.tanh(self.W3 @ self.h2 + self.b3)
        self.y = self.softmax(self.W4 @ self.h3 + self.b4).T

    def backward(self, label, lr, debug=False):  # 自行确定参数表
        # 反向传播
        label = label.reshape((1,4))
        self.label = torch.tensor(label.T)
        self.loss = -torch.log2(self.y[self.label.T == 1])
        # print(self.loss)

        self.loss.backward()
        self.W1 = self.W1 - lr * self.W1.grad
        self.W1.retain_grad()
        self.W2 = self.W2 - lr * self.W2.grad
        self.W2.retain_grad()
        self.W3 = self.W3 - lr * self.W3.grad
        self.W3.retain_grad()
        # print(self.W4.grad)
        self.partial_W4 = self.W4.grad
        self.W4 = self.W4 - lr * self.W4.grad
        self.W4.retain_grad()
        self.b1 = self.b1 - lr * self.b1.grad
        self.b1.retain_grad()
        self.b2 = self.b2 - lr * self.b2.grad
        self.b2.retain_grad()
        self.b3 = self.b3 - lr * self.b3.grad
        self.b3.retain_grad()
        self.b4 = self.b4 - lr * self.b4.grad
        self.b4.retain_grad()

def train(mlp: MLP, epochs, lr, inputs, labels):
    '''
        mlp: 传入实例化的MLP模型
        epochs: 训练轮数
        lr: 学习率
        inputs: 生成的随机数据
        labels: 生成的one-hot标签
    '''
    net = MyNet(mlp)
    loss = []
    torch_loss = []
    for i in range(epochs):
        temp_torch_loss = 0.0
        for input, label in zip(inputs, labels):
            mlp.backward(lr, input.reshape(len(input), 1), label.reshape(len(label), 1))

            net.forward(input)
            net.backward(label, lr)
            temp_torch_loss += net.loss
        
        loss.append(mlp.loss(inputs, labels) / len(inputs))
        torch_loss.append(temp_torch_loss.detach().numpy()[0] / len(inputs))
        print('[{}/{}]'.format(i+1,epochs))

    print(mlp.partial_W4)
    print(net.partial_W4)
    return loss, torch_loss

if __name__ == '__main__':
    # 设置随机种子,保证结果的可复现性
    np.random.seed(1)
    mlp = MLP()
    # 生成数据
    inputs = np.random.randn(100, 10)

    # 生成one-hot标签
    labels = np.eye(4)[np.random.randint(0, 4, size=(1, 100))].reshape(100, 4)

    # 训练
    epochs = 100
    lr = 0.01
    loss, torch_loss = train(mlp, epochs, lr, inputs, labels)
    print(loss)
    print(torch_loss)

    plt.figure(1)
    plt.plot(loss, 'r-', label=u'my_mlp')
    plt.plot(torch_loss, 'b-', label=u'torch_mlp')
    plt.legend()
    plt.grid(True)  # 增加格点
    plt.axis('tight')  # 坐标轴适应数据量 axis 设置坐标轴
    plt.xlabel("epoch")  # x轴label
    plt.title('LOSS')
    plt.show()

    # print(mlp.W1)
    # print(mlp.b1)
    # print(mlp.W2)
    # print(mlp.b2)
    # print(mlp.W3)
    # print(mlp.b3)
    # print(mlp.W4)
    # print(mlp.b4)


    
    