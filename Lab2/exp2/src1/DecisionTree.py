from cProfile import label
import numpy as np

values = [0,1,2,3,4,5,6,7,8,9,10]
attributes = [0,1,2,3,4,5,6,7,8]

class DecisionTree:

    def fit(self, train_features, train_labels):
        '''
        TODO: 实现决策树学习算法.
        train_features是维度为(训练样本数,属性数)的numpy数组
        train_labels是维度为(训练样本数, )的numpy数组
        '''
        attrs = attributes
        features_list = []
        labels_list = []
        for feature, label in zip(train_features, train_labels):
            temp_feature = []
            for item in feature:
                temp_feature.append(item)
            features_list.append(temp_feature)
            labels_list.append(label)
        self.node = self.TreeGenerate(features_list, labels_list, attrs)

    def TreeGenerate(self, features, labels, attrs):
        label_list = []
        for label in labels:
            if label not in label_list:
                label_list.append(label)
            if len(label_list) == 2:
                break
        
        if len(label_list) == 1:
            return DecisionTreeNode(type=0, label=label_list[0])
        
        if len(attrs) == 0:
            return DecisionTreeNode(type=0, label=self.get_most(labels))

        is_same_feature = True
        first_feature = features[0]
        for feature in features:
            for attr in attrs:
                if feature[attr] != first_feature[attr]:
                    is_same_feature = False
        if is_same_feature:
            return DecisionTreeNode(type=0, label=self.get_most(labels))

        # 目标：信息熵增量最大
        # entropy 为信息熵
        entropy = self.get_entropy(labels)
        # 被选中的属性
        selected_attr = 0
        # 选中 selected_attr 后的信息熵增量
        gain_entropy = 0

        # 遍历剩余属性
        for attr in attrs:
            # temp_entropy 为选中属性 attr 情况下的信息熵
            temp_entropy = 0
            # 计算 temp_entropy
            for value in values:
                temp_label_list = []
                for feature, label in zip(features, labels):
                    if (feature[attr] == value):
                        temp_label_list.append(label)
                if len(temp_label_list) != 0:
                    temp_entropy += len(temp_label_list) / len(features) * self.get_entropy(temp_label_list)
            
            # 如果选中属性 attr 的信息熵增量大于当前所找到的最优属性的信息熵增量
            # 则，attr 成为新的最优属性
            if (entropy - temp_entropy > gain_entropy):
                gain_entropy = entropy - temp_entropy
                selected_attr = attr
        
        node = DecisionTreeNode(type=1, attr=selected_attr, children={})

        sub_attrs = []
        for attr in attrs:
            if attr != selected_attr:
                sub_attrs.append(attr)
        
        for value in values:
            sub_features = []
            sub_labels = []
            for feature, label in zip(features, labels):
                if feature[selected_attr] == value:
                    sub_features.append(feature)
                    sub_labels.append(label)
            
            if len(sub_features) == 0:
                node.children[value] = DecisionTreeNode(type=0, label=self.get_most(labels))
            else:
                node.children[value] = self.TreeGenerate(features=sub_features, labels=sub_labels, attrs=sub_attrs)
        return node

    def get_entropy(self, labels):
        l0, l1 = self.get_label_distribution(labels)
        entropy = 0.0
        if l0 != 0:
            entropy += -l0 / (l0 + l1) * np.log(l0 / (l0 + l1)) / np.log(2)
        if l1 != 0:
            entropy += -l1 / (l0 + l1) * np.log(l1 / (l0 + l1)) / np.log(2)
        return entropy
        
    def get_label_distribution(self, labels):
        l0 = 0
        l1 = 0
        for label in labels:
            if label == 0:
                l0 += 1
            else:
                l1 += 1
        return l0, l1
            
    def get_most(self, labels):
        l0 = 0
        l1 = 0
        for label in labels:
            if label == 0:
                l0 += 1
            else:
                l1 += 1
        if l0 >= l1:
            return 0
        else:
            return 1

    def predict(self, test_features):
        '''
        TODO: 实现决策树预测.
        test_features是维度为(测试样本数,属性数)的numpy数组
        该函数需要返回预测标签，返回一个维度为(测试样本数, )的numpy数组
        '''
        labels = np.empty(len(test_features), dtype='int64')
        for i in range(len(test_features)):
            prediction = self.get_prediction_result(test_features[i])
            labels[i] = prediction
        return labels

    def get_prediction_result(self, feature):
        node = self.node
        while(node.type == 1):
            node = node.children[feature[node.attr]]
        return node.label


# treenode: [attr, feat[attr] == 1, feat[attr] == 2, feat[attr] == 3]

class DecisionTreeNode:
    def __init__(self, type=None, attr=None, children=None, label=None):
        self.type = type
        self.attr = attr
        self.children = children
        self.label = label