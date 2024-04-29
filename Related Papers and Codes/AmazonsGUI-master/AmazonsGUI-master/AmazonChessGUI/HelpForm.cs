using System;
using System.Windows.Forms;

namespace AmazonChessGUI
{
    public partial class HelpForm : Form
    {
        public HelpForm()
        {
            InitializeComponent();
        }

        private void HelpForm_Load(object sender, EventArgs e)
        {
            textBox.Text = "MINI亚马逊棋基本规则：" + Environment.NewLine +
                "·  在8×8的棋盘上，每方有四个棋子（即四个Amazons）。" + Environment.NewLine +
                "·  每个棋子的行棋方法与国际象棋中的皇后相同，可以在八个方向上任意行走，但不能穿过阻碍。" + Environment.NewLine +
                "·  当轮到一方行棋时，此方只能而且必须移动四个Amazons中的一个。" + Environment.NewLine +
                "·  移动棋子后，由其释放一个障碍。" + Environment.NewLine +
                "·  障碍的释放方法与棋子的移动方法相同（即皇后的走法），同样，障碍的放置也是必须的。" + Environment.NewLine +
                "·  若某方完成某次移动后，对方四个棋子均不能再移动，则对方输掉比赛。" + Environment.NewLine +
                "·  每次开局位于黑方的玩家先手。" + Environment.NewLine +
                "·  注意，整个比赛中双方均不能吃掉对方或己方的棋子或障碍。" + Environment.NewLine
                ;
            textBox.Text += Environment.NewLine;
            textBox.Text += "本软件操作说明：" + Environment.NewLine +
                "·  默认状态下，玩家执黑，计算机执白。障碍用蓝色棋子表示。" + Environment.NewLine +
                "·  单击棋子，棋子变色，说明已选中。" + Environment.NewLine +
                "·  再左键单击要移动到的位置，即完成移动。" + Environment.NewLine +
                "·  注意，如果该次移动不合法，将会取消选中。单击鼠标右键同样取消选中。" + Environment.NewLine +
                "·  再单击所要放置障碍的地方，就完成了你的这一步。" + Environment.NewLine +
                "·  注意，如果障碍物的放置不合法，将不会有任何反应。" + Environment.NewLine +
                "·  计算机将会思考1s左右的时间，然后给出它的走棋。之后，你可以走下一步棋。" + Environment.NewLine +
                "·  玩家可以点击“交换”来交换走子顺序。此操作仅当你还没有做本回合的移动时有效。" + Environment.NewLine +
                "·  本软件支持存盘和读盘功能。你可以随时保存游戏。" + Environment.NewLine +
                "·  本软件支持“悔棋”功能。当你完成一次移动后，单击“悔棋”可以撤回。若上一步是计算机所走，则“悔棋”操作将会撤回到你走的上一步之前。" + Environment.NewLine +
                "·  本软件支持“提示”功能。单击“提示”，计算机将会帮你走一步。此操作仅当你还没有做本回合的移动时有效。" + Environment.NewLine +
                "·  在计算机思考1s左右的时间并给出提示后，会暂停3s来给你观看给出的提示，之后计算机将完成它的移动。为了方便查看具体做出了什么移动，移动的源的边框被暂时保留。"
                ;
        }
    }
}
