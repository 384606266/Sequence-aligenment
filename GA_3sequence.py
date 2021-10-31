import numpy as np
import time
from timeit import default_timer as timer

class TSA:
    def __init__(self, seq1, seq2, seq3, CROSSOVER_RATE=0.8, MUTATION_RATE=0.01, N_GENERATIONS=50):
        # 参数初始化
        self.seq_1 = seq1
        self.seq_2 = seq2
        self.seq_3 = seq3
        self.crossover_rate = CROSSOVER_RATE  # 交叉概率
        self.mutation_rate = MUTATION_RATE  # 变异概率
        self.GENERATIONS = N_GENERATIONS  # 迭代次数
        self.popsize = 100
        self.size1 = len(seq1)
        self.size2 = len(seq2)
        self.size3 = len(seq3)
        self.size = self.size1 + self.size2 + self.size3
        # 初始化种群
        self.pop = np.random.randint(7, size=(self.popsize, self.size))

    def fitness(self):
        score = []
        for i in range(self.popsize):
            score.append(0)
            num1 = 0
            num2 = 0
            num3 = 0
            individual = self.pop[i]
            for t in individual:
                if num1+num2+num3 == self.size:
                    break
                if num1 == self.size1:      #满足1gap
                    if num2 == self.size2:
                        t = 4
                    else:
                        t = 5

                if num2 == self.size2:      #满足2gap
                    if num3 == self.size3:
                        t = 6
                    else:
                        t = 4

                if num3 == self.size3:      #满足3gap
                    if num1 == self.size1:
                        t = 5
                    else:
                        t = 6

                
                if t == 6:  # 23gap
                    score[i] += 4
                    num1 += 1
                if t == 5:  # 13gap
                    score[i] += 4
                    num2 += 1
                if t == 4:  # 12gap
                    score[i] += 4
                    num3 += 1
                if t == 3:  # 3gap
                    if self.seq_1[num1] != self.seq_2[num2]:
                        score[i] += 7
                    else:
                        score[i] += 4
                    num1 += 1
                    num2 += 1
                if t == 2:  # 2gap
                    if self.seq_1[num1] != self.seq_3[num3]:
                        score[i] += 7
                    else:
                        score[i] += 4
                    num1 += 1
                    num3 += 1
                if t == 1:  # 1gap
                    if self.seq_2[num2] != self.seq_3[num3]:
                        score[i] += 7
                    else:
                        score[i] += 4
                    num2 += 1
                    num3 += 1
                if t == 0:
                    # match
                    cost = 0
                    if self.seq_1[num1] != self.seq_2[num2]:
                        cost += 3
                    if self.seq_1[num1] != self.seq_3[num3]:
                        cost += 3
                    if self.seq_2[num2] != self.seq_3[num3]:
                        cost += 3
                    score[i] += cost
                    num1 += 1
                    num2 += 1
                    num3 += 1
        return score

    def crossover_and_mutation(self):
        new_pop = []
        for father in self.pop:  # 遍历种群中的每一个个体，将该个体作为父亲
            child = father  # 孩子先得到父亲的全部基因（这里我把一串二进制串的那些0，1称为基因）
            if np.random.rand() < self.crossover_rate:  # 产生子代时不是必然发生交叉，而是以一定的概率发生交叉
                # 再种群中选择另一个个体，并将该个体作为母亲
                mother = self.pop[np.random.randint(self.popsize)]
                cross_points = np.random.randint(
                    low=0, high=self.size)  # 随机产生交叉的点
                # 孩子得到位于交叉点后的母亲的基因
                child[cross_points:] = mother[cross_points:]
            self.mutation(child)  # 每个后代有一定的机率发生变异
            new_pop.append(child)

        return new_pop

    def mutation(self, child):
        if np.random.rand() < self.mutation_rate:  # 以MUTATION_RATE的概率进行变异
            mutate_point = np.random.randint(
                0, self.size)  # 随机产生一个实数，代表要变异基因的位置
            child[mutate_point] = np.random.randint(7)

    def select(self):    # nature selection wrt pop's fitness
        fitness = self.fitness()
        max_element = max(fitness)
        fitness = np.array([(max_element-value)+0.01 for value in fitness])
        idx = np.random.choice(np.arange(self.popsize), size=self.popsize, replace=True,
                               p=(fitness)/(fitness.sum()))
        return self.pop[idx]

    def generation(self):
        best_fitniss = 600
        judge = 10
        for i in range(self.GENERATIONS):
            self.pop = np.array(self.crossover_and_mutation())
            self.pop = self.select()
            fitness = self.fitness()
            min_fitness = np.min(fitness)
            # 如果有“早熟”现象，保留最优值
            if min_fitness < best_fitniss:
                judge = 10
                best_fitniss = min_fitness
            else:
                judge -= 1
            if judge == 0:
                break
        return best_fitniss


if __name__ == "__main__":
    start_time = time.time()  # 开始时间
    # 文件读入，得到seq1和seq2两条字符串
    with open("project1\MSA_database.txt", encoding='utf-8') as file_obj:
        database_lines = file_obj.readlines()

    with open("project1\MSA_query.txt", encoding='utf-8') as file_obj:
        sample_lines_1 = file_obj.readlines()
    sample_lines = sample_lines_1[7:9]

    file_handle = open('GA_result3.txt', mode='a')  # 保存结果

    for idx, seq_1 in enumerate(sample_lines):
        score = [[2000 for i in range(100)]
                 for j in range(100)]  # 得分矩阵

        for idx1, seq_2 in enumerate(database_lines):
            start = timer()
            for idx2, seq_3 in enumerate(database_lines):
                if idx1 == idx2:
                    score[idx1][idx2] = 2000  # 一个大整数，表示该处不存在
                    break

                tsa = TSA(seq_1, seq_2, seq_3)
                score[idx1][idx2] = tsa.generation()   
            end = timer()
            print("{}/100，  运行时间为：{}".format(idx1, end-start))
          
        min_row = [min(row) for row in score]
        min_score = min(min_row)
        min_idx_col = min_row.index(min_score)
        row = score[min_idx_col]
        min_idx_row = row.index(min_score)
        
        file_handle.write("For sequence {},the best score is:{},with the {}th and {}th sequence.\n".format(
            idx, min_score, min_idx_row, min_idx_col))
        print("对于第{}条序列，最小得分为：{}，分别为第{}条序列和第{}条序列。".format(
            idx, min_score, min_idx_row, min_idx_col))
    end_time = time.time()  # 结束时间
    print("time:%f s" % (end_time-start_time))  # 结束时间-开始时间
    file_handle.write("time:%f s \n" % (end_time-start_time))
    file_handle.close()