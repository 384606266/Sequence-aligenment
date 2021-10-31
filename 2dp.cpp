#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <codecvt>
#include <windows.h>

using namespace std;

class TSA
{
private:
	string seq_1;
	string seq_2;
	int **graph;
	int col;
	int row;

public:
	TSA(string seq1, string seq2);
	~TSA()
	{
		//释放二维数组
		for (int j = 0; j <= row; j++)
			delete[] graph[j];
		delete[] graph;
	}
	int doubleMessage(void);
};

TSA::TSA(string seq1, string seq2)
{
	seq_1 = seq1;
	seq_2 = seq2;
	row = seq_2.size();
	col = seq_1.size();
	//初始化二维数组
	graph = new int *[row];
	for (int j = 0; j <= row; j++)
		graph[j] = new int[col];
	//初始化两条边
	for (int j = 0; j <= row; j++)
		graph[j][0] = j * 2;
	for (int k = 0; k <= col; k++)
		graph[0][k] = k * 2;
}

int TSA::doubleMessage(void)
{
	int cost;
	for (int j = 1; j <= row; j++)
	{ //平面yx
		for (int k = 1; k <= col; k++)
		{
			if (seq_2[j - 1] == seq_1[k - 1])
				cost = 0;
			else
				cost = 3;
			graph[j][k] = min(graph[j - 1][k] + 2, min(graph[j][k - 1] + 2, graph[j - 1][k - 1] + cost));
		}
	}
	return graph[row][col];
}

int main()
{
	double time = 0;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime); //开始计时

	int i, j, min_index;
	int score[100] = {1000};
	int min_score = 1000;
	string seq_tem;
	string *seqq = new string();
	string *data[100]; //存储数据库数组
	string *q[5];	   //存储对比序列
	//导入字符串
	ifstream query("MSA_query.txt", ios::in | ios::binary);
	ifstream database("MSA_database.txt", ios::in | ios::binary);
	ofstream result("DP_result_cpp.txt", ios::app);

	if (!query.is_open())
	{
		cout << "fail" << endl;
	}
	getline(query, seq_tem);
	i = 0;
	while (getline(query, *seqq) && i < 5)
	{
		q[i] = seqq;
		seqq = new string();
		i++;
	}

	i = 0;
	while (getline(database, *seqq))
	{
		data[i] = seqq;
		seqq = new string();
		i++;
	}
	query.close();
	database.close();
	//计算得分

	for (j = 0; j < 5; j++)
	{
		min_score = 1000;
		for (i = 0; i < 100; i++)
		{
			TSA tsa(*q[j], *data[i]);
			score[i] = tsa.doubleMessage();
			if (score[i] < min_score)
			{
				min_score = score[i];
				min_index = i;
			}
		}
		cout << "The minist scroe is" << min_score << "\t"
			 << "(" << min_index << ")" << endl;
		result << "The minist scroe is" << min_score << "\t"
			   << "(" << min_index << ")" << endl;
	}
	QueryPerformanceCounter(&nEndTime);												   //停止计时
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart; //计算程序执行时间单位为s
	cout << "The time is:" << double(time * 1000) << "ms" << endl;
	result << "The time is:" << time * 1000 << "ms" << endl;
	result.close();

	return 0;
}