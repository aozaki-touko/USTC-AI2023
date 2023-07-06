import numpy as np
import pandas as pd

class BayesianNetwork:
    def __init__(self, n_labels=10, n_pixels=784, n_values=2) -> None:
        '''
        n_labels: number of labels, 10 for digit recognition
        n_pixels: number of pixels, 256 for 16x16 image
        n_values: number of values for each pixel, 0 for black, 1 for white
        '''
        self.n_labels = n_labels
        self.n_pixels = n_pixels
        self.n_values = n_values
        # prior probability
        self.labels_prior = np.zeros(n_labels)
        self.pixels_prior = np.zeros((n_pixels, n_values))
        # conditional probability
        self.pixels_cond_label = np.zeros((n_pixels, n_values, n_labels))
    

    # fit the model with training data
    def fit(self, pixels:np.ndarray, labels:np.ndarray):
        '''
        pixels: (n_samples, n_pixels, )
        labels: (n_samples, )
        '''
        n_samples = len(labels)
        # TODO: calculate prior probability and conditional probability
        for i in range(self.n_labels):
            self.labels_prior[i] = np.sum(labels == i)

            for j in range(self.n_pixels):
                for k in range(self.n_values):
                    # (pixels[:, j] == k) & (labels == i)
                    # 左边pixels[:, j]代表所有相片中,第j个像素点
                    # 分别计算是0和1的概率
                    self.pixels_cond_label[j, k, i] = np.sum((pixels[:, j] == k) & (labels == i)) / np.sum(labels == i)


        # calculate pixels_prior
        for j in range(self.n_pixels):
            for k in range(self.n_values):
                #计算每个像素是1/0的数目,即为这个值的权重
                self.pixels_prior[j, k] = np.sum(pixels[:, j] == k) / n_samples

    # predict the labels for new data
    def predict(self, pixels):
        '''
        pixels: (n_samples, n_pixels, )
        return labels: (n_samples, )
        '''
        # n_samples = len(pixels)
        # labels = np.zeros(n_samples)
        # # TODO: predict for new data
        #
        # return labels
        n_samples = len(pixels)
        labels = np.zeros(n_samples)
        for i in range(n_samples):
            # calculate posterior probability for each label
            posterior = np.zeros(self.n_labels)
            for j in range(self.n_labels):
                posterior[j] = self.labels_prior[j]
                for k in range(self.n_pixels):
                    #report 说明此概率计算
                    posterior[j] *= self.pixels_cond_label[k, pixels[i, k], j]
            # choose the label with the highest posterior probability
            labels[i] = np.argmax(posterior)
        return labels
    

    # calculate the score (accuracy) of the model
    def score(self, pixels, labels):
        '''
        pixels: (n_samples, n_pixels, )
        labels: (n_samples, )
        '''
        n_samples = len(labels)
        labels_pred = self.predict(pixels)
        return np.sum(labels_pred == labels) / n_samples


if __name__ == '__main__':
    # load data
    print("start")
    train_data = np.loadtxt('../data/train.csv', delimiter=',', dtype=np.uint8)
    test_data = np.loadtxt('../data/test.csv', delimiter=',', dtype=np.uint8)
    pixels_train, labels_train = train_data[:, :-1], train_data[:, -1]
    pixels_test, labels_test = test_data[:, :-1], test_data[:, -1]
    # build bayesian network
    bn = BayesianNetwork()
    bn.fit(pixels_train, labels_train)
    print('test score: %f' % bn.score(pixels_test, labels_test))