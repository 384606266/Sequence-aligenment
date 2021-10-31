# 二维动态规划
import sys
import os
from timeit import default_timer as timer

# 二维数组索引 graph[i][j]  i is col, j is row
# 字符串对比，填规划表


class DSA:
    def __init__(self, seq_1, seq_2):
        self.seq_1 = seq_1
        self.seq_2 = seq_2
        self.graph = [[0 for i in range(len(self.seq_1)+1)]
                      for j in range(len(self.seq_2)+1)]
        # 规划表初始化
        for i in range(len(self.seq_1)+1):
            self.graph[0][i] = i*2
        for i in range(1, len(self.seq_2)+1):
            self.graph[i][0] = i*2

    def Double_compare(self):
        for i in range(1, len(self.seq_1)+1):
            for j in range(1, len(self.seq_2)+1):
                if self.seq_1[i-1] == self.seq_2[j-1]:
                    cost = 0
                else:
                    cost = 3
                self.graph[j][i] = min(
                    self.graph[j-1][i]+2, self.graph[j][i-1]+2, self.graph[j-1][i-1]+cost)  # 下，右，右下

        return self.graph[len(self.seq_2)][len(self.seq_1)]


def DoubleMessage():
    # 双序列比对函数
    tic = timer()
    # 文件读入，得到seq1和seq2两条字符串
    with open("project1\MSA_database.txt", encoding='utf-8') as file_obj:
        database_lines = file_obj.readlines()

    with open("project1\MSA_query.txt", encoding='utf-8') as file_obj:
        sample_lines_1 = file_obj.readlines()
    sample_lines = sample_lines_1[1:6]

    file_handle = open('DP_result_2.txt', mode='a')  # 保存结果

    for idx, seq_1 in enumerate(sample_lines):
        score = []

        for seq_2 in database_lines:
            dsa = DSA(seq_1, seq_2)
            score.append(dsa.Double_compare())

        min_score = min(score)
        min_index = score.index(min_score)
        print("对于第{}条序列，最小得分为：{}，是第{}条序列".format(idx, min_score, min_index))
        file_handle.write("For sequence {},the best score is:{},with the {}th sequence.\n".format(
            idx, min_score, min_index))
    toc = timer()
    print("总运行时间为：", toc - tic)  # 输出的时间，秒为单位
    file_handle.write("time:%f s" % (toc - tic))
    file_handle.close()


class TSA:
    def __init__(self, seq_1, seq_2, seq_3):
        self.seq_1 = seq_1
        self.seq_2 = seq_2
        self.seq_3 = seq_3
        self.graph = [[[0 for k in range(len(self.seq_1)+1)]
                      for j in range(len(self.seq_2)+1)]
                      for i in range(len(self.seq_3)+1)]
        # graph[i][j][k]     i = seq_3, j = seq_2, k = seq_1
        # 初始化三条边
        for i in range(len(self.seq_3)+1):
            self.graph[i][0][0] = i*4
        for i in range(len(self.seq_2)+1):
            self.graph[0][i][0] = i*4
        for i in range(len(self.seq_1)+1):
            self.graph[0][0][i] = i*4
        # 初始化三个平面
        # seq_1和seq_2平面，j，k
        for j in range(1, len(self.seq_2)+1):
            for k in range(1, len(self.seq_1)+1):
                if self.seq_1[k-1] == self.seq_2[j-1]:
                    cost = 0
                else:
                    cost = 3
                self.graph[0][j][k] = min(
                    self.graph[0][j-1][k]+4, self.graph[0][j][k-1]+4, self.graph[0][j-1][k-1]+cost+4)  # 前，右，右前

        # seq_1和seq_3的平面，i，k
        for i in range(1, len(self.seq_3)+1):
            for k in range(1, len(self.seq_1)+1):
                if self.seq_1[k-1] == self.seq_3[i-1]:
                    cost = 0
                else:
                    cost = 3
                self.graph[i][0][k] = min(
                    self.graph[i-1][0][k]+4, self.graph[i][0][k-1]+4, self.graph[i-1][0][k-1]+cost+4)  # 上，右，右上

        # seq_2和seq_3的平面，i3，j2
        for i in range(1, len(self.seq_3)+1):
            for j in range(1, len(self.seq_2)+1):
                if self.seq_2[j-1] == self.seq_3[i-1]:
                    cost = 0
                else:
                    cost = 3
                self.graph[i][j][0] = min(
                    self.graph[i-1][j][0]+4, self.graph[i][j-1][0]+4, self.graph[i-1][j-1][0]+cost+4)  # 上，前，前上

    def Triple_compare(self):
        for i in range(1, len(self.seq_3)+1):
            for j in range(1, len(self.seq_2)+1):
                for k in range(1, len(self.seq_1)+1):
                    if self.seq_1[k-1] == self.seq_2[j-1]:
                        cost_12 = 0
                    else:
                        cost_12 = 3

                    if self.seq_1[k-1] == self.seq_3[i-1]:
                        cost_13 = 0
                    else:
                        cost_13 = 3

                    if self.seq_2[j-1] == self.seq_3[i-1]:
                        cost_23 = 0
                    else:
                        cost_23 = 3
                    cost_123 = cost_12 + cost_13 + cost_23

                    # 上，前，右，右前，右上，前上，右前上
                    self.graph[i][j][k] = min(self.graph[i-1][j][k]+4, self.graph[i][j-1][k]+4, self.graph[i][j][k-1]+4,
                                              self.graph[i][j -
                                                            1][k-1]+cost_12+4,
                                              self.graph[i-1][j][k-1] +
                                              cost_13+4,
                                              self.graph[i-1][j -
                                                              1][k]+cost_23+4,
                                              self.graph[i-1][j-1][k-1]+cost_123)
        # print(self.graph)
        return self.graph[len(self.seq_3)][len(self.seq_2)][len(self.seq_1)]


def TripleMessage():
    # 三序列比对函数
    start = timer()
    # 文件读入，得到seq1和seq2两条字符串
    with open("project1\MSA_database.txt", encoding='utf-8') as file_obj:
        database_lines = file_obj.readlines()

    with open("project1\MSA_query.txt", encoding='utf-8') as file_obj:
        sample_lines_1 = file_obj.readlines()
    sample_lines = sample_lines_1[7:9]

    for idx, seq_1 in enumerate(sample_lines):
        score = [[0 for i in range(100)]
                 for j in range(100)]  # 得分矩阵

        for idx1, seq_2 in enumerate(database_lines):
            for idx2, seq_3 in enumerate(database_lines):
                if idx1 == idx2:
                    score[idx1][idx2] = 1000  # 一个大整数，表示该处不存在
                    break

                tsa = TSA(seq_1, seq_2, seq_3)
                score[idx1][idx2] = tsa.Triple_compare()

            print("{}/100".format(idx1))

        min_row = [min(row) for row in score]
        min_score = min(min_row)
        min_idx_col = min_row.index(min_score)
        row = score[min_idx_col]
        min_idx_row = row.index(min_score)
        print("对于第{}条序列，最小得分为：{}，分别为第{}条序列和第{}条序列。".format(
            idx, min_score, min_idx_row, min_idx_col))
        # 保存得分矩阵
        with open("project1\Triple_score.txt", 'a') as file_obj:
            file_obj.write("对于第{}条序列，最小得分为：{}，分别为第{}条序列和第{}条序列。\n".format(
                idx, min_score, min_idx_row, min_idx_col))
    end = timer()
    with open("project1\Triple_score.txt", 'a') as file_obj:
        file_obj.write("程序运行时间为： \n".format(end-start))
