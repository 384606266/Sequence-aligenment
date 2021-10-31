#include <vector>
#include <list>
#include <cstdlib>
#include <string>
#include <windows.h>
#include <fstream>
#include <iostream>

using namespace std;

const int GAPCOST = 2;	 //gap消耗
const int MATCHCOST = 3; //dismatch消耗

struct Point
{
	int x, y;															 //点坐标，这里为了方便按照C++的数组来计算，x代表横排，y代表竖列
	int F, G, H;														 //F=G+H
	Point *parent;														 //parent的坐标，这里没有用指针，从而简化代码
	Point(int _x, int _y) : x(_x), y(_y), F(0), G(0), H(0), parent(NULL) //变量初始化
	{
	}
};

class Astar
{
public:
	Astar(string seq1, string seq2);
	list<Point *> GetPath(Point &startPoint);
	Point *findPath(Point &startPoint);

private:
	vector<Point *> getSurroundPoints(const Point *point) const;
	bool isCanreach(const Point *point, const Point *target) const;		  //判断某点是否可以用于下一步判断
	Point *isInList(const list<Point *> &list, const Point *point) const; //判断开启/关闭列表中是否包含某点
	Point *getLeastFpoint();											  //从开启列表中返回F值最小的节点
	//计算FGH值
	int calcG(Point *temp_start, Point *point);
	int calcH(Point *point);
	int calcF(Point *point);

private:
	list<Point *> openList; //开启列表
	//list<Point *> closeList; //关闭列表
	string seq1;
	string seq2;
	int col; //列数，seq1
	int row; //行数，seq2
	vector<vector<bool>> graph;
};

Astar::Astar(string _seq1, string _seq2)
{
	seq1 = _seq1;
	seq2 = _seq2;
	col = _seq1.size();
	row = _seq2.size();
	graph = vector<vector<bool>>(row + 1, vector<bool>(col + 1, false));
}

int Astar::calcG(Point *temp_start, Point *point) //temp为当前节点，point为父节点
{
	int extraG = 0;
	if ((temp_start->x - point->x) + (temp_start->y - point->y) == 1)
		extraG = GAPCOST;
	else if (seq1[temp_start->x - 1] != seq2[temp_start->y - 1])
		extraG = MATCHCOST;
	//int parentG = point->parent == NULL ? 0 : point->parent->G; //如果是初始节点，则其父节点是空
	int parentG = point->G;
	return parentG + extraG;
}

int Astar::calcH(Point *point)
{
	//设匹配成功率为0.8，不足处GAP
	return (max((col - point->x), (row - point->y)) - min((col - point->x), (row - point->y))) * GAPCOST + 0.2 * MATCHCOST * min((col - point->x), (row - point->y));
}

int Astar::calcF(Point *point)
{
	return point->G + point->H;
}

Point *Astar::getLeastFpoint() //获取openlist的首部元素
{
	if (!openList.empty())
	{
		auto resPoint = openList.front();
		for (auto &point : openList) //遍历找最小值
			if (point->F < resPoint->F)
				resPoint = point;
		return resPoint;
	}
	return NULL;
}

Point *Astar::findPath(Point &startPoint)
{
	openList.push_back(new Point(startPoint.x, startPoint.y)); //置入起点,拷贝开辟一个节点，内外隔离
	while (!openList.empty())
	{
		auto curPoint = getLeastFpoint(); //找到F值最小的点
		openList.remove(curPoint);		  //从开启列表中删除
		graph[curPoint->y][curPoint->x] = true;
		//1,找到当前周围三个格中可以通过的格子
		auto surroundPoints = getSurroundPoints(curPoint);
		for (auto &target : surroundPoints)
		{
			//2,对某一个格子，如果它不在开启列表中，加入到开启列表，设置当前格为其父节点，计算F G H
			int tempG = calcG(target, curPoint);
            int tempH = calcH(target);
            int tempF = tempG+tempH;
            if(tempF > (curPoint->F+3)&& (curPoint->F!=0))  {   //采用部分扩展
                
                continue;}
            Point *t = isInList(openList, target);
            if (!t)
            {
                target->parent = curPoint;

                target->G = tempG;
                target->H = tempH;
                target->F = calcF(target);

                openList.push_back(target);
            }
            //3，对某一个格子，它在开启列表中，计算G值, 如果比原来的大, 就什么都不做, 否则设置它的父节点为当前点,并更新G和F
            else
            {
                if (tempG < t->G)
                {
                    t->parent = curPoint;
                    t->G = tempG;
                    t->F = calcF(t);
                }
            }

			if (target->x == col && target->y == row)
				return target;
		}
	}

	return NULL;
}

list<Point *> Astar::GetPath(Point &startPoint)
{
	Point *result = findPath(startPoint);
	list<Point *> path;
	//返回路径，如果没找到路径，返回空链表
	while (result)
	{
		path.push_front(result);
		result = result->parent;
	}

	// 清空临时开闭列表，防止重复执行GetPath导致结果异常
	openList.clear();
	//closeList.clear();

	return path;
}

Point *Astar::isInList(const list<Point *> &list, const Point *point) const
{
	//判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标
	for (auto p : list)
		if (p->x == point->x && p->y == point->y)
			return p;
	return NULL;
}

bool Astar::isCanreach(const Point *point, const Point *target) const
{
	if (target->x > col || target->y > row || target->x == point->x && target->y == point->y || graph[target->y][target->x] == true) //如果点与当前节点重合、超出地图、在close表返回false
		return false;
	else
		return true;
}

vector<Point *> Astar::getSurroundPoints(const Point *point) const
{
	vector<Point *> surroundPoints;

	for (int x = point->x; x <= point->x + 1; x++)
		for (int y = point->y; y <= point->y + 1; y++)
			if (isCanreach(point, new Point(x, y)))
				surroundPoints.push_back(new Point(x, y));

	return surroundPoints;
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
	ofstream result("Astar_result_2.txt", ios::app);

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
	Point start(0, 0);
	for (j = 0; j < 5; j++)
	{
		min_score = 1000;
		for (i = 0; i < 100; i++)
		{
			Astar astar(*q[j], *data[i]);
			score[i] = astar.findPath(start)->G;
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
	cout << "The time is：" << double(time * 1000) << "ms" << endl;
	result << "The time is：" << time * 1000 << "ms" << endl;
	result.close();

	return 0;
}