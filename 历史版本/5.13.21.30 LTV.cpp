/*

参数待调整，F1-F5
更名、规范化修改
期望加入后期蒙特卡洛算法

*/


#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <vector>
#include <algorithm>
#include <math.h>
#include<queue>

#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
#define INFINITE 0x3f3f3f3f

#define TIME_CONTROL
#ifdef TIME_CONTROL
int start_time;
int current_time;
// 时间控制用的一些参数
int threshold = 0.95* (double)CLOCKS_PER_SEC; // 限界时间
int A_SEC = 1.00 * (double)CLOCKS_PER_SEC;     // 一秒，便于调试
bool time_out = false;
#endif
//#define PARA
// #define DEBUG
#ifdef DEBUG
double choice_cost_time = 0;
double movevalue_cost_time = 0;
#endif

using namespace std;

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

class Pos
{
public:
    int beginPos[3000][2] = { 0 }, possiblePos[3000][2] = { 0 }, obstaclePos[3000][2] = { 0 };
    int number = 0;
};

int Queen[300][3], King[300][3];
int Queenend = 0, Kingend = 0, Queenhead = 0, Kinghead = 0;

int turnID;
int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];

int first_beginPos[3000][2], first_possiblePos[3000][2], first_obstaclePos[3000][2];
int startX, startY, resultX, resultY, obstacleX, obstacleY;
vector<pair<int, int>> blackInfo; // 记录黑子的x和y
vector<pair<int, int>> whiteInfo; // 记录白子的x和y
vector<pair<int, int>> arrowInfo; // 记录箭的x和y

/*******************************************************************************************************/
int currBotColor;                       // 本方所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

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
    cx0 = x0;
    cy0 = y0;
    cx1 = x1;
    cy1 = y1;
    cx2 = x2;
    cy2 = y2;
    ccolor = color;
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
/***********************************************************************************************************/

void Clear(int x0, int y0, int x1, int y1, int x2, int y2, int color) // 回溯一回合某方的行动
{
    gridInfo[x2][y2] = 0;
    gridInfo[x1][y1] = 0;
    gridInfo[x0][y0] = color;
}

void scan() // 扫描棋盘，记录当前子的位置，保存到Info数组中
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

Pos Choices(int color) // 找出当前所有可能走法,返回走法数
{
#ifdef DEBUG
    int start_time = clock(); // 进入时间
#endif
    Pos pos;
    scan();
    int posCount = 0;
    vector<pair<int, int>>* Info[2];
    Info[0] = &blackInfo, Info[1] = &whiteInfo;
    int cnt = -1;
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
#ifdef DEBUG
    int current_time = clock();
    double cost_time = (double)(current_time - start_time) * 1.0 / A_SEC;
    choice_cost_time += cost_time;
    // printf("Choices Cost time = %.9f\n", cost_time);
    // printf("Choices_Cost_time_sum = %.9f\n", choice_cost_time);
#endif
    return pos;
}

// 评估函数
/*******************************************************************************************************************/

int chess1[4][2] = { 0 }; // 存放我方棋子位置
int chess2[4][2] = { 0 }; // 存放对方棋子位置

double mobility[GRIDSIZE][GRIDSIZE] = { 0 }; // 各个空格的灵活度
double D2[2][8][8] = { 0 };
double D1[2][8][8] = { 0 }; // 0 我的值  1 对方的值

int mostvalue = 0;

// 检查落子是否合法

// 清除模拟落子，回溯专用，清空顺序与刚才是逆着的

inline void moble()
{
    for (int i = 0; i < GRIDSIZE; i++)
        for (int j = 0; j < GRIDSIZE; j++) // ij位上的棋子灵活度
        {
            if (gridInfo[i][j] != 0)
                continue; // 如果不是空格就算了
            for (int delta = 0; delta < 8; delta++)
            {
                int xx = i + dx[delta];
                int yy = j + dy[delta];
                if (inMap(xx, yy)&& gridInfo[xx][yy] == 0)
                {
                        mobility[i][j]++; // 它周围的八个位置如果是空格，它的价值就升高
                }

            }
        }
}
// 计算灵活度,算king值
class Coor {
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
};

void init()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (gridInfo[i][j] == 0)
            {
                D1[0][i][j] = D1[1][i][j] = D2[0][i][j] = D2[1][i][j] = 100;
            }
            else if (gridInfo[i][j] == currBotColor)
            {
                D1[1][i][j] = D2[1][i][j] = -1;
                D1[0][i][j] = D2[0][i][j] = 0;
            }
            else if(gridInfo[i][j] == -currBotColor)
            {
                D1[0][i][j] = D2[0][i][j] = -1;
                D1[1][i][j] = D2[1][i][j] = 0;
            }
            else
            {
                D1[0][i][j] = D2[0][i][j] =D1[1][i][j] = D2[1][i][j] = -1;
            }
        }
    }
}
inline void movevalue(int color)
{
    
    queue<Coor> qQueen, qKing;
    if (color == currBotColor)
    {
        for (int i = 0; i < 4; i++)
        {
            qQueen.push(Coor(chess1[i][0], chess1[i][1], 0));
            qKing.push(Coor(chess1[i][0], chess1[i][1], 0));
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            qQueen.push(Coor(chess2[i][0], chess2[i][1], 0));
            qKing.push(Coor(chess2[i][0], chess2[i][1], 0));
        }
    }
    int div = color == currBotColor ? 0 : 1;
    //处理皇后走法，赋值给D1
    while (!qQueen.empty())
    {
        Coor pos = qQueen.front();
        qQueen.pop();
        for (int i = 0; i < 8; i++) //方向
        {
            for (int k = 1; k < GRIDSIZE; k++)
            {
                int tempx = dx[i] * k + pos.x;
                int tempy = dy[i] * k + pos.y;
                //不在地图内部，或不是空格，就退出
                if (!inMap(tempx, tempy) || gridInfo[tempx][tempy] != 0)
                    break;
                if (pos.val + 1 < D1[div][tempx][tempy])
                {
                    D1[div][tempx][tempy] = pos.val + 1;
                    qQueen.push(Coor(tempx, tempy, pos.val + 1));
                }
            }
        }
    }
    //处理国王走法，赋值给D2
    while (!qKing.empty())
    {
        Coor pos = qKing.front();
        qKing.pop();
        for (int i = 0; i < 8; i++) //方向
        {
            int tempx = dx[i] + pos.x;
            int tempy = dy[i] + pos.y;
            //不在地图内部，或不是空格，就退出
            if (!inMap(tempx, tempy) || gridInfo[tempx][tempy] != 0)
                continue;
            if (pos.val + 1 < D2[div][tempx][tempy])
            {
                D2[div][tempx][tempy] = pos.val + 1;
                qKing.push(Coor(tempx, tempy, pos.val + 1));
            }
        }
    }

}

inline double t1(int color)
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
                    else//步数相等
                    {
                        if (D1[0][i][j] < 100)//都能到
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

inline double t2(int color)
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
                else//步数相等
                {
                    if (D2[0][i][j] < 100)//都能到
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
// kingmove，得到t2

inline double m(int color)
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
                tmp += 100.0 / (10 + sum);
            }
        }
    }
    return tmp;
}
// 算出最终灵活度
inline double c1()
{
    double c_1 = 0;
    for (int i = 0; i < GRIDSIZE; i++)
        for (int j = 0; j < GRIDSIZE; j++)
        {
            if (D1[0][i][j] != 100 && D1[1][i][j] != 100)
                c_1 += pow(2, -D1[0][i][j]) - pow(2, -D1[1][i][j]);
        }
    return 2 * c_1;
}
inline double c2()
{
    double c_2 = 0;
    for (int i = 0; i < GRIDSIZE; i++)
        for (int j = 0; j < GRIDSIZE; j++)
        {
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
        }
    return c_2;
}

inline double value(int color, int turn) // 查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同
{
    // double x = t1(color);
    // cout << x << endl;
    #ifdef PARA
    cout << t1(color)<<"   " << t2(color) << "   " << c1() << "   " << c2() << "   " << m(color) << endl;
    #endif
    return f1[turnID] * t1(color) + f2[turnID] * t2(color) + f3[turnID] * c1() + f4[turnID] * c2() + f5[turnID] * m(color);

} // 查看这个颜色的棋子，在这盘局面下拥有的价值。三阶段下权重不同

inline void memory() // 先存下来双方棋盘上棋子的位置信息
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
// 先存下来双方棋盘上棋子的位置信息

inline double calculate(int turn)
{
    memory();                              // 存状态
    memset(mobility, 0, sizeof(mobility)); // 清零灵活度数组
    //memset(D2,0x3f, sizeof(D2));    // 清零queenmove数组
    //memset(D1, 0x3f, sizeof(D1));    // 清零kingmove数组
    init();
    movevalue(currBotColor);
    movevalue(-currBotColor);
    moble(); // 存下来灵活度
    double Value1 = value(currBotColor, turn);
    //double Value2 = value(-currBotColor, turn);
    //double temvalue = Value1 - Value2; // 计算灵活度
    return Value1;
}
/*******************************************************************************************************************/

// 负极大极小搜索，a-b剪枝，初始alpha为负无穷，beta为正无穷
double Max_Min_Search(int depth, int now_depth, double alpha, double beta, int color, int now_color)
{
#ifdef TIME_CONTROL
    current_time = clock();
    double interval_time = current_time - start_time;
    // 开局时间翻倍
    double threshold_temp = turnID == 1 ? threshold * 2 : threshold;
    if (interval_time > threshold_temp)
    {
        // 时间超了
        time_out = true;
        // int sign = now_depth % 2 == 0 ? 1 : -1;
        // double x = sign * calculate();
        return 0;
        // return 0;
    }
#endif
    if (now_depth == depth) // 到达层数评估当前棋盘
    {
        int sign = now_depth % 2 == 0 ? 1 : -1;
        double x = sign * calculate(turnID + now_depth);
        return x;
    }
    Pos pos;
    pos = Choices(now_color);
    if (pos.number == 0)
    {
        int sign = now_depth % 2 == 0 ? 1 : -1;
        double x = sign * calculate(turnID + now_depth);
        return x;
    }

    double val = 0;
    int choose = 0; // choose需要初值吗？待考虑！

    for (int i = 0; i < pos.number; i++)
    {
        int x0, y0, x1, y1, x2, y2;

        // 落子
        if (!ProcStep(pos.beginPos[i][0], pos.beginPos[i][1], pos.possiblePos[i][0], pos.possiblePos[i][1], pos.obstaclePos[i][0], pos.obstaclePos[i][1], now_color, false))
        {
            cout << "fail in procstep" << endl;
            getchar();
        }

        x0 = pos.beginPos[i][0];
        y0 = pos.beginPos[i][1];
        x1 = pos.possiblePos[i][0];
        y1 = pos.possiblePos[i][1];
        x2 = pos.obstaclePos[i][0];
        y2 = pos.obstaclePos[i][1];

        val = -Max_Min_Search(depth, now_depth + 1, -beta, -alpha, color, -now_color); // 返回负，从而可以将每层都看作Max层

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

#ifdef TIME_CONTROL
    current_time = clock();
    interval_time = current_time - start_time;
    // 开局时间翻倍
    threshold_temp = turnID == 1 ? threshold * 2 : threshold;
    if (interval_time > threshold_temp)
    {
        // 时间超了
        time_out = true;
        // int sign = now_depth % 2 == 0 ? 1 : -1;
        // double x = sign * calculate();
        return 0;
        // return 0;
    }
#endif

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
#ifdef DEBUG
    int main_start_time = clock(); // 进入时间
#endif
#ifdef TIME_CONTROL
    start_time = clock();
#endif
    int depth = 1;
    // 这里理论上可以无穷层
    for (depth = 1; depth <= INFINITE; depth++)
    {
        Max_Min_Search(depth, 0, -INFINITE, INFINITE, currBotColor, currBotColor);
#ifdef TIME_CONTROL
        if (time_out)
            break;
#endif
    }

    /****在上方填充你的代码，决策结果（本方将落子的位置）存入startX、startY、resultX、resultY、obstacleX、obstacleY中****/
    /*****************************************************************************************************************/

    cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
    cout << depth << endl;
#ifdef DEBUG
    int main_return_time = clock(); // 退出时间
    double main_time_sum = (main_return_time - main_start_time) * 1.0 / A_SEC;
    printf("main_cost_time = %.9f\n", main_time_sum);
#endif
    return 0;
}