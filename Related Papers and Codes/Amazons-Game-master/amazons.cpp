#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include<iomanip>


#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
double gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int chess1[4][2] = { 0 };//存放我方棋子位置
int chess2[4][2] = { 0 };//存放对方棋子位置

double mobility[GRIDSIZE][GRIDSIZE] = { 0 };//各个空格的灵活度
double queen[2][8][8] = { 0 };
double king[2][8][8] = { 0 };//0 我的值  1 对方的值

int mostvalue = 0;
int turnID;
int endgame = 1;

bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}

bool legal(int x0, int y0, int x1, int y1, int x2, int y2, int color)//检查落子是否合法
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))//这个点不为0且不是移动前的位置（那就一定是有阻挡了）
		return false;
	return true;//其他情况都可以返回真值
}//检查落子是否合法

void putchess(int x0, int y0, int x1, int y1, int x2, int y2, int color)//落子函数
{
	gridInfo[x0][y0] = 0;//移动前位置
	gridInfo[x1][y1] = color;//移动后位置
	gridInfo[x2][y2] = OBSTACLE;//障碍释放区
}//落子函数

void clear(int x0, int y0, int x1, int y1, int x2, int y2, int color)
{
	gridInfo[x2][y2] = 0;//障碍释放区
	gridInfo[x1][y1] = 0;//移动后位置
	gridInfo[x0][y0] = color;//移动前位置
}//清除模拟落子，回溯专用，一定要注意这里的清空顺序，非常重要！！！与刚才是逆着的


void moble()
{
	/*3．3 mobility灵活度特征值的计算在文献[2]中灵活度是用来衡量棋子向相邻八个
		方向移动的灵活性大小的。棋盘中的每个空格和棋
		子都有一个灵活度值，棋子灵活度的计算依赖于棋
		子可达空格的灵活度。图5中空格内的值代表该空
		格的灵活度值，而棋子皇后左上角的数字则表示该
		棋子在此棋局下的灵活度数值。空格的灵活度值为
		与该空格相邻的空格数。例如：图5中左上角(10，)
		空格因为有3 + ~W4g空格，所以该空格的灵活度值为3。
		棋子的灵活度值则与棋子采用Queen走法一步之内
		能到达的空格的灵活度值有关。下面是某个棋子a
		灵活度值的计算方法：
		步骤1计算棋盘中所有空格的灵活度值。
		步骤2记录棋子a采用Queen走法时一步之内
		能到达的空格。
		步骤3对步骤2记录的每个空格，计算空格的灵
		活度值除以棋子a采用King走法到达该空格的步数
		的值，然后将这些值累加，即得到当前棋子a的灵活度。*/
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)//ij位上的棋子灵活度
		{
			if (gridInfo[i][j] != 0)continue;//如果不是空格就算了
			for (int delta = 0; delta < 8; delta++)
				if (gridInfo[i + dx[delta]][j + dy[delta]] == 0)mobility[i][j]++;//它周围的八个位置如果是空格，它的价值就升高
		}
}

void movevalue(int color)//通过kingmove和queenmove能到的位置
{
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] != color)continue;//确保此处棋子颜色和我想评估的相同
			for (int k = 0; k < 8; ++k)
			{
				for (double delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k] * delta1;//八个方向都走
					int yy = j + dy[k] * delta1;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break; //一旦遇到障碍就停下来
					if (currBotColor == color)
					{
						if (queen[0][xx][yy] == 0)queen[0][xx][yy]++;
						if (1 / delta1 > king[0][xx][yy]) king[0][xx][yy] = 1 / delta1;
					}//queen数组只取1，表示能到；king数组则要评估到这里需要的步数，取倒数
					else if (currBotColor == -color)
					{
						if (queen[0][xx][yy] != 0)endgame = 0;//一旦这个地方我方也可到，对方也可到，就没有结束游戏
						if (queen[1][xx][yy] == 0)queen[1][xx][yy]++;
						if (1 / delta1 > king[1][xx][yy]) king[1][xx][yy] = 1 / delta1;
					}//queen数组只取1，表示能到；king数组则要评估到这里需要的步数，取倒数
				}
			}
		}
	}
}

double value11(int color)//queenmove
{
	double tem = 0;
	{
		if (color == currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[0][i][j];
		else if (color == -currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[1][i][j];
	}
	return tem;
}//queenmove

double value12(int color)//kingmove
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
					tem = tem + mobility[i][j] * king[1][i][j];
	}

	return tem;
}

double value(int color)//查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同
{
	if (endgame == 1)//如果已经到了结束局面，即两方各玩各的
		return value11(color);//那么这时候真正起决定性作用的其实是queenmove
	else if (turnID <= 16)
		return 0.05*value11(color) + 0.5*value12(color) + 0.45*value2(color);
	else if (turnID <= 45)
		return 0.35*value11(color) + 0.35*value12(color) + 0.3*value2(color);
	else
		return 0.85*value11(color) + 0.1*value12(color) + 0.05*value2(color);
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
}//先存下来双方棋盘上棋子的位置信息

int main()
{
	int x0, y0, x1, y1, x2, y2;
	int flags = 0;

	// 初始化棋盘
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;

	cin >> turnID;
	// 读入到当前回合为止，自己和对手的所有行动，从而把局面恢复到当前回合
	currBotColor = grid_white; // 先假设自己是白方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合

		// 首先是对手行动
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black; // 第一回合收到坐标是-1, -1，说明我是黑方
		else
			putchess(x0, y0, x1, y1, x2, y2, -currBotColor); // 模拟对方落子

		// 然后是自己当时的行动
		// 对手行动总比自己行动多一个
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				putchess(x0, y0, x1, y1, x2, y2, currBotColor); // 模拟己方落子
		}
	}
	//draw();
	// 做出决策（你只需修改以下部分）
	/*思路：写一个评估函数，然后对所有走法进行搜索，选择最优的那个*/
	// 这里枚举了所有可能的下法，以便之后随机用……
	int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];//起始能走的点，落点，障碍点
	int posCount = 0, choice;
	double maxvalue = -280000;//浮点，注意了
	memory();//存起来我方对方棋子
	for (int slt = 0; slt < 4; slt++)
	{
		int i = chess1[slt][0];
		int j = chess1[slt][1];//从我刚刚存进去的我方棋子位置开始看八个方向
		for (int k = 0; k < 8; ++k)
		{
			for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
			{
				int xx = i + dx[k] * delta1;//八个方向都走
				int yy = j + dy[k] * delta1;
				if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
					break; //一旦遇到障碍就停下来
				for (int l = 0; l < 8; ++l)
				{
					for (int delta2 = 1; delta2 < GRIDSIZE; delta2++)
					{
						int xxx = xx + dx[l] * delta2;
						int yyy = yy + dy[l] * delta2;
						if (!inMap(xxx, yyy))
							break;
						if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
							break;
						if (legal(i, j, xx, yy, xxx, yyy, currBotColor))//一旦这是合法的落子
						{
							//cout << i << " " << j << endl;
							putchess(i, j, xx, yy, xxx, yyy, currBotColor); // 模拟己方落子
							memset(mobility, 0, sizeof(mobility));//清零灵活度数组
							memset(queen, 0, sizeof(queen));//清零queenmove数组
							memset(king, 0, sizeof(king));//清零kingmove数组
							movevalue(currBotColor);
							movevalue(-currBotColor);
							moble();//存下来灵活度
							beginPos[posCount][0] = i;
							beginPos[posCount][1] = j;
							possiblePos[posCount][0] = xx;
							possiblePos[posCount][1] = yy;
							obstaclePos[posCount][0] = xxx;
							obstaclePos[posCount][1] = yyy;//ij移动点，xx yy移动到的地方 xxx yyy释放障碍
							posCount++;
							double temvalue = value(currBotColor) - value(-currBotColor);//浮点！浮点！！！！！
							if (mobility[xx][yy] != 0)flags = 1;
							//cout << temvalue;
							//cout << value11(currBotColor) << endl;
							//cout << value2(currBotColor) << endl;
							if (temvalue > maxvalue)
								if ((flags != 0 && mobility[xx][yy] != 0) || flags == 0)
								{
									maxvalue = temvalue;
									choice = posCount - 1;
								}
							clear(i, j, xx, yy, xxx, yyy, currBotColor);//清除己方落子，回溯
						}
					}

				}
			}

		}
	}
	//cout << posCount << endl;
	//draw();
	int startX, startY, resultX, resultY, obstacleX, obstacleY;
	if (posCount > 0)
	{
		startX = beginPos[choice][0];
		startY = beginPos[choice][1];
		resultX = possiblePos[choice][0];
		resultY = possiblePos[choice][1];
		obstacleX = obstaclePos[choice][0];
		obstacleY = obstaclePos[choice][1];
	}
	else
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
		obstacleX = -1;
		obstacleY = -1;
	}

	// 决策结束，输出结果（你只需修改以上部分）
	//cout << endgame << endl;
	cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
	return 0;
}
