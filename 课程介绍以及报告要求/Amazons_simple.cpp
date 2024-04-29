/*
��̬������ֻ��ʹ���ϲ���
����С��ab��֦����
��TIME_CONTROL����ʱ�����ƣ���max_min����������������
��������������
��̬������û�����ϴβ���������ϵĽ��
*/







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
#define INFINITE 0x3f3f3f3f

//#define DEBUG
#ifdef DEBUG
double choice_cost_time = 0;
double movevalue_cost_time = 0;
#endif
#define TIME_CONTROL

using namespace std;

/*typedef struct choice {
	int x, y;
	int xx, yy;
	int xxx, yyy;
}choice;
choice best;*/

class Pos
{
public:
	int beginPos[3000][2] = { 0 }, possiblePos[3000][2] = { 0 }, obstaclePos[3000][2] = { 0 };
	int number = 0;
};

//���õ���ѡ��


//ʱ������õ�һЩ����
int threshold = 0.95 * (double)CLOCKS_PER_SEC;	//�޽�ʱ��

int start_time = 0, current_time = 0;
bool time_out = false;

int turnID;
int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];

int first_beginPos[3000][2], first_possiblePos[3000][2], first_obstaclePos[3000][2];
int startX, startY, resultX, resultY, obstacleX, obstacleY;
vector<pair<int, int>> blackInfo; // ��¼���ӵ�x��y
vector<pair<int, int>> whiteInfo; // ��¼���ӵ�x��y
vector<pair<int, int>> arrowInfo; // ��¼����x��y

/*******************************************************************************************************/
int currBotColor; // ������ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // ��x��y����¼����״̬
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };

// �ж��Ƿ���������
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2))) //0��1��2����������
	{
		return false;
	}
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0) //���ԭ��λ��û�и���ɫ���ӣ�����Ŀ��λ������
	{
		return false;
	}
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0)) //���ϰ�λ�����Ӳ��Ҹ��Ӳ�Ϊԭλ��
	{
		return false;
	}
	if (!check_only) //ֻ���Ͳ�����
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}
/***********************************************************************************************************/

void Clear(int x0, int y0, int x1, int y1, int x2, int y2, int color) //����һ�غ�ĳ�����ж�
{
	gridInfo[x2][y2] = 0;
	gridInfo[x1][y1] = 0;
	gridInfo[x0][y0] = color;
}

void scan()//ɨ�����̣���¼��ǰ�ӵ�λ�ã����浽Info������
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

Pos Choices(int color) //�ҳ���ǰ���п����߷�,�����߷���
{
#ifdef DEBUG
	int start_time = clock();	//����ʱ��
#endif
	Pos pos;
	scan();
	int posCount = 0;
	vector<pair<int, int>>* Info[2];
	Info[0] = &blackInfo, Info[1] = &whiteInfo;
	int cnt = -1, Cnt = 0;
	cnt = color == grid_black ? 0 : 1; // 0������ӣ�1�������
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 8; j++)
		{
			for (int try1 = 1; try1 < GRIDSIZE; try1++)
			{
				int resX = dx[j] * try1 + (*Info[cnt])[i].first; // �����յ�λ��
				int resY = dy[j] * try1 + (*Info[cnt])[i].second;
				if (!inMap(resX, resY) || gridInfo[resX][resY] != 0)
					break;
				// Ѱ���ϰ����λ��
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
	//printf("Choices Cost time = %.9f\n", cost_time);
	//printf("Choices_Cost_time_sum = %.9f\n", choice_cost_time);
#endif
	return pos;
}

//��������
/*******************************************************************************************************************/


int chess1[4][2] = { 0 };//����ҷ�����λ��
int chess2[4][2] = { 0 };//��ŶԷ�����λ��

double mobility[GRIDSIZE][GRIDSIZE] = { 0 };//�����ո������
double queen[2][8][8] = { 0 };
double king[2][8][8] = { 0 };//0 �ҵ�ֵ  1 �Է���ֵ

int mostvalue = 0;
int endgame = 1;
//��������Ƿ�Ϸ�

//���ģ�����ӣ�����ר�ã����˳����ղ������ŵ�


inline void moble()
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)//ijλ�ϵ���������
		{
			if (gridInfo[i][j] != 0)continue;//������ǿո������
			for (int delta = 0; delta < 8; delta++)
				if (gridInfo[i + dx[delta]][j + dy[delta]] == 0)mobility[i][j]++;//����Χ�İ˸�λ������ǿո����ļ�ֵ������
		}
}
//��������,��kingֵ

inline void movevalue(int color)//ͨ��kingmove��queenmove�ܵ���λ��
{
#ifdef DEBUG
	int start_time = clock();	//����ʱ��
#endif
	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{
			if (gridInfo[i][j] != color)continue;//ȷ���˴�������ɫ����������ͬ
			for (int k = 0; k < 8; ++k)
			{
				for (double legnth = 1; legnth < GRIDSIZE; legnth++)
				{
					int xx = i + dx[k] * legnth;//�˸�������
					int yy = j + dy[k] * legnth;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break; //һ�������ϰ���ͣ����
					if (currBotColor == color)
					{
						if (queen[0][xx][yy] == 0)queen[0][xx][yy]++;
						if (1 / legnth > king[0][xx][yy]) king[0][xx][yy] = 1 / legnth;
					}//queen����ֻȡ1����ʾ�ܵ���king������Ҫ������������Ҫ�Ĳ�����ȡ����
					else if (currBotColor == -color)
					{
						if (queen[0][xx][yy] != 0)endgame = 0;//һ������ط��ҷ�Ҳ�ɵ����Է�Ҳ�ɵ�����û�е��оֽ׶�
						if (queen[1][xx][yy] == 0)queen[1][xx][yy]++;
						if (1 / legnth > king[1][xx][yy]) king[1][xx][yy] = 1 / legnth;
					}//queen����ֻȡ1����ʾ�ܵ���king������Ҫ������������Ҫ�Ĳ�����ȡ����
				}
			}
		}
	}
#ifdef DEBUG
	int current_time = clock();
	double cost_time = (double)(current_time - start_time) * 1.0 / A_SEC;
	movevalue_cost_time += cost_time;
	//printf("Movevalue Cost time = %.9f\n", cost_time);
	//printf("Movevalue_Cost_time_sum = %.9f\n", movevalue_cost_time);
#endif
}

inline double value11(int color)
{
	double tem = 0;
	{
		if (color == currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[0][i][j];//0�Լ�
		else if (color == -currBotColor)
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					tem += queen[1][i][j];//1����
	}
	return tem;
}
//queenmove,�õ�t1

inline double value12(int color)
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
//kingmove���õ�t2

inline double value2(int color)
{
	double tem = 0;
	if (currBotColor == color)//������ҵ���ɫ
	{
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
				if (king[0][i][j] != 0)
					tem = tem + mobility[i][j] * king[0][i][j];
	}

	else if (currBotColor == -color)//����ǶԷ�����ɫ
	{
		for (int i = 0; i < GRIDSIZE; i++)
			for (int j = 0; j < GRIDSIZE; j++)
				if (king[1][i][j] != 0)
					tem = tem + mobility[i][j] * king[1][i][j];//��Ϊkingȡ�õ����������������ֱ�����
	}
	return tem;
}
//�����������
inline double value(int color)//�鿴�����ɫ�����ӣ������̾�����ӵ�еļ�ֵ�����׶���Ȩ�ز�ͬ
{
	if (endgame == 1)//����Ѿ����˽������棬�����������
		return value11(color);//��ô��ʱ����������������õ���ʵ��queenmove
	else if (turnID <= 16)
		return 0.14 * value11(color) + 0.37 * value12(color) + 0.13 * value2(color);
	else if (turnID <= 45)
		return 0.3 * value11(color) + 0.25 * value12(color) + 0.2 * value2(color);
	else//���׶�
		return 0.8 * value11(color) + 0.1 * value12(color) + 0.05 * value2(color);
}//�鿴�����ɫ�����ӣ������̾�����ӵ�еļ�ֵ�����׶���Ȩ�ز�ͬ

inline void memory()//�ȴ�����˫�����������ӵ�λ����Ϣ
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
//�ȴ�����˫�����������ӵ�λ����Ϣ

inline double calculate()
{
	memory();//��״̬
	memset(mobility, 0, sizeof(mobility));//������������
	memset(queen, 0, sizeof(queen));//����queenmove����
	memset(king, 0, sizeof(king));//����kingmove����
	movevalue(currBotColor);
	movevalue(-currBotColor);
	moble();//����������
	double Value1 = value(currBotColor);
	double Value2 = value(-currBotColor);
	double temvalue = Value1 - Value2;//��������
	return temvalue;
}


/*******************************************************************************************************************/

//������С������a-b��֦����ʼalphaΪ�����betaΪ������
double Max_Min_Search(int depth, int now_depth, double alpha, double beta, int color, int now_color)
{

#ifdef TIME_CONTROL
	current_time = clock();
	double interval_time = current_time - start_time;
	//����ʱ�䷭��
	if (turnID == 1) threshold*=2;
	if (interval_time > threshold)
	{
		//ʱ�䳬��
		time_out = true;
		//int sign = now_depth % 2 == 0 ? 1 : -1;
		//double x = sign * calculate();
		return 0;
		//return 0;
	}
#endif

	if (now_depth == depth) //�������������ǰ����
	{
		int sign = now_depth % 2 == 0 ? 1 : -1;
		double x = sign * calculate();
		return x;
	}
	Pos pos;
	pos = Choices(now_color);
	if (pos.number == 0)
	{
		int sign = now_depth % 2 == 0 ? 1 : -1;
		double x = sign * calculate();
		return x;
	}

	double val = 0;
	int choose = 0; //choose��Ҫ��ֵ�𣿴����ǣ�

	for (int i = 0; i < pos.number; i++)
	{
		int x0, y0, x1, y1, x2, y2;

		//����
		if (!ProcStep(pos.beginPos[i][0], pos.beginPos[i][1], pos.possiblePos[i][0], pos.possiblePos[i][1], pos.obstaclePos[i][0], pos.obstaclePos[i][1], now_color, false))
		{
			cout << "fail in procstep" << endl;
			getchar();

		}

		x0 = pos.beginPos[i][0]; y0 = pos.beginPos[i][1];
		x1 = pos.possiblePos[i][0]; y1 = pos.possiblePos[i][1];
		x2 = pos.obstaclePos[i][0]; y2 = pos.obstaclePos[i][1];

		val = -Max_Min_Search(depth, now_depth + 1, -beta, -alpha, color, -now_color); //���ظ����Ӷ����Խ�ÿ�㶼����Max��
		//����
		Clear(x0, y0, x1, y1, x2, y2, now_color);



		if (val >= beta)
		{
			return beta; //beta��֦
		}

		if (val > alpha)
		{
			alpha = val;
			choose = i; //Ŀǰ��i���������
		}
		//��������Ч�ʸ���
#ifdef TIME_CONTROL
		current_time = clock();
		if (turnID == 1) threshold *= 2;
		if (interval_time > threshold)
		{
			//ʱ�䳬��
			time_out = true;
			//int sign = now_depth % 2 == 0 ? 1 : -1;
			//double x = sign * calculate();
			return 0;
			//return 0;
		}
#endif
	}

	if (now_depth == 0 && !time_out) //�����ǰΪ���򱣴�����߷�
	{
		/*startX=best.x = pos.beginPos[choose][0];
		startY=best.y = pos.beginPos[choose][1];
		resultX=best.xx = pos.possiblePos[choose][0];
		resultY=best.yy = pos.possiblePos[choose][1];
		obstacleX=best.xxx = pos.obstaclePos[choose][0];
		obstacleY=best.yyy = pos.obstaclePos[choose][1];*/
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

	// ��ʼ������
	gridInfo[0][2] = grid_black;
	gridInfo[2][0] = grid_black;
	gridInfo[5][0] = grid_black;
	gridInfo[7][2] = grid_black;

	gridInfo[0][5] = grid_white;
	gridInfo[2][7] = grid_white;
	gridInfo[5][7] = grid_white;
	gridInfo[7][5] = grid_white;

	// �����Լ��յ���������Լ���������������ָ�����״̬

	cin >> turnID;

	currBotColor = grid_white; // �ȼ����Լ��ǰ׷�
	for (int i = 0; i < turnID; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�

		// �����Ƕ����ж�
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black; // ��һ�غ��յ�������-1, -1��˵���ҷ��Ǻڷ�
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // ģ��Է�����

		// Ȼ���Ǳ�����ʱ���ж�
		// �����ж��ܱ��Լ��ж���һ��
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // ģ�Ȿ������
		}
	}


	/*****************************************************************************************************************/
	/***�����������Ĵ��룬���߽�������������ӵ�λ�ã�����startX��startY��resultX��resultY��obstacleX��obstacleY��*****/
#ifdef DEBUG
	int main_start_time = clock();	//����ʱ��
#endif
	start_time = clock();
	int depth = 1;
	//���������Ͽ��������
	for (depth = 1; depth <= INFINITE; depth++)
	{
		Max_Min_Search(depth, 0, -INFINITE, INFINITE, currBotColor, currBotColor);
		if (time_out) break;
	}
	//startX = best.x;
	//startY = best.y;
	//resultX = best.xx;
	//resultY = best.yy;
	//obstacleX = best.xxx;
	//obstacleY = best.yyy;

	/****���Ϸ������Ĵ��룬���߽�������������ӵ�λ�ã�����startX��startY��resultX��resultY��obstacleX��obstacleY��****/
	/*****************************************************************************************************************/
	cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
	cout << depth << endl;
#ifdef DEBUG
	int main_return_time = clock();	//�˳�ʱ��
	double main_time_sum = (main_return_time - main_start_time) * 1.0 / A_SEC;
	printf("main_cost_time = %.9f\n", main_time_sum);
#endif
	return 0;
}