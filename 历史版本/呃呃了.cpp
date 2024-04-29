#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <vector>
#include <algorithm>
#include <math.h>
#include <queue>

#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
#define INFINITE 0x3f3f3f3f
#define WIN 1
#define LOSE 0

using namespace std;

#define TIME_CONTROL
#ifdef TIME_CONTROL
// 时间控制用的一些参数
int A_SEC = CLOCKS_PER_SEC;
double threshold = 0.95 * A_SEC; // 限界时间
int start_time;
int current_time;
int loop_out_time;
bool time_out = false;
#endif

class Pos // 用于保存某个状态下的所有可能走法
{
public:
	int beginPos[3000][2] = { 0 }, possiblePos[3000][2] = { 0 }, obstaclePos[3000][2] = { 0 };
	int number = -1;
};

class Coor // 用于队列处理的结构体
{
public:
	int x = -1;
	int y = -1;
	int val = 0;
	Coor(int x, int y, int val)
	{
		this->x = x;
		this->y = y;
		this->val = val;
	}
	Coor() {}
};

// 用于保存一个走法
class Move
{
public:
	int x = -1, y = -1;
	int xx = -1, yy = -1;
	int xxx = -1, yyy = -1;
	int color = 0;
	Move() {};
	Move(int x, int y, int xx, int yy, int xxx, int yyy, int color)
	{
		this->x = x;
		this->y = y;
		this->xx = xx;
		this->yy = yy;
		this->xxx = xxx;
		this->yyy = yyy;
		this->color = color;
	}
};

class treeNode
{
public:
	// 每个结点保存如何从上一步（父亲节点）走到当前步（当前节点），而不是保存棋盘状态
	Move move;
	treeNode* father = nullptr;        // 保存父亲节点指针，便于回溯
	Pos pos;                           // 用于保存此结点，能走的走法总数
	treeNode* selected_ch[3000] = { nullptr }; // 保存已选孩子的节点的指针
	int selected = 0; // 保存已选孩子数
	double win = 0;                    // 胜利的次数
	int visit = 0;                     // 访问次数
	int color = 0;
	double UCB = INFINITE;             // UCB值
	treeNode(treeNode* father, int color)
	{
		this->father = father;
		this->move.color = color;
		this->color = -color;
	}
	treeNode(int x, int y, int xx, int yy, int xxx, int yyy, treeNode* father)
	{
		this->father = father;
		this->move.x = x;
		this->move.y = y;
		this->move.xx = xx;
		this->move.yy = yy;
		this->move.xxx = xxx;
		this->move.yyy = yyy;
		this->move.color = father->color;
		this->color = -father->color;
	}
	treeNode() {};
};
treeNode* root;

double f1[32] = { 0.0000, 0.1080, 0.1080, 0.1235, 0.1332, 0.1400,
				 0.1468, 0.1565, 0.1720, 0.1949, 0.2217,
				 0.2476, 0.2680, 0.2800, 0.2884, 0.3000,
				 0.3208, 0.3535, 0.4000, 0.4613, 0.5350,
				 0.6181, 0.7075, 0.8000, 1.0000, 1.0000,
				 1.0000, 1.0000, 1.0000, 1.0000, 1.0000, 1.0000 };
double f2[32] = { 1.0000, 0.3940, 0.3940, 0.3826, 0.3753, 0.3700,
				 0.3647, 0.3574, 0.3460, 0.3294, 0.3098,
				 0.2903, 0.2740, 0.2631, 0.2559, 0.2500,
				 0.2430, 0.2334, 0.2200, 0.2020, 0.1800,
				 0.1550, 0.1280, 0.1000, 0.0000, 0.0000,
				 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };
double f3[32] = { 0.0000, 0.1160, 0.1160, 0.1224, 0.1267, 0.1300,
				 0.1333, 0.1376, 0.1440, 0.1531, 0.1640,
				 0.1754, 0.1860, 0.1944, 0.1995, 0.2000,
				 0.1950, 0.1849, 0.1700, 0.1510, 0.1287,
				 0.1038, 0.0773, 0.0500, 0.0000, 0.0000,
				 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };
double f4[32] = { 0.0000, 0.1160, 0.1160, 0.1224, 0.1267, 0.1300,
				 0.1333, 0.1376, 0.1440, 0.1531, 0.1640,
				 0.1754, 0.1860, 0.1944, 0.1995, 0.2000,
				 0.1950, 0.1849, 0.1700, 0.1510, 0.1287,
				 0.1038, 0.0773, 0.0500, 0.0000, 0.0000,
				 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };
double f5[32] = { 0.0000, 0.2300, 0.2300, 0.2159, 0.2067, 0.2000,
				 0.1933, 0.1841, 0.1700, 0.1496, 0.1254,
				 0.1010, 0.0800, 0.0652, 0.0557, 0.0500,
				 0.0464, 0.0436, 0.0400, 0.0346, 0.0274,
				 0.0190, 0.0097, 0.0000, 0.0000, 0.0000,
				 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };

double C = 2; // UCB值 C
int turnID;                             // 当前轮次
int currBotColor;                       // 本方所执子颜色（1为黑，-1为白，棋盘状态亦同）
vector<pair<int, int>> blackInfo;       // 记录黑子的x和y
vector<pair<int, int>> whiteInfo;       // 记录白子的x和y
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
double mobility[GRIDSIZE][GRIDSIZE] = { 0 }; // 各个空格的灵活度
double D1[2][8][8] = { 0 };
double D2[2][8][8] = { 0 }; // 0 我的值  1 对方的值
int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];
int startX, startY, resultX, resultY, obstacleX, obstacleY;
Coor queQ[1000];
Coor queK[1000];

int visitMax;
double win_cnt;

/*************************************函数声明***********************************************/
inline void Choices(const int& color, Pos& pos);
inline int RandomNumber(const int& l, const int& r);
inline double calculate();
void MCTS_backPropagate(treeNode* leaf, double result);
inline bool inMap(const int& x, const int& y);
inline bool ProcStep(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color, bool check_only);
inline void NoCheckStep(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color);
inline void NoCheckStep(Move& move);
inline void scan();
inline double calculate();
void MCTS(treeNode* root);
double calculate();
inline void initializeD();
inline void movevalue(const int& color);
inline void moble();
double value(const int& color);
inline void cal_c1c2(double& c_1, double& c_2);
inline double t1(const int& color);
inline double t2(const int& color);
inline double m(const int& color);
inline void Clear(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color);
inline void Clear(Move& move);
inline double Win_Value();
inline void MCTS_updateUCB(treeNode* p);
Move MCTS_selectBest(treeNode* root);
inline double MCTS_calcUCB(treeNode* node);
inline void main_init();
double Max_Min_Search(int depth, int now_depth, double alpha, double beta, int now_color);
void MCTS(treeNode* root);
/*************************************函数声明***********************************************/







/*************************************函数定义***********************************************/
void MCTS_backPropagate(treeNode* leaf, double result)
{
	if (leaf->father != nullptr)
	{
		leaf->visit++;
		leaf->win += result;
		MCTS_backPropagate(leaf->father, result);
	}
	else
	{
		leaf->visit++;
		leaf->win += result;
	}
}

void Choices(const int& color, Pos& pos)
{
	pos.number = 0;
	int posCount = 0;
	vector<pair<int, int>>* Info;
	if (color == grid_black)
		Info = &blackInfo;
	else
		Info = &whiteInfo;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 8; j++)
		{
			for (int try1 = 1; try1 < GRIDSIZE; try1++)
			{
				// 尝试终点位置
				int xx = dx[j] * try1 + (*Info)[i].first;
				int yy = dy[j] * try1 + (*Info)[i].second;
				if (!inMap(xx, yy) || gridInfo[xx][yy] != 0)
					break;
				// 寻找障碍物的位置
				for (int k = 0; k < 8; k++)
					for (int try2 = 1; try2 < GRIDSIZE; try2++)
					{
						int xxx = xx + dx[k] * try2;
						int yyy = yy + dy[k] * try2;
						if ((!inMap(xxx, yyy)) || (gridInfo[xxx][yyy] != 0 && !((*Info)[i].first == xxx && (*Info)[i].second == yyy)))
							break;
						if (ProcStep((*Info)[i].first, (*Info)[i].second, xx, yy, xxx, yyy, color, true))
						{
							pos.beginPos[posCount][0] = (*Info)[i].first;
							pos.beginPos[posCount][1] = (*Info)[i].second;
							pos.possiblePos[posCount][0] = xx;
							pos.possiblePos[posCount][1] = yy;
							pos.obstaclePos[posCount][0] = xxx;
							pos.obstaclePos[posCount++][1] = yyy;
						}
					}
			}
		}
	pos.number = posCount;

	blackInfo.clear();
	whiteInfo.clear();

	// 清空记录的数组
	return;
}

inline void main_init()
{
	int x0, y0, x1, y1, x2, y2;

	gridInfo[0][2] = grid_black;
	gridInfo[2][0] = grid_black;
	gridInfo[5][0] = grid_black;
	gridInfo[7][2] = grid_black;

	gridInfo[0][5] = grid_white;
	gridInfo[2][7] = grid_white;
	gridInfo[5][7] = grid_white;
	gridInfo[7][5] = grid_white;

	cin >> turnID;

	currBotColor = grid_white;
	for (int i = 0; i < turnID; i++)
	{
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black;
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false);
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false);
		}
	}
}

inline double MCTS_calcUCB(treeNode* node)
{
	// 传入node结点，返回其UCB值
	double left = node->win / node->visit;
	double right = C * sqrt(log(node->father->visit) * 1.0 / node->visit);
	if (node->color == -currBotColor)
		return left + right;
	else
		return (1 - left + right);
}

Move MCTS_selectBest(treeNode* root)
{
	int max_visit = -1;
	treeNode* select = nullptr;

	for (int i = 0; i < root->selected; i++)
	{
		treeNode* ch = root->selected_ch[i];
		if (max_visit < ch->visit)
		{
			max_visit = ch->visit;
			visitMax = max_visit;
			win_cnt = ch->win;
			select = ch;
		}
	}
	Move move = select->move;
	// 如果一个都选不出来，直接输了，不处理，浪费时间
	return move;
}

inline void MCTS_updateUCB(treeNode* p)
{
	// 更新UCB值，左值+右值
	// 左值描述历史信息，右值描述可能性
	if (p->father == nullptr)
		return;
	// 考虑是否有可能root->visit==0up
	double left = p->win * 1.0 / p->visit;
	double right;
	// if (p->father == root) right = C * sqrt(log(N) / p->visit);
	right = C * sqrt(log(p->father->visit + 1) / p->visit);
	p->UCB = (p->color == currBotColor) ? (1 - left + right) : (left + right);
	return;
}

inline double Win_Value()
{
	double v = calculate();

	double val = 1.0 / (1 + exp(-v));
	return val;
}


inline void Clear(Move& move)
{
	gridInfo[move.xxx][move.yyy] = 0;
	gridInfo[move.xx][move.yy] = 0;
	gridInfo[move.x][move.y] = move.color;
	return;
}

inline void Clear(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color)
{
	gridInfo[x2][y2] = 0;
	gridInfo[x1][y1] = 0;
	gridInfo[x0][y0] = color;
	return;
}

inline double m(const int& color)
{
	double tmp = 0;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			if (gridInfo[i][j] == currBotColor)
			{
				double sum = 0;
				for (int k = 0; k < 8; k++) // 8个方向
					for (int step = 1; step < GRIDSIZE; step++)
					{
						int tmpx = i + dx[k] * step;
						int tmpy = j + dy[k] * step;
						if (gridInfo[tmpx][tmpy] != 0 || !inMap(tmpx, tmpy))
							break;
						sum += mobility[tmpx][tmpy] / step;
					}
				tmp -= 100.0 / (10 + sum);
			}
			if (gridInfo[i][j] == -currBotColor)
			{
				double sum = 0;
				for (int k = 0; k < 8; k++) // 8个方向
					for (int step = 1; step < GRIDSIZE; step++)
					{
						int tmpx = i + dx[k] * step;
						int tmpy = j + dy[k] * step;
						if (gridInfo[tmpx][tmpy] != 0 || !inMap(tmpx, tmpy))
							break;
						sum += mobility[tmpx][tmpy] / step;
					}
				tmp += 100.0 / (10 + sum); // 唯一不同的地方
			}
		}
	}
	return tmp;
}

inline double t2(const int& color)
{
	double tem = 0;
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if (D2[1][i][j] > D2[0][i][j])
				{
					tem += 1;
				}
				else if (D2[1][i][j] < D2[0][i][j])
				{
					tem -= 1;
				}
				else // 步数相等
				{
					if (D2[0][i][j] < 100) // 都能到
					{
						if (color == currBotColor)
							tem += 0.1;
						else
							tem -= 0.1;
					}
				}
			}
		}
	}
	return tem;
}

inline double t1(const int& color)
{
	double tem = 0;
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if (D1[1][i][j] > D1[0][i][j])
				{
					tem += 1;
				}
				else if (D1[1][i][j] < D1[0][i][j])
				{
					tem -= 1;
				}
				else // 步数相等
				{
					if (D1[0][i][j] < 100) // 都能到
					{
						if (color == currBotColor)
							tem += 0.1;
						else
							tem -= 0.1;
					}
				}
			}
		}
	}
	return tem;
}

inline void cal_c1c2(double& c_1, double& c_2)
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
		{
			// if (!gridInfo[i][j])
			if (D1[0][i][j] != 100 && D1[1][i][j] != 100)
			{
				double tem = (D2[1][i][j] - D2[0][i][j]) / 6.0;
				if (tem < -1)
				{
					c_2 -= 1.0;
				}
				else if (tem >= -1 && tem <= 1)
				{
					c_2 += tem;
				}
				else
				{
					c_2 += 1.0;
				}
			}
			if (D1[0][i][j] != 100 && D1[1][i][j] != 100) // 两边都能到
				c_1 += pow(2, -D1[0][i][j]) - pow(2, -D1[1][i][j]);
		}
	c_1 = 2 * c_1;
	return;
}

double value(const int& color)
{
	double c_1 = 0.0, c_2 = 0.0;
	cal_c1c2(c_1, c_2);
	return f1[turnID] * t1(color) + f2[turnID] * t2(color) + f3[turnID] * c_1 + f4[turnID] * c_2 + f5[turnID] * m(color);
}

inline void moble()
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++) // ij位上的棋子灵活度
		{
			if (gridInfo[i][j] != 0)
				continue; // 如果不是空格就算了
			for (int try1 = 0; try1 < 8; try1++)
			{
				int xx = i + dx[try1];
				int yy = j + dy[try1];
				if (inMap(xx, yy) && gridInfo[xx][yy] == 0)
				{
					mobility[i][j]++; // 它周围的八个位置如果是空格，它的价值就升高
				}
			}
		}
}

inline void movevalue(const int& color)
{
	int headQ = 0, headK = 0, tailQ = 0, tailK = 0;
	int simu_grid1[GRIDSIZE][GRIDSIZE], simu_grid2[GRIDSIZE][GRIDSIZE] = { 0 };
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
			simu_grid1[i][j] = simu_grid2[i][j] = gridInfo[i][j];

	if (color == grid_black) // 读入棋子的位置
	{
		for (int i = 0; i < 4; i++)
		{
			queQ[tailQ++] = Coor(blackInfo[i].first, blackInfo[i].second, 0);
			queK[tailK++] = Coor(blackInfo[i].first, blackInfo[i].second, 0);
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			queQ[tailQ++] = Coor(whiteInfo[i].first, whiteInfo[i].second, 0);
			queK[tailK++] = Coor(whiteInfo[i].first, whiteInfo[i].second, 0);
		}
	}
	int div = color == currBotColor ? 0 : 1;
	// 处理皇后走法，赋值给D1
	while (headQ != tailQ)
	{
		Coor pos = queQ[headQ++];
		//if (simu_grid1[pos.x][pos.y] != 0) continue;
		for (int i = 0; i < 8; i++) // 方向
		{
			for (int k = 1; k < GRIDSIZE; k++)
			{
				int tempx = dx[i] * k + pos.x;
				int tempy = dy[i] * k + pos.y;
				// 不在地图内部，或不是空格，就退出
				if (!inMap(tempx, tempy) || gridInfo[tempx][tempy] != 0)
					break;
				if (simu_grid1[tempx][tempy] != 0)
					continue;
				simu_grid1[tempx][tempy] = 1; // 标记为已访问
				if (pos.val + 1 < D1[div][tempx][tempy])
				{
					D1[div][tempx][tempy] = pos.val + 1;
					queQ[tailQ++] = Coor(tempx, tempy, pos.val + 1);
				}
			}
		}
	}
	// 处理国王走法，赋值给D2
	while (headK != tailK)
	{
		Coor pos = queK[headK++];
		//if (simu_grid2[pos.x][pos.y] == 1) continue;
		for (int i = 0; i < 8; i++) // 方向
		{
			int tempx = dx[i] + pos.x;
			int tempy = dy[i] + pos.y;
			// 不在地图内部，或不是空格，就退出
			// 若已访问过
			if (!inMap(tempx, tempy) || gridInfo[tempx][tempy] != 0 || simu_grid2[tempx][tempy] != 0)
				continue;
			simu_grid2[tempx][tempy] = 1; // 标记为已访问
			if (pos.val + 1 < D2[div][tempx][tempy])
			{
				D2[div][tempx][tempy] = pos.val + 1;
				queK[tailK++] = Coor(tempx, tempy, pos.val + 1);
			}
		}
	}
}

inline void initializeD() // 用于初始化D1/D2数组
{
	blackInfo.clear();
	whiteInfo.clear();
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (gridInfo[i][j] == grid_black)
				blackInfo.push_back(make_pair(i, j));
			else if (gridInfo[i][j] == grid_white)
				whiteInfo.push_back(make_pair(i, j));

			if (gridInfo[i][j] == 0)
			{
				D1[0][i][j] = D1[1][i][j] = D2[0][i][j] = D2[1][i][j] = 100;
			}
			else if (gridInfo[i][j] == currBotColor)
			{
				D1[1][i][j] = D2[1][i][j] = -1;
				D1[0][i][j] = D2[0][i][j] = 0;
			}
			else if (gridInfo[i][j] == -currBotColor)
			{
				D1[0][i][j] = D2[0][i][j] = -1;
				D1[1][i][j] = D2[1][i][j] = 0;
			}
			else
			{
				D1[0][i][j] = D2[0][i][j] = D1[1][i][j] = D2[1][i][j] = -1;
			}
		}
	}
}

double calculate()
{

	memset(mobility, 0, sizeof(mobility)); // 清零灵活度数组
	initializeD();
	movevalue(currBotColor);
	movevalue(-currBotColor);
	moble();                            // 存下来灵活度
	double Value = value(currBotColor); // 查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同
	return Value;
}

inline int RandomNumber(const int& l, const int& r)
{
	// 错误状态
	if (l > r)
		return INFINITE;
	// 返回一个l到r之间的随机整数
	srand(time(nullptr));
	return rand() % (r - l + 1) + l;
}

inline void scan()
{
	blackInfo.clear();
	whiteInfo.clear();
	// arrowInfo.clear();
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
		{
			int address = gridInfo[i][j];
			if (!address)
				continue;
			else if (address == grid_black)
				blackInfo.push_back(make_pair(i, j));
			else if (address == grid_white)
				whiteInfo.push_back(make_pair(i, j));
		}
}

inline void NoCheckStep(Move& move)
{
	gridInfo[move.x][move.y] = 0;
	gridInfo[move.xx][move.yy] = move.color;
	gridInfo[move.xxx][move.yyy] = OBSTACLE;
	return;
}

inline bool inMap(const int& x, const int& y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}

inline bool ProcStep(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2))) // 0或1或2不在棋盘内
	{
		return false;
	}
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0) // 如果原来位置没有该颜色的子，或者目标位置有子
	{
		return false;
	}
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0)) // 放障碍位置有子并且该子不为原位置
	{
		return false;
	}
	if (!check_only) // 只检查就不落子
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}

inline void NoCheckStep(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const int& color)
{
	gridInfo[x0][y0] = 0;
	gridInfo[x1][y1] = color;
	gridInfo[x2][y2] = OBSTACLE;
	return;
}
/*************************************函数定义***********************************************/

/*************************************极大极小主程序***********************************************/
// 负极大极小搜索，a-b剪枝，初始alpha为负无穷，beta为正无穷
double Max_Min_Search(int depth, int now_depth, double alpha, double beta, int now_color)
{
#ifdef TIME_CONTROL
	current_time = clock();
	loop_out_time = current_time - start_time;
	if (loop_out_time > threshold)
	{
		time_out = true;
		return 0;
	}
#endif

	if (now_depth == depth) // 到达层数评估当前棋盘
	{
		int sign = now_depth % 2 == 0 ? 1 : -1;
		double x = sign * calculate();
		return x;
	}

	Pos pos;
	scan();
	Choices(now_color, pos);

	if (pos.number == 0)
	{
		int sign = now_depth % 2 == 0 ? 1 : -1;
		double x = sign * calculate();
		return x;
	}

	double val = 0;
	int choose = 0;
	for (int i = 0; i < pos.number; i++)
	{
#ifdef TIME_CONTROL
		current_time = clock();
		loop_out_time = current_time - start_time;
		if (loop_out_time > threshold)
		{
			time_out = true;

			if (depth == 1)
			{
				startX = pos.beginPos[choose][0];
				startY = pos.beginPos[choose][1];
				resultX = pos.possiblePos[choose][0];
				resultY = pos.possiblePos[choose][1];
				obstacleX = pos.obstaclePos[choose][0];
				obstacleY = pos.obstaclePos[choose][1];
			}

			return 0;
		}
#endif
		int x0, y0, x1, y1, x2, y2;
		NoCheckStep(pos.beginPos[i][0], pos.beginPos[i][1], pos.possiblePos[i][0], pos.possiblePos[i][1], pos.obstaclePos[i][0], pos.obstaclePos[i][1], now_color);

		x0 = pos.beginPos[i][0];
		y0 = pos.beginPos[i][1];
		x1 = pos.possiblePos[i][0];
		y1 = pos.possiblePos[i][1];
		x2 = pos.obstaclePos[i][0];
		y2 = pos.obstaclePos[i][1];

		val = -Max_Min_Search(depth, now_depth + 1, -beta, -alpha, -now_color); // 返回负，从而可以将每层都看作Max层

		// 回溯
		Clear(x0, y0, x1, y1, x2, y2, now_color);

		if (val >= beta)
		{
			return beta; // beta剪枝
		}

		if (val > alpha)
		{
			alpha = val;
			choose = i; // 目前第i种落子最好
		}
	}

	if (now_depth == 0 && !time_out) // 如果当前为根则保存最佳走法
	{
		startX = pos.beginPos[choose][0];
		startY = pos.beginPos[choose][1];
		resultX = pos.possiblePos[choose][0];
		resultY = pos.possiblePos[choose][1];
		obstacleX = pos.obstaclePos[choose][0];
		obstacleY = pos.obstaclePos[choose][1];
	}

	return alpha;
}

/*************************************极大极小主程序***********************************************/


/*************************************MCTS主程序***********************************************/
void MCTS(treeNode* root)
{
	treeNode* p = root;
	if (p->selected == 0) // 若一个都没选过，那么找到所有可行走法。若无走法，返回result；否则随机选一个，rollout返回。
	{
		scan();
		Choices(p->color, p->pos);
		if (p->pos.number == 0) // 达到终局
		{
			if (p->color == currBotColor)
				MCTS_backPropagate(p, LOSE);
			else
				MCTS_backPropagate(p, WIN);
			return;
		}
		else
		{  // 未达到终局
			int select = RandomNumber(0, p->pos.number - 1); // 下标
			treeNode* ch = new treeNode(p->pos.beginPos[select][0], p->pos.beginPos[select][1], p->pos.possiblePos[select][0],
				p->pos.possiblePos[select][1], p->pos.obstaclePos[select][0], p->pos.obstaclePos[select][1], p);
			p->selected_ch[p->selected++] = ch;
			// 用过这个走法了，交换
			swap(p->pos.beginPos[p->selected - 1], p->pos.beginPos[select]);
			swap(p->pos.possiblePos[p->selected - 1], p->pos.possiblePos[select]);
			swap(p->pos.obstaclePos[p->selected - 1], p->pos.obstaclePos[select]);
			// 走一步到ch结点，进行评估，然后复原棋盘
			NoCheckStep(ch->move);
			double result = Win_Value();
			Clear(ch->move);
			MCTS_backPropagate(ch, result);
		}
	}
	else
	{ // 已经选过至少一个点，那么可能全选完了，或还没选完
		int full_size = p->pos.number;

		if (p->selected < full_size) // 还能扩展
		{
			int select = RandomNumber(p->selected, p->pos.number - 1); // 下标，注意-1
			// 以走法创建新节点，然后放入selected_ch
			treeNode* ch = new treeNode(p->pos.beginPos[select][0], p->pos.beginPos[select][1], p->pos.possiblePos[select][0],
				p->pos.possiblePos[select][1], p->pos.obstaclePos[select][0], p->pos.obstaclePos[select][1], p);
			p->selected_ch[p->selected++] = ch;
			// 用过这个走法了，交换
			swap(p->pos.beginPos[p->selected - 1], p->pos.beginPos[select]);
			swap(p->pos.possiblePos[p->selected - 1], p->pos.possiblePos[select]);
			swap(p->pos.obstaclePos[p->selected - 1], p->pos.obstaclePos[select]);

			// 走一步到ch结点，进行评估，然后复原棋盘
			NoCheckStep(ch->move);

			double result = Win_Value();

			// cout << "after result" << endl;

			Clear(ch->move);

			MCTS_backPropagate(ch, result);
		}
		else // 已经完全展开
		{
			// 找ucb最大的点，递归调用MCTS()
			// 选一个UCB最大的点，现用现算
			double max_ucb = -1;
			treeNode* select = nullptr;

			for (int i = 0; i < p->selected; i++)
			{
				treeNode* ch = p->selected_ch[i];
				double ch_ucb = MCTS_calcUCB(ch);
				// 先保存一下，应该不会用的
				ch->UCB = ch_ucb;
				if (ch_ucb > max_ucb)
				{
					max_ucb = ch_ucb;
					select = ch;
				}
			}
			NoCheckStep(select->move);
			MCTS(select);
			Clear(select->move);
		}
	}
}
/*************************************MCTS主程序***********************************************/

/*************************************主程序***********************************************/
int main()
{

#ifdef TIME_CONTROL
	start_time = clock();
	if (turnID == 1)
		threshold *= 2;
#endif

	main_init();

	if (turnID < 12)
		C = 0.01 * turnID; // 动态调整C
	else
		C = 0.30;

	if (1) // 用来进入蒙特卡洛树搜索
	{
		root = new treeNode(nullptr, -currBotColor);
		while (!time_out)
		{

			MCTS(root);
#ifdef TIME_CONTROL
			current_time = clock();
			loop_out_time = current_time - start_time;
			if (loop_out_time > threshold)
			{
				time_out = true;
				break;
			}
#endif
		}	// 时间快到了，出来赋值
		Move move = MCTS_selectBest(root);
		startX = move.x;
		startY = move.y;
		resultX = move.xx;
		resultY = move.yy;
		obstacleX = move.xxx;
		obstacleY = move.yyy;
		cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
		cout << " N=" << root->visit << " w=" << win_cnt << " visit=" << visitMax << endl;
	}
	else
	{
		// 后边是极大极小搜索
		int depth = 1;
		for (depth = 1; depth <= INFINITE; depth++)
		{
			Max_Min_Search(depth, 0, -INFINITE, INFINITE, currBotColor);
#ifdef TIME_CONTROL
			if (time_out)
				break;
#endif
		}
		cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
		cout << "valid depth = " << depth << endl;
	}
	return 0;
}
/*************************************Main主程序***********************************************/