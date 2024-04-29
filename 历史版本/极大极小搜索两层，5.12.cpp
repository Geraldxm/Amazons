#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <vector>


#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
#define infinite 0x3f3f3f3f

using namespace std;

typedef struct choice {
	int x, y;
	int xx, yy;
	int xxx, yyy;
}choice;

int turnID;
int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];
class Pos
{
public:
	int beginPos[3000][2] = { 0 }, possiblePos[3000][2] = { 0 }, obstaclePos[3000][2] = { 0 };
	int number=0;
};
int first_beginPos[3000][2], first_possiblePos[3000][2], first_obstaclePos[3000][2];
int startX, startY, resultX, resultY, obstacleX, obstacleY;
vector<pair<int, int>> blackInfo; // 记录黑子的x和y
vector<pair<int, int>> whiteInfo; // 记录白子的x和y
vector<pair<int, int>> arrowInfo; // 记录箭的x和y

/*******************************************************************************************************/
int currBotColor; // 本方所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };

// 判断是否在棋盘内
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}

int cx0, cy0, cx1, cy1, cx2, cy2, ccolor;
// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	cx0 = x0; cy0 = y0; cx1 = x1; cy1 = y1; cx2 = x2; cy2 = y2; ccolor = color;
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2))) //0或1或2不在棋盘内
	{
		return false;
	}
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0) //如果原来位置没有该颜色的子，或者目标位置有子
	{
		return false;
	}
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0)) //放障碍位置有子并且该子不为原位置
	{
		return false;
	}
	if (!check_only) //只检查就不落子
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}
/***********************************************************************************************************/

void Clear(int x0, int y0, int x1, int y1, int x2, int y2, int color) //回溯一回合某方的行动
{
	gridInfo[x2][y2] = 0;
	gridInfo[x1][y1] = 0;
	gridInfo[x0][y0] = color;
}

void scan()//扫描棋盘，记录当前子的位置，保存到Info数组中
{
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
			else if (address == OBSTACLE)
				arrowInfo.push_back(make_pair(i, j));
		}
}

Pos Choices(int color) //找出当前所有可能走法,返回走法数
{
	Pos pos;
	scan();
	int posCount = 0;
	vector<pair<int, int>> *Info[2];
	Info[0] = &blackInfo, Info[1] = &whiteInfo;
	int cnt = -1, Cnt = 0;
	cnt = color == grid_black ? 0 : 1; // 0代表黑子，1代表白子
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 8; j++)
		{
			for (int try1 = 1; try1 < GRIDSIZE; try1++)
			{
				int resX = dx[j] * try1 + (*Info[cnt])[i].first; // 尝试终点位置
				int resY = dy[j] * try1 + (*Info[cnt])[i].second;
				if (!inMap(resX, resY) || gridInfo[resX][resY] != 0)
					break;
				// 寻找障碍物的位置
				for (int k = 0; k < 8; k++)
					for (int try2 = 1; try2 < GRIDSIZE; try2++)
					{
						int obsX = resX + dx[k] * try2;
						int obsY = resY + dy[k] * try2;
						if ((!inMap(obsX, obsY)) || (gridInfo[obsX][obsY] != 0 && !((*Info[cnt])[i].first == obsX && (*Info[cnt])[i].second == obsY)))
							break;
						if (ProcStep((*Info[cnt])[i].first, (*Info[cnt])[i].second, resX, resY, obsX, obsY, color, true))
						{
							pos.beginPos[posCount][0] = (*Info[cnt])[i].first;
							pos.beginPos[posCount][1] = (*Info[cnt])[i].second;
							pos.possiblePos[posCount][0] = resX;
							pos.possiblePos[posCount][1] = resY;
							pos.obstaclePos[posCount][0] = obsX;
							pos.obstaclePos[posCount++][1] = obsY;
						}
					}
			}
		}
	pos.number = posCount;
	blackInfo.clear();
	whiteInfo.clear();
	arrowInfo.clear();
	return pos;
}

//评估函数
/*******************************************************************************************************************/


int chess1[4][2] = { 0 };//存放我方棋子位置
int chess2[4][2] = { 0 };//存放对方棋子位置

double mobility[GRIDSIZE][GRIDSIZE] = { 0 };//各个空格的灵活度
double queen[2][8][8] = { 0 };
double king[2][8][8] = { 0 };//0 我的值  1 对方的值

int mostvalue = 0;
int endgame = 1;
//检查落子是否合法

//清除模拟落子，回溯专用，清空顺序与刚才是逆着的


void moble()
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)//ij位上的棋子灵活度
		{
			if (gridInfo[i][j] != 0)continue;//如果不是空格就算了
			for (int delta = 0; delta < 8; delta++)
				if (gridInfo[i + dx[delta]][j + dy[delta]] == 0)mobility[i][j]++;//它周围的八个位置如果是空格，它的价值就升高
		}
}
//计算灵活度,算king值

void movevalue(int color)//通过kingmove和queenmove能到的位置
{
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] != color)continue;//确保此处棋子颜色和评估的相同
			for (int k = 0; k < 8; ++k)
			{
				for (double legnth = 1; legnth < GRIDSIZE; legnth++)
				{
					int xx = i + dx[k] * legnth;//八个方向都走
					int yy = j + dy[k] * legnth;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break; //一旦遇到障碍就停下来
					if (currBotColor == color)
					{
						if (queen[0][xx][yy] == 0)queen[0][xx][yy]++;
						if (1 / legnth > king[0][xx][yy]) king[0][xx][yy] = 1 / legnth;
					}//queen数组只取1，表示能到；king数组则要评估到这里需要的步数，取倒数
					else if (currBotColor == -color)
					{
						if (queen[0][xx][yy] != 0)endgame = 0;//一旦这个地方我方也可到，对方也可到，就没有到残局阶段
						if (queen[1][xx][yy] == 0)queen[1][xx][yy]++;
						if (1 / legnth > king[1][xx][yy]) king[1][xx][yy] = 1 / legnth;
					}//queen数组只取1，表示能到；king数组则要评估到这里需要的步数，取倒数
				}
			}
		}
	}
}

double value11(int color)
{
	double tem = 0;
	{
		if (color == currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[0][i][j];//0自己
		else if (color == -currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[1][i][j];//1对面
	}
	return tem;
}
//queenmove,得到t1

double value12(int color)
{
	double tem = 0;
	{
		if (color == currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += king[0][i][j];
		else if (color == -currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += king[1][i][j];
	}
	return tem;
}
//kingmove，得到t2

double value2(int color)
{
	double tem = 0;
	if (currBotColor == color)//如果是我的颜色
	{
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
				if (king[0][i][j] != 0)
					tem = tem + mobility[i][j] * king[0][i][j];
	}

	else if (currBotColor == -color)//如果是对方的颜色
	{
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
				if (king[1][i][j] != 0)
					tem = tem + mobility[i][j] * king[1][i][j];//因为king取得倒数，所以这里可以直接相乘
	}
	return tem;
}
//算出最终灵活度
double value(int color)//查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同
{
	if (endgame == 1)//如果已经到了结束局面，两方各玩各的
		return value11(color);//那么这时候真正起决定性作用的其实是queenmove
	else if (turnID <= 16)
		return 0.14 * value11(color) + 0.37 * value12(color) + 0.13 * value2(color);
	else if (turnID <= 45)
		return 0.3 * value11(color) + 0.25 * value12(color) + 0.2 * value2(color);
	else//最后阶段
		return 0.8 * value11(color) + 0.1 * value12(color) + 0.05 * value2(color);
}//查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同

void memory()//先存下来双方棋盘上棋子的位置信息
{
	int start1 = 0, start2 = 0;
	for (int i = 0; i < GRIDSIZE; ++i)
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] == currBotColor)
			{
				chess1[start1][0] = i;
				chess1[start1++][1] = j;
			}
			else if (gridInfo[i][j] == -currBotColor)
			{
				chess2[start2][0] = i;
				chess2[start2++][1] = j;
			}
		}
}
//先存下来双方棋盘上棋子的位置信息

double calculate()
{
	memory();//存状态
	memset(mobility, 0, sizeof(mobility));//清零灵活度数组
	memset(queen, 0, sizeof(queen));//清零queenmove数组
	memset(king, 0, sizeof(king));//清零kingmove数组
	movevalue(currBotColor);
	movevalue(-currBotColor);
	moble();//存下来灵活度
	double Value1 = value(currBotColor);
	double Value2 = value(-currBotColor);
	double temvalue = Value1 - Value2;//计算灵活度
	return temvalue;
}


/*******************************************************************************************************************/

//负极大极小搜索，a-b剪枝，初始alpha为负无穷，beta为正无穷
double Max_Min_Search(int depth, int now_depth, double alpha, double beta, int color, int now_color, choice& best)
{
	if (now_depth == depth) //到达层数评估当前棋盘
	{
		double x = calculate();
		return x;
	}
	Pos pos;
	pos = Choices(now_color);
	if (pos.number == 0)
	{
		double x = calculate();
		return x;
	}

	double val = 0;
	int choose = 0; //choose需要初值吗？待考虑！

	for (int i = 0; i < pos.number; i++)
	{
		int x0, y0, x1, y1, x2, y2;

		//落子
		if (!ProcStep(pos.beginPos[i][0], pos.beginPos[i][1], pos.possiblePos[i][0], pos.possiblePos[i][1], pos.obstaclePos[i][0], pos.obstaclePos[i][1], now_color, false))
		{
			cout << "fail in procstep" << endl;
			getchar();
			
		}

		x0 = pos.beginPos[i][0]; y0 = pos.beginPos[i][1];
		x1 = pos.possiblePos[i][0]; y1 = pos.possiblePos[i][1];
		x2 = pos.obstaclePos[i][0]; y2 = pos.obstaclePos[i][1];

		val = -Max_Min_Search(depth, now_depth + 1, -beta, -alpha, color, -now_color, best); //返回负，从而可以将每层都看作Max层


		//回溯
		Clear(x0, y0, x1, y1, x2, y2, now_color);

		if (val >= beta)
		{
			return beta; //beta剪枝
		}

		if (val > alpha)
		{
			alpha = val;
			choose = i; //目前第i种落子最好
		}
	}

	if (now_depth == 0) //如果当前为根则保存最佳走法
	{
		best.x = pos.beginPos[choose][0];
		best.y = pos.beginPos[choose][1];
		best.xx = pos.possiblePos[choose][0];
		best.yy = pos.possiblePos[choose][1];
		best.xxx = pos.obstaclePos[choose][0];
		best.yyy = pos.obstaclePos[choose][1];
	}

	return alpha;
}

int main()
{
	int x0, y0, x1, y1, x2, y2;

	// 初始化棋盘
	gridInfo[0][2] = grid_black;
	gridInfo[2][0] = grid_black;
	gridInfo[5][0] = grid_black;
	gridInfo[7][2] = grid_black;

	gridInfo[0][5] = grid_white;
	gridInfo[2][7] = grid_white;
	gridInfo[5][7] = grid_white;
	gridInfo[7][5] = grid_white;

	// 分析自己收到的输入和自己过往的输出，并恢复棋盘状态

	cin >> turnID;

	currBotColor = grid_white; // 先假设自己是白方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合

		// 首先是对手行动
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black; // 第一回合收到坐标是-1, -1，说明我方是黑方
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // 模拟对方落子

																	// 然后是本方当时的行动
																	// 对手行动总比自己行动多一个
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // 模拟本方落子
		}
	}


	/*****************************************************************************************************************/
	/***在下面填充你的代码，决策结果（本方将落子的位置）存入startX、startY、resultX、resultY、obstacleX、obstacleY中*****/

	choice best;
	Pos pos;
	pos = Choices(currBotColor);
	if (pos.number == 0)
	{
		best.x = -1;
		best.y = -1;
		best.xx = -1;
		best.yy = -1;
		best.xxx = -1;
		best.yyy = -1;
	}
	else
	{
		Max_Min_Search(2, 0, -infinite, infinite, currBotColor, currBotColor, best);
	}

	startX = best.x;
	startY = best.y;
	resultX = best.xx;
	resultY = best.yy;
	obstacleX = best.xxx;
	obstacleY = best.yyy;

	/****在上方填充你的代码，决策结果（本方将落子的位置）存入startX、startY、resultX、resultY、obstacleX、obstacleY中****/
	/*****************************************************************************************************************/


	cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;

	return 0;
}