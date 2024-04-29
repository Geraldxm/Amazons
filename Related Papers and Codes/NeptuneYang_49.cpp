//MCTS


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
#define me 1
#define him -1
#define the_first_move_adv 0.3										 //先行方优势
#define max(a,b) (a>b)?a:b
#define min(a,b) (a<b)?a:b

using namespace std;
using grid = int[GRIDSIZE][GRIDSIZE];

grid D_queen_me, D_queen_him, D_king_me, D_king_him;				 // 用于计算 kingmove queenmove
double timelimit = 0.97 * CLOCKS_PER_SEC;							 // 卡时
int currBotColor;													 // 我所执子颜色
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 };							 // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int turn_num = 0;													 // 当前回合数
double C_ucb = 0;													 // ucb算法中的 c

																	 // 由训练得到的数据
double w1[60] = { 0.017539279518905833, -0.04021565337540642, 0.009926859031951738, 0.08642758311704062, 0.01437765361811897, 0.03702105521982053, 0.014064253382399472, 0.017821517859228756, 0.003637125459811753, 0.061010703925692744, 0.007226182848242214, 0.062097862401994317, 0.05192390254025116, 0.026119108548309514, 0.04874848453161014, 0.03817299358393731, 0.07622939999010621, 0.03550438104814331, 0.027433643403792857, 0.033689786962845386, 0.05595869156096635, 0.0077834123178497925, 0.11418940226775452, 0.1251269052094458, 0.015293585610845224, 0.06139618121671625, 0.05328942343941323, 0.09818775156890429, 0.04654732194485187, 0.038181028630794894, 0.05327008709281307, 0.2102948022564665, 0.05787071060693268, 0.15708155545041794, 0.2694298632472131, 0.2442182545783312, 0.20746072894141168, 0.09553396374025389, 0.07646462203341331, 0.47428923356940483, 0.20710066579311134, 0.25064799226047485, 0.1111866963738114, 0.6894203756636964, 0.6603727874024296, 0.3559481012985614, 0.4961138748667092, 0.45952963304844435, 0.29332476803651336, 0.5181358025855182, 0.7001335118236421, 0.5110266223907547, 0.48371898010407144, 0.5450052219917028, 0.6420891530019827, 0.7991742392010405, 0.3730641389641275, 0.6016792485366864, 0.5432232012527364, 0.12827752277530047 };
double w2[60] = { 0.4161367035694622, -0.12574125713137738, -0.0028236541528660368, -0.03889452065755132, -0.06082969044050724, -0.011954563454365056, -0.022098915648674857, 0.02366535715841465, -0.014908356530754209, -0.00033655727531270224, 0.012818905535262853, 0.039598708903596944, 0.01286781578353177, 0.03014239132513331, -0.017455365312813845, -0.002771240057097532, -0.011112444442698382, 0.05347710700472003, 0.011418877912649835, 0.08101316981120855, 0.06757488198084763, 0.051042452209843533, 0.06817808326861731, 0.06700660696581791, 0.12130842426068336, 0.11443134237858613, 0.08415148153070406, 0.06626897508046854, 0.14808029819904003, 0.1842814575601383, 0.20047810420127485, 0.09654065759816526, 0.19098343511846785, 0.22719101102104855, 0.04074698966814579, 0.17621378341561653, 0.1984575337735236, 0.21436461904806586, 0.3095682155864248, -0.024957469879821177, 0.49136078621824053, 0.25259389285671235, 0.2963093833158402, 0.44112288225617485, 0.3128201188773954, 0.9357666239340254, 0.7315468424140507, 0.5337217760622192, 0.3099931591272743, 0.380776855993629, 0.438164689980234, 0.4740163725362654, 0.3784036511057698, 0.44170491947661084, 0.5001539885365189, 0.4401216144001494, 0.36028201001272225, 0.31422161509523616, 1.010087647175932, 0.20536535044002285 };
double w3[60] = { 0.29571705389558667, 0.11768616563224417, 0.1079889822995275, -0.029585124613049848, 0.05033072314819434, -0.03393031980853569, 0.07842714783564027, 0.02341968599239014, 0.10870345511859424, 0.022727617280822995, 0.09753170021618622, -0.04172386038785276, 0.040674735628077455, 0.09442020542438279, 0.0858381546759595, 0.047834445007584096, 0.039414950295311446, 0.07136125820192227, 0.1148206779445391, 0.0147775135979634, 0.051609353247467335, 0.0885709729635472, 0.016310455663120706, -0.06547252684230634, 0.1744073820750567, 0.10018652603654425, 0.08025132484021547, 0.034427265670766954, 0.08201103587313185, 0.09958321506781341, 0.17638711172788596, 0.016615647366647356, 0.1466483322782983, -0.0076969817061976415, 0.15519852035707177, 0.129298886540791, 0.22538642640485643, 0.1518523080652958, 0.3676296546649733, 0.2495418454392968, 0.09041091441349103, 0.35547866239633064, 0.35061674620462935, 0.16711925311585357, 0.38720011963726053, 0.5593944694223154, 0.4477682302767064, 0.5474497696227778, 1.2470598626788232, 0.6576150973463604, 0.79408712367841, 0.18889707637585343, 0.626596627555329, 0.7068347767876898, 0.4834066752266345, 0.36743953663858775, 0.5793040317373417, 0.43707651286881377, 0.3812488834234786, 0.7299001135435965 };
double w4[60] = { 0.7671132336681443, 0.23295785025978227, 0.08920288587382819, 0.24983244112119235, 0.2830814961158663, 0.27279591364454214, 0.22125803124067944, 0.23568039956618747, 0.25661023023620055, 0.22656138852997545, 0.26053269196600753, 0.2147059864882623, 0.23027472257600204, 0.16111116468448333, 0.2169930818033648, 0.20829823463727892, 0.20079804852077837, 0.18402408822578345, 0.22345223132242953, 0.19986916661381898, 0.1818870834588763, 0.2036534452652452, 0.19579242432305985, 0.2090027544865477, 0.1429803009168061, 0.11137480614963498, 0.12160028851772273, 0.1660292490986964, 0.14119144124283167, 0.12677394169661896, 0.08752454242637446, 0.14859369289118798, 0.1264901459808086, 0.12686534990734505, 0.21216846473899395, 0.1862469818613287, 0.19476243127287146, 0.3576681035309681, 0.26292923528514756, 0.30461049090753584, 0.3066428550620119, 0.650266560822012, 0.5499234708318328, 0.221502348078104, 0.4256892637869417, 0.17755291978757837, 0.626228806955244, 0.5320595807451424, 0.6844824085232213, 0.687189390188111, 0.5824125572651453, 0.7815138080343331, 0.6008948347286416, 0.4253694158405869, 0.35200250177706005, 0.453000467502793, -0.0010916264148790322, 0.8651988914269338, 0.1571865177807934, 0.48467208720052624 };
double w5[60] = { 0.2364865460491925, 0.028984447160166814, 0.07308012475908135, 0.04112878592673885, 0.07024385606442397, 0.1062973680124154, 0.09982767288836572, 0.09020212300644938, 0.08903906008131407, 0.13059871813269902, 0.1458549257625203, 0.10921721705628715, 0.16995107075658436, 0.16756341071575667, 0.14678802170097716, 0.10707991270534306, 0.1600373166078966, 0.04679207562063768, 0.15592696281435547, 0.11309071740522318, 0.1356055577528753, 0.15992333340104886, 0.1456791065469635, 0.1492846848251655, 0.12379063633301636, 0.11122627387513795, 0.15770164695069722, 0.1616146526135293, 0.1453349072286027, 0.09952698842740361, 0.07788871253891898, 0.21899487940264992, 0.16643608286424383, 0.2562124102971503, 0.2494453443217598, 0.29074198283799835, 0.4805932115139708, 0.6115507028668665, 0.5049948733411211, 0.37741125122202634, 0.7046241442380455, 0.1785804590183498, 0.12375207061556008, 0.5400618101077649, 0.4829926045605114, 0.46254962926977, 0.4953689500218497, 0.17713649325552125, 0.2699438541333597, 0.7580515289969904, 0.703700724723163, 0.3713445022470069, 0.8928153230956943, 0.29140581491354633, 0.3859925154796848, 0.5584864134659872, 0.2610000210150233, 0.40241389116162707, 0.465385766954905, 0.4209307197666383 };
double b[60] = { 0.03957108229415428, 0.11752471442025977, -0.023393546195482454, -0.003367858361555568, 0.18204736142714462, 0.14372786839308507, 0.007200203951132804, 0.08780322109041236, 0.1818050930959559, -0.05601344916978047, 0.06820243644964515, -0.16079282163895026, -0.14352336132013796, 0.15196641441802816, -0.05624996340748454, 0.019177646270843108, -0.058047843020280994, -0.004840852613350671, 0.09499056136235279, -0.04458509598844868, -0.05720501450979707, 0.20216247807291038, -0.059608467762678675, -0.21983672759783593, 0.12866490340952694, -0.04312197078434593, 0.08942893557553128, -0.054075501714904724, -0.06432111780362959, 0.0028843751384352214, -0.06656881214178348, -0.23726903542283292, -0.02776972320579057, -0.1645168689696197, -0.03808053714976485, -0.1562202921528593, -0.07704128406429009, -0.106864042063083, -0.10468953906105412, -0.1621125050722838, -0.2034813485991701, -0.16977641523865816, -0.1141249701145847, -0.1540056214243305, -0.06712643699377024, -0.15097995078620316, -0.13977866164122404, 0.003254560379977395, -0.18971504057171362, -0.12825245191870052, -0.23385221227076824, -0.1550704024849557, -0.03413771803337695, -0.10218613470894461, -0.01927805559018171, -0.19596304326287112, -0.22520474366448945, 0.030752552669726114, -0.13696386726262685, 0.2670337244799694 };

struct move_forward {
	int x0, y0, x1, y1, x2, y2;
	int who;
};
struct mv {
	int x0, y0, x1, y1, x2, y2;
};
struct point {
	point* head_tree;
	int x0, y0, x1, y1, x2, y2;
	int who;
	int N = 0;
	double v = 0;
	vector<point*> sub_tree;
	int N_0_start = 0;
	vector<mv> submv;
};
struct position {
	int x = 0, y = 0;
};
struct simple_point {
	int x0, y0, x1, y1, x2, y2;
	int who;
	position mychess[4];
	position hischess[4];
	int move_id = 0;
};

inline int set_maxdepth(int turn) {														  // rollout 深度
	return 1;
}

inline bool inMap(int x, int y)
{
	return !(x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE);
}

inline int qpow2(int n) {																   // 快速幂
	if (n < 10) return (1 << n);
	else return 1024;
}

bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)  // 在坐标处落子，检查是否合法或模拟落子
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}

inline simple_point getSP(point f_p) {
	simple_point out;
	out.x0 = f_p.x0, out.y0 = f_p.y0;
	out.x1 = f_p.x1, out.y1 = f_p.y1;
	out.x2 = f_p.x2, out.y2 = f_p.y2;
	out.who = f_p.who;
	int len1 = 0, len2 = 0;
	for (int i = 0;i < GRIDSIZE;i++) {
		for (int j = 0;j < GRIDSIZE;j++) {
			if (gridInfo[i][j] == currBotColor) {
				out.mychess[len1].x = i, out.mychess[len1].y = j;
				len1++;
			}
			else if (gridInfo[i][j] == -currBotColor) {
				out.hischess[len2].x = i, out.hischess[len2].y = j;
				len2++;
			}
		}
	}
	if (out.who == currBotColor) {
		for (int s = 0;s < 4;s++) {
			if (out.mychess[s].x == out.x0 && out.mychess[s].y == out.y0) {
				out.move_id = s;
				break;
			}
		}
	}
	else {
		for (int s = 0;s < 4;s++) {
			if (out.hischess[s].x == out.x0 && out.hischess[s].y == out.y0) {
				out.move_id = s;
				break;
			}
		}
	}
	return out;
}

inline void init(void) {
	int x0, y0, x1, y1, x2, y2;
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;
	int turnID;
	cin >> turnID;
	if (turnID == 1) timelimit *= 2;
	currBotColor = grid_white;
	for (int i = 0; i < turnID; i++)
	{
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)	currBotColor = grid_black;
		else ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // 模拟对方落子
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0) ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // 模拟己方落子
		}
	}
	turn_num = turnID;
}


inline double ucb(double&, int&, int, double&);
inline void get_there_step_queen(int, position*, position*);
inline double value(int color);
inline void set_parameter_value(int, double&, double&, double&, double&, double&, double&);
inline double mobility(int, position*, position*);
inline void get_there_step_king(int, position*, position*);
inline void MCTS(point*);
inline void tr_back(point*, double);
inline void roll_out(simple_point*, double&, int);


int main()
{

	init();
	double s = clock();

	if (turn_num < 12) C_ucb = 0.01 * turn_num;									// 动态调整C
	else C_ucb = 0.3;

	point* head_point = new point({ nullptr,-1,-1,-1,-1,-1,-1,-currBotColor });   // 创建头部
	double nowtime;

	while (1) {																	// 搜索
		MCTS(head_point);
		nowtime = clock();
		if ((nowtime - s) > timelimit) {
			break;
		}
	}

	int sublen = head_point->sub_tree.size();									// 找访问最多子节点
	int Nmax = -1;
	int max_i = -1;
	for (int i = 0;i < sublen;i++) {
		if ((head_point->sub_tree[i])->N > Nmax) {
			Nmax = (head_point->sub_tree[i])->N;
			max_i = i;
		}
	}

	point* outp = head_point->sub_tree[max_i];									// 决策结束，输出结果
	double win = head_point->v;
	cout << outp->x0 << ' ' << outp->y0 << ' ' << outp->x1 << ' ' << outp->y1 << ' ' << outp->x2 << ' ' << outp->y2 << endl;
	cout << "win:" << win << "  N:" << head_point->N << endl;
	return 0;
}

inline void MCTS(point* f_p) {
	point& np = (*f_p);
	if (np.submv.empty()) {									 //若空，展开


		position chessman[4];								//找到所有可行走法
		bool can_move = 0;
		int len_of_chessman = 0;

		for (int i = 0;i < GRIDSIZE;i++) {					//先找到棋子的位置
			for (int j = 0;j < GRIDSIZE;j++) {
				if (gridInfo[i][j] == -np.who) {
					chessman[len_of_chessman].x = i;
					chessman[len_of_chessman].y = j;
					len_of_chessman++;
				}
			}
		}

		for (int the_man = 0;the_man < 4;the_man++) {
			int x = chessman[the_man].x;
			int y = chessman[the_man].y;

			for (int dire = 0;dire < 8;dire++) {
				int delta_x = dx[dire], delta_y = dy[dire];
				int trylen = 0;
				while (1) {
					++trylen;
					int try_x = trylen * delta_x + x;
					int try_y = trylen * delta_y + y;
					if (!inMap(try_x, try_y))
						break;
					if (gridInfo[try_x][try_y])
						break;

					gridInfo[x][y] = 0;
					can_move = 1;

					int ob_x = try_x;
					int ob_y = try_y;

					for (int ob_dire = 0;ob_dire < 8;ob_dire++) {
						int ob_delta_x = dx[ob_dire], ob_delta_y = dy[ob_dire];
						int ob_trylen = 0;
						while (1) {
							++ob_trylen;
							int ob_try_x = ob_trylen * ob_delta_x + ob_x;
							int ob_try_y = ob_trylen * ob_delta_y + ob_y;
							if (!inMap(ob_try_x, ob_try_y))
								break;
							if (gridInfo[ob_try_x][ob_try_y])
								break;
							np.submv.push_back({ x,y,try_x,try_y,ob_try_x,ob_try_y });
						}
					}

					gridInfo[x][y] = -np.who;					//还原
				}
			}
		}

		if (!can_move) {									   // 若达到输赢
			if (np.who == currBotColor)						   // 反向传播
				tr_back(f_p, 1);
			else tr_back(f_p, 0);
		}
		else {												  //若没有达到输赢 rollout

			int choose = rand() % np.submv.size();			  // 随机
			mv luckmv = np.submv[choose];
			// 创建新节点
			point* new_point = new point({ f_p,luckmv.x0,luckmv.y0,luckmv.x1,luckmv.y1,luckmv.x2,luckmv.y2, -np.who });
			np.sub_tree.push_back(new_point);

			point& chose_p = *(new_point);					  //临时简化节点，用于快速rollout
			simple_point tp_p = getSP(chose_p);

			double result = 0;								  // rollout
			roll_out(&tp_p, result, set_maxdepth(turn_num));

			gridInfo[tp_p.x2][tp_p.y2] = 0;					  // 还原棋盘
			gridInfo[tp_p.x1][tp_p.y1] = 0;
			gridInfo[tp_p.x0][tp_p.y0] = tp_p.who;

			tr_back(new_point, result);


			mv mid = np.submv[choose];						  // 交换位置
			np.submv[choose] = np.submv[0];
			np.submv[0] = mid;
			np.N_0_start++;
		}
	}
	else {													  // 若非空 , 可能未完全展开或已完全展开
		int tp_sz = np.submv.size();
		if (np.N_0_start < tp_sz) {							  // 若未完全展开	
															  // 随机
			int choose = rand() % (tp_sz - np.N_0_start) + np.N_0_start;
			mv luckmv = np.submv[choose];
			// 创建新节点
			point* new_point = new point({ f_p,luckmv.x0,luckmv.y0,luckmv.x1,luckmv.y1,luckmv.x2,luckmv.y2, -np.who });
			np.sub_tree.push_back(new_point);

			point& chose_p = *(new_point);					  // 临时简化节点，用于rollout
			simple_point tp_p = getSP(chose_p);

			double result = 0;
			roll_out(&tp_p, result, set_maxdepth(turn_num));

			gridInfo[tp_p.x2][tp_p.y2] = 0;
			gridInfo[tp_p.x1][tp_p.y1] = 0;
			gridInfo[tp_p.x0][tp_p.y0] = tp_p.who;

			tr_back(new_point, result);

			mv mid = np.submv[choose];						  // 交换位置
			np.submv[choose] = np.submv[np.N_0_start];
			np.submv[np.N_0_start] = mid;
			np.N_0_start++;
		}
		else {												  // 已完全展开,找到fv最大的子结点,递归

			double max_value = -1;
			point* max_sub_p = nullptr;
			double lgN = sqrt(log(np.N)), fv;				  // 提前计算
			int cl = -np.who;
			for (auto p : np.sub_tree) {					  // 找到fv最大子节点
				fv = ucb(p->v, p->N, cl, lgN);
				if (fv > max_value) {
					max_value = fv;
					max_sub_p = p;
				}
			}
			point* next_p = max_sub_p;
			gridInfo[next_p->x0][next_p->y0] = 0;
			gridInfo[next_p->x1][next_p->y1] = next_p->who;
			gridInfo[next_p->x2][next_p->y2] = OBSTACLE;

			MCTS(next_p);

			gridInfo[next_p->x2][next_p->y2] = 0;			  // 复原
			gridInfo[next_p->x1][next_p->y1] = 0;
			gridInfo[next_p->x0][next_p->y0] = next_p->who;
		}
	}
}

inline void tr_back(point* f_p, double q) {
	if ((*f_p).head_tree != nullptr) {
		(*f_p).v += q;
		(*f_p).N++;
		tr_back((*f_p).head_tree, q);
	}
	else {
		(*f_p).N++;
		(*f_p).v += q;
	}
}

inline double ucb(double& vi, int& ni, int who, double& lgN) {
	if (who == currBotColor)	return  vi / ni + C_ucb / sqrt(ni) * lgN;
	else return 1.0 - vi / ni + C_ucb / sqrt(ni) * lgN;
}

inline void roll_out(simple_point* f_p, double& result, int maxdep) {
	simple_point& np = (*f_p);
	gridInfo[np.x0][np.y0] = 0;
	gridInfo[np.x1][np.y1] = np.who;
	gridInfo[np.x2][np.y2] = OBSTACLE;
	if (maxdep == 0) {
		result = (-np.who == currBotColor) ? value(-np.who) : (1 - value(-np.who));
		return;
	}
	int next_who = -np.who;
	simple_point next_p;
	next_p.who = next_who;
	for (int s = 0;s < 4;s++) {
		next_p.mychess[s].x = np.mychess[s].x;
		next_p.mychess[s].y = np.mychess[s].y;
		next_p.hischess[s].x = np.hischess[s].x;
		next_p.hischess[s].y = np.hischess[s].y;
	}
	if (np.who == currBotColor) next_p.mychess[np.move_id].x = np.x1, next_p.mychess[np.move_id].y = np.y1;
	else next_p.hischess[np.move_id].x = np.x1, next_p.hischess[np.move_id].y = np.y1;
	bool finish = 0;
	{
		int cnt_1 = 0;

		for (int id = 0;id < 4;id++) {
			int x = (next_p.who == currBotColor) ? next_p.mychess[id].x : next_p.hischess[id].x;
			int y = (next_p.who == currBotColor) ? next_p.mychess[id].y : next_p.hischess[id].y;
			for (int dr = 0;dr < 8;dr++) {
				for (int try_len = 1;try_len < GRIDSIZE;try_len++) {
					int tx = try_len * dx[dr] + x, ty = try_len * dy[dr] + y;
					if (!inMap(tx, ty) || gridInfo[tx][ty])
						break;
					cnt_1++;
				}
			}
		}
		if (cnt_1 == 0) finish = 1;
		else
		{
			int lukymv = rand() % cnt_1 + 1;
			for (int id = 0;id < 4;id++) {
				int x = (next_p.who == currBotColor) ? next_p.mychess[id].x : next_p.hischess[id].x;
				int y = (next_p.who == currBotColor) ? next_p.mychess[id].y : next_p.hischess[id].y;
				for (int dr = 0;dr < 8;dr++) {
					for (int try_len = 1;try_len < GRIDSIZE;try_len++) {
						int tx = try_len * dx[dr] + x, ty = try_len * dy[dr] + y;
						if (!inMap(tx, ty) || gridInfo[tx][ty])
							break;
						lukymv--;
						if (lukymv == 0) {
							next_p.x0 = x, next_p.y0 = y;
							next_p.x1 = tx, next_p.y1 = ty;
							next_p.move_id = id;
							goto osd;
						}
					}
				}
			}
		}
	osd:;
	}
	if (finish) {
		if (np.who == currBotColor) result = 1;
	}
	else {
		int cnt_2 = 0;

		int x = next_p.x1, y = next_p.y1;
		gridInfo[next_p.x0][next_p.y0] = 0;
		for (int dr = 0;dr < 8;dr++) {
			for (int try_len = 1;try_len < GRIDSIZE;try_len++) {
				int tx = try_len * dx[dr] + x, ty = try_len * dy[dr] + y;
				if (!inMap(tx, ty) || gridInfo[tx][ty])
					break;
				cnt_2++;
			}
		}
		int luckymv = rand() % cnt_2 + 1;
		for (int dr = 0;dr < 8;dr++) {
			for (int try_len = 1;try_len < GRIDSIZE;try_len++) {
				int tx = try_len * dx[dr] + x, ty = try_len * dy[dr] + y;
				if (!inMap(tx, ty) || gridInfo[tx][ty])
					break;
				luckymv--;
				if (luckymv == 0) next_p.x2 = tx, next_p.y2 = ty;
			}
		}

		gridInfo[next_p.x0][next_p.y0] = next_p.who;

		roll_out(&next_p, result, maxdep - 1);

		gridInfo[next_p.x2][next_p.y2] = 0;
		gridInfo[next_p.x1][next_p.y1] = 0;
		gridInfo[next_p.x0][next_p.y0] = next_p.who;
	}
}

inline void get_there_step_queen(int color, position* chessman_me, position* chessman_him) { // queenmove bfs
	for (int id = 0;id < 4;id++) {
		position have_been[GRIDSIZE * GRIDSIZE] = { 0 };
		int s_p = 0, e_p = 1;
		int sx = chessman_me[id].x, sy = chessman_me[id].y;
		D_queen_me[sx][sy] = 0;
		have_been[0].x = sx, have_been[0].y = sy;
		gridInfo[sx][sy] = 0;
		for (int step = 1;;step++) {
			int cnt = 0;
			for (int i = s_p;i < e_p;i++) {
				int x = have_been[i].x, y = have_been[i].y;
				for (int dr = 0;dr < 8;dr++) {
					for (int tl = 1;tl < 8;tl++) {
						int tx = tl * dx[dr] + x, ty = tl * dy[dr] + y;
						if (!inMap(tx, ty) || gridInfo[tx][ty]) break;
						if (D_queen_me[tx][ty] > step) {
							D_queen_me[tx][ty] = step;
							have_been[e_p + cnt].x = tx, have_been[e_p + cnt].y = ty;
							++cnt;
						}
					}
				}
			}
			if (cnt == 0) break;
			s_p = e_p;
			e_p += cnt;
		}
		gridInfo[sx][sy] = color;
	}
	for (int id = 0;id < 4;id++) {
		position have_been[GRIDSIZE * GRIDSIZE] = { 0 };
		int s_p = 0, e_p = 1;
		int sx = chessman_him[id].x, sy = chessman_him[id].y;
		D_queen_him[sx][sy] = 0;
		have_been[0].x = sx, have_been[0].y = sy;
		gridInfo[sx][sy] = 0;
		for (int step = 1;;step++) {
			int cnt = 0;
			for (int i = s_p;i < e_p;i++) {
				int x = have_been[i].x, y = have_been[i].y;
				for (int dr = 0;dr < 8;dr++) {
					for (int tl = 1;tl < 8;tl++) {
						int tx = tl * dx[dr] + x, ty = tl * dy[dr] + y;
						if (!inMap(tx, ty) || gridInfo[tx][ty]) break;
						if (D_queen_him[tx][ty] > step) {
							D_queen_him[tx][ty] = step;
							have_been[e_p + cnt].x = tx, have_been[e_p + cnt].y = ty;
							++cnt;
						}
					}
				}
			}
			if (cnt == 0) break;
			s_p = e_p;
			e_p += cnt;
		}
		gridInfo[sx][sy] = -color;
	}
}

inline void set_parameter_value(int chess_man_num, double& a, double& b0, double& c, double& d, double& e, double& bb) {  // 设置权重
	a = w1[chess_man_num], b0 = w2[chess_man_num], c = w3[chess_man_num], d = w4[chess_man_num], e = w5[chess_man_num], bb = b[chess_man_num];
}
inline double mobility(int color, position* chessman_me, position* chessman_him) {
	int mob[GRIDSIZE][GRIDSIZE] = { 0 };
	for (int i = 0;i < GRIDSIZE;i++) {						// 计算空格的灵活度
		for (int j = 0;j < GRIDSIZE;j++) {
			if (!gridInfo[i][j]) {
				int cnt = 0;
				for (int dire = 0;dire < 8;dire++) {
					int try_x = dx[dire] + i;
					int try_y = dy[dire] + j;
					if (!inMap(try_x, try_y))
						continue;
					if (gridInfo[try_x][try_y])
						continue;
					cnt++;
				}
				mob[i][j] = cnt;
			}
		}
	}
	double sum1 = 0, sum2 = 0;								// 步骤2，对于使用queen走法能到达的
	for (int id = 0;id < 4;id++) {
		int x = chessman_me[id].x, y = chessman_me[id].y;
		for (int dr = 0;dr < 8;dr++) {
			for (int tl = 1;tl < 8;tl++) {
				int tx = dx[dr] * tl + x, ty = dy[dr] * tl + y;
				if (!inMap(tx, ty) || gridInfo[tx][ty]) break;
				sum1 += mob[tx][ty] * 1.0 / tl;
			}
		}
	}
	for (int id = 0;id < 4;id++) {
		int x = chessman_him[id].x, y = chessman_him[id].y;
		for (int dr = 0;dr < 8;dr++) {
			for (int tl = 1;tl < 8;tl++) {
				int tx = dx[dr] * tl + x, ty = dy[dr] * tl + y;
				if (!inMap(tx, ty) || gridInfo[tx][ty]) break;
				sum2 += mob[tx][ty] * 1.0 / tl;
			}
		}
	}
	return sum1 - sum2;
}

inline void get_there_step_king(int color, position* chessman_me, position* chessman_him) {
	for (int id = 0;id < 4;id++) {
		position have_been[GRIDSIZE * GRIDSIZE] = { 0 };
		int s_p = 0, e_p = 1;
		int sx = chessman_me[id].x, sy = chessman_me[id].y;
		D_king_me[sx][sy] = 0;
		have_been[0].x = sx, have_been[0].y = sy;
		gridInfo[sx][sy] = 0;
		for (int step = 1;;step++) {
			int cnt = 0;
			for (int i = s_p;i < e_p;i++) {
				int x = have_been[i].x, y = have_been[i].y;
				for (int dr = 0;dr < 8;dr++) {
					int tx = dx[dr] + x, ty = dy[dr] + y;
					if (!inMap(tx, ty) || gridInfo[tx][ty]) continue;
					if (D_king_me[tx][ty] > step) {
						D_king_me[tx][ty] = step;
						have_been[e_p + cnt].x = tx, have_been[e_p + cnt].y = ty;
						++cnt;
					}
				}
			}
			if (cnt == 0) break;
			s_p = e_p;
			e_p += cnt;
		}
		gridInfo[sx][sy] = color;
	}
	for (int id = 0;id < 4;id++) {
		position have_been[GRIDSIZE * GRIDSIZE] = { 0 };
		int s_p = 0, e_p = 1;
		int sx = chessman_him[id].x, sy = chessman_him[id].y;
		D_king_him[sx][sy] = 0;
		have_been[0].x = sx, have_been[0].y = sy;
		gridInfo[sx][sy] = 0;
		for (int step = 1;;step++) {
			int cnt = 0;
			for (int i = s_p;i < e_p;i++) {
				int x = have_been[i].x, y = have_been[i].y;
				for (int dr = 0;dr < 8;dr++) {
					int tx = dx[dr] + x, ty = dy[dr] + y;
					if (!inMap(tx, ty) || gridInfo[tx][ty]) continue;
					if (D_king_him[tx][ty] > step) {
						D_king_him[tx][ty] = step;
						have_been[e_p + cnt].x = tx, have_been[e_p + cnt].y = ty;
						++cnt;
					}
				}
			}
			if (cnt == 0) break;
			s_p = e_p;
			e_p += cnt;
		}
		gridInfo[sx][sy] = -color;
	}
}
inline double value(int color) {  //先行方,先行方胜率

	memset(D_queen_me, 1, sizeof(D_queen_me));
	memset(D_queen_him, 1, sizeof(D_queen_him));
	memset(D_king_me, 1, sizeof(D_king_me));
	memset(D_king_him, 1, sizeof(D_king_him));

	int chess_man_num = 0;
	position chessman_me[4];
	position chessman_him[4];
	int len_c_m = 0, len_c_h = 0;
	for (int i = 0;i < GRIDSIZE;i++) {
		for (int j = 0;j < GRIDSIZE;j++) {
			if (gridInfo[i][j] == OBSTACLE)
				chess_man_num++;
			else if (gridInfo[i][j] == color) {
				chessman_me[len_c_m].x = i;
				chessman_me[len_c_m].y = j;
				len_c_m++;
			}
			else if (gridInfo[i][j] == -color) {
				chessman_him[len_c_h].x = i;
				chessman_him[len_c_h].y = j;
				len_c_h++;
			}
		}
	}

	double a, b, c, d, e, bb;
	set_parameter_value(chess_man_num, a, b, c, d, e, bb);					// 设置权重参数

	get_there_step_queen(color, chessman_me, chessman_him);
	get_there_step_king(color, chessman_me, chessman_him);

	double t1 = 0, t2 = 0;													// 计算t1，t2
	for (int i = 0;i < GRIDSIZE;i++) {
		for (int j = 0;j < GRIDSIZE;j++) {
			if (!gridInfo[i][j]) {
				if (D_queen_me[i][j] == D_queen_him[i][j]) {
					if (D_queen_me[i][j] < 2) t1 += the_first_move_adv;
				}
				else {
					if (D_queen_me[i][j] < D_queen_him[i][j]) t1 += 1;
					else t2 += 1;
				}
			}
		}
	}

	double t3 = 0, t4 = 0;
	for (int i = 0;i < GRIDSIZE;i++) {
		for (int j = 0;j < GRIDSIZE;j++) {
			if (!gridInfo[i][j]) {
				if (D_king_me[i][j] == D_king_him[i][j]) {				    // 若相等且有限
					if (D_king_me[i][j] < 2) t3 += the_first_move_adv;
				}
				else {
					if (D_king_me[i][j] < D_king_him[i][j]) t3 += 1;
					else t4 += 1;
				}
			}
		}
	}

	double p = 0, pp = 0;
	for (int i = 0;i < GRIDSIZE;i++) {
		for (int j = 0;j < GRIDSIZE;j++) {
			if (!gridInfo[i][j]) {
				double tpd = max(-1.0, (D_king_him[i][j] - D_king_me[i][j]) / 6.0);
				pp += min(1.0, tpd);
				if (D_queen_me[i][j] < 6)
					p += 2.0 / qpow2(D_queen_me[i][j]);
				if (D_queen_him[i][j] < 6)
					p -= 2.0 / qpow2(D_queen_him[i][j]);
			}
		}
	}
	double mob = mobility(color, chessman_me, chessman_him) * 0.1;
	double v = a * (t1 - t2) + b * (t3 - t4) + c * p + d * pp + e * mob + bb;
	double r = 1.0 / (1 + exp(-v));
	return r;
}