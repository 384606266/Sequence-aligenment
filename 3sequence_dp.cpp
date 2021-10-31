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
    string seq_3;
    int ***graph;
    int height;
    int col;
    int row;

public:
    TSA(string seq1, string seq2, string seq3);
    ~TSA()
    {
        //释放三维数组
        for (int i = 0; i <= height; i++)
            for (int j = 0; j <= row; j++)
                delete[] graph[i][j];
        for (int i = 0; i <= height; i++)
            delete[] graph[i];
        delete[] graph;
    }
    int tripleMessage(void);
};

TSA::TSA(string seq1, string seq2, string seq3)
{
    seq_1 = seq1;
    seq_2 = seq2;
    seq_3 = seq3;
    height = seq_3.size(); //i,j,k
    row = seq_2.size();
    col = seq_1.size();
    //初始化三维数组
    graph = new int **[height];
    for (int i = 0; i <= height; i++)
    {
        graph[i] = new int *[row];
    }
    for (int i = 0; i <= height; i++)
    {
        for (int j = 0; j <= row; j++)
        {
            graph[i][j] = new int[col];
        }
    }
    //初始化三条边
    for (int i = 0; i <= height; i++)
        graph[i][0][0] = i * 4;
    for (int j = 0; j <= row; j++)
        graph[0][j][0] = j * 4;
    for (int k = 0; k <= col; k++)
        graph[0][0][k] = k * 4;

    //初始化三个平面
    int cost;
    for (int j = 1; j <= row; j++)
    { //平面yx
        for (int k = 1; k <= col; k++)
        {
            if (seq_2[j - 1] == seq_1[k - 1])
                cost = 0;
            else
                cost = 3;
            graph[0][j][k] = min(graph[0][j - 1][k] + 4,
                                 min(graph[0][j][k - 1] + 4, graph[0][j - 1][k - 1] + cost + 4));
        }
    }
    for (int i = 1; i <= height; i++)
    { //平面zx
        for (int k = 1; k <= col; k++)
        {
            if (seq_3[i - 1] == seq_1[k - 1])
                cost = 0;
            else
                cost = 3;
            graph[i][0][k] = min(graph[i - 1][0][k] + 4,
                                 min(graph[i][0][k - 1] + 4, graph[i - 1][0][k - 1] + cost + 4));
        }
    }
    for (int i = 1; i <= height; i++)
    { //平面zy
        for (int j = 1; j <= row; j++)
        {
            if (seq_2[j - 1] == seq_3[i - 1])
                cost = 0;
            else
                cost = 3;
            graph[i][j][0] = min(graph[i][j - 1][0] + 4,
                                 min(graph[i - 1][j][0] + 4, graph[i - 1][j - 1][0] + cost + 4));
        }
    }
}

int TSA::tripleMessage(void)
{
    int cost_12, cost_13, cost_23;
    int cost_123;
    int array[7];
    for (int i = 1; i <= height; i++)
    {
        for (int j = 1; j <= row; j++)
        {
            for (int k = 1; k <= col; k++)
            {

                if (seq_3[i - 1] == seq_1[k - 1])
                    cost_13 = 0;
                else
                    cost_13 = 3;
                if (seq_3[i - 1] == seq_2[j - 1])
                    cost_23 = 0;
                else
                    cost_23 = 3;
                if (seq_1[k - 1] == seq_2[j - 1])
                    cost_12 = 0;
                else
                    cost_12 = 3;
                cost_123 = cost_13 + cost_23 + cost_12;

                array[0] = graph[i - 1][j][k] + 4;                //z GAP
                array[1] = graph[i][j - 1][k] + 4;                //y GAP
                array[2] = graph[i][j][k - 1] + 4;                //x GAP
                array[3] = graph[i][j - 1][k - 1] + cost_12 + 4;  //x,y match
                array[4] = graph[i - 1][j][k - 1] + cost_13 + 4;  //x,z match
                array[5] = graph[i - 1][j - 1][k] + cost_23 + 4;  //y,z match
                array[6] = graph[i - 1][j - 1][k - 1] + cost_123; //x,y,z match

                graph[i][j][k] = *min_element(array, array + 7);
            }
        }
    }
    return graph[height][row][col];
}

int main()
{
    double time = 0;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nBeginTime); //开始计时

    int i, j = 0;
    string *p[100]; //存放database的字符串
    string *q[2];   //存放query的字符串
    int score[100][100];
    int min_score = 1000;
    int x, y;
    string seq_tmp, seq_1;
    string *seqq = new string();

    ifstream query("MSA_query.txt", ios::in | ios::binary);
    ifstream database("MSA_database.txt", ios::in | ios::binary);
    ofstream result("DP_result_3.txt", ios::app);
    //ofstream outfile(" ", ios::app);

    if (!query.is_open())
    {
        cout << "fail" << endl;
    }
    while (getline(query, seq_tmp) && i < 6) //跳过前面6行
    {
        i++;
    }
    i = 0;
    while (getline(query, *seqq)) //读取query
    {
        q[i] = seqq;
        seqq = new string();
        i++;
    }
    i = 0;
    while (getline(database, *seqq)) //读取database
    {
        p[i] = seqq;
        seqq = new string();
        i++;
    }
    query.close();
    database.close();

    for (int k = 0; k < 2; k++)
    {
        for (i = 0; i < 100; i++)
        {
            for (j = 0; j <= i; j++)
            {
                if (i == j)
                    score[i][i] = 1000;
                else
                {
                    TSA tsa(*q[k], *p[i], *p[j]);
                    score[i][j] = tsa.tripleMessage();
                }
                if (score[i][j] < min_score)
                {
                    min_score = score[i][j]; //record the minsist score
                    x = i;
                    y = j;
                }
            }
            //result<<endl;
            cout << i << "/100" << endl;
        }
        cout << "The minist scroe is:" << min_score << "\t"
             << "(" << x << "," << y << ")" << endl;
        result << "The minist scroe is:" << min_score << "\t"
               << "(" << x << "," << y << ")" << endl;
    }

    QueryPerformanceCounter(&nEndTime);                                                //停止计时
    time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart; //计算程序执行时间单位为s
    cout << "程序执行时间:" << double(time * 1000) << "ms" << endl;
    result << "程序执行时间:" << time * 1000 << "ms" << endl;
    result.close();
    return 0;
}