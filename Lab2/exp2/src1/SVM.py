import numpy as np
import cvxpy as cp

class SupportVectorMachine:
    def __init__(self, C=1, kernel='Linear', epsilon=1e-4):
        self.C = C
        self.epsilon = epsilon
        self.kernel = kernel

        # Hint: 你可以在训练后保存这些参数用于预测
        # SV即Support Vector，表示支持向量，SV_alpha为优化问题解出的alpha值，
        # SV_label表示支持向量样本的标签。
        # self.SV = None
        # self.SV_alpha = None
        # self.SV_label = None

    def KERNEL(self, x1, x2, d=2, sigma=1):
        #d for Poly, sigma for Gauss
        if self.kernel == 'Gauss':
            K = np.exp(-(np.sum((x1 - x2) ** 2)) / (2 * sigma ** 2))
        elif self.kernel == 'Linear':
            K = np.dot(x1,x2)
        elif self.kernel == 'Poly':
            K = (np.dot(x1,x2) + 1) ** d
        else:
            raise NotImplementedError()
        return K
    
    def fit(self, train_data, train_label):
        '''
        TODO：实现软间隔SVM训练算法
        train_data：训练数据，是(N, 7)的numpy二维数组，每一行为一个样本
        train_label：训练数据标签，是(N,)的numpy数组，和train_data按行对应
        '''
        n = len(train_data)
        
        # 计算核矩阵 K
        K = np.empty([n,n], dtype='float64')
        for i in range(n):
            for j in range(n):
                K[i][j] = self.KERNEL(train_data[i], train_data[j]) * train_label[i] * train_label[j]
        
        # alpha 为 n * 1 的矩阵
        alpha = cp.Variable(shape=(n, 1))

        # 描述所求的优化问题
        objecctive = cp.Maximize(cp.sum(alpha) - (1 / 2) * cp.quad_form(alpha, K))
        
        # 给变量 alpha 添加约束条件
        constraints = [alpha >= 0, alpha <= self.C, train_label @ alpha == 0]

        # 求解问题
        prob = cp.Problem(objecctive, constraints)
        prob.solve(solver=cp.ECOS)

        count = 0
        for i in range(n):
            if alpha.value[i][0] > self.epsilon:
                count += 1
        
        self.SV = np.empty([count,len(train_data[0])], dtype='float64')
        self.SV_alpha = np.empty(count, dtype='float64')
        self.SV_label = np.empty(count, dtype=np.int32)

        # 记录支持向量, 对应的 label, 以及对应的 alpha
        j = 0
        for i in range(n):
            if alpha.value[i][0] > self.epsilon:
                self.SV[j] = train_data[i]
                self.SV_alpha[j] = alpha.value[i][0]
                self.SV_label[j] = train_label[i]
                j = j + 1
        
        # 计算 b
        self.b = self.SV_label[0]
        for i in range(count):
            self.b -= self.SV_alpha[i] * self.SV_label[i] * self.KERNEL(self.SV[i], self.SV[0])
        

    def predict(self, test_data):
        '''
        TODO：实现软间隔SVM预测算法
        train_data：测试数据，是(M, 7)的numpy二维数组，每一行为一个样本
        必须返回一个(M,)的numpy数组，对应每个输入预测的标签，取值为1或-1表示正负例
        '''
        result = np.empty(len(test_data), dtype=np.int32)
        for i in range(len(test_data)):
            classify = 0.0
            for alpha, sv, label in zip(self.SV_alpha, self.SV, self.SV_label):
                classify += alpha * label * self.KERNEL(sv, test_data[i])
            classify += self.b
            if classify > 0:
                result[i] = 1
            else:
                result[i] = -1
        return result
