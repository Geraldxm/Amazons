using System;
using System.IO;
using System.Threading;
using System.Windows.Forms;

namespace AmazonChessGUI
{
    public partial class SinglePlayerForm : Form
    {
        private ChessGame Game { get; set; }

        public SinglePlayerForm(ChessGame game)
        {
            Game = game;
            InitializeComponent();
        }

        private void SinglePlayerForm_Load(object sender, EventArgs e)
        {
            waitingLabel.Visible = false;

            chessTable.Init(Game);
        }

        public bool Saved { get; set; } = true;


        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (!Saved)
                if (MessageBox.Show("当前的棋局还未保存，确定要退出吗？", "提示",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2) == DialogResult.No)
                {
                    if (e is FormClosingEventArgs fc_e)
                        fc_e.Cancel = true;
                    return;
                }
            base.OnFormClosing(e);
        }

        private void ExitButton_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void HelpButton_Click(object sender, EventArgs e)
        {
#pragma warning disable IDE0067 // 丢失范围之前释放对象
            var form = new HelpForm();
#pragma warning restore IDE0067 // 丢失范围之前释放对象
            form.Show();
        }

        private void RegretButton_Click(object sender, EventArgs e)
        {
            if (Game.Text == "") return;
            string[] moves = Game.Text.Trim().Split('\n');
            if (moves.Length == 1 && chessTable.CurrentMove == ChessTable.WhoseMove.whiteMove) return;

            chessTable.ValidBoard = true;

            chessTable.UnselectSelected();

            if (chessTable.CurrentMove == ChessTable.WhoseMove.blackMove ||
                chessTable.CurrentMove == ChessTable.WhoseMove.whiteMove)
                chessTable.RegretOneMoveAndPaint();
            chessTable.RegretOneMoveAndPaint();
        }

        private void HintButton_Click(object sender, EventArgs e)
        {
            if (CheckIfNotMoved())
            {
                HintMessageShow();
                return;
            }
            chessTable.UnselectSelected();
            chessTable.AutoMoveOnce();
            chessTable.ManuallyRepaint();
            var newTrd = new Thread((ThreadStart)delegate
            {
                chessTable.ValidBoard = false;
                Thread.Sleep(3000);
                chessTable.ValidBoard = true;
                chessTable.AutoMoveOnce();
            });
            newTrd.Start();
        }

        private void LoadButton_Click(object sender, EventArgs e)
        {
            string path;
            using (OpenFileDialog dialog = new OpenFileDialog
            {
                Filter = "亚马逊棋存档文件(*.amz)|*.amz|" +
                    "文本文件(*.txt)|*.txt" +
                    "|所有文件(*.*)|*.*",
                FileName = "quick_save.amz",
                InitialDirectory = Environment.CurrentDirectory,
                DefaultExt = "amz"
            })
            {
                if (dialog.ShowDialog() == DialogResult.Cancel)
                    return;
                path = dialog.FileName;
            }
            if (path == "") return;
            ChessGame game = LoadAMZ.LoadGame(path);
            if (game == null) return;
            var newTrd = new Thread((ThreadStart)delegate
             {
                 Application.Run(new SinglePlayerForm(game));
             });
            newTrd.SetApartmentState(ApartmentState.STA);
            newTrd.Start();
            Close();
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            if (!chessTable.ValidBoard)
            {
                if (MessageBox.Show("这局游戏已经结束了！也许你想要把这局棋留作纪念？按“确定”可以保留局面的截图哦！" + Environment.NewLine +
                    "Tips: 按“取消”之后悔一步棋则可以保存局面为*.amz文件", "提示",
                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
                    == DialogResult.OK)
                {
                    string imgpath;
                    using (SaveFileDialog dialog = new SaveFileDialog
                    {
                        Filter = "png文件(*.png)|*.png|" +
                        "jpeg文件(*.jpg)|*.jpg|" +
                        "所有文件(*.*)|*.*",
                        FileName = "interesting.png",
                        InitialDirectory = Environment.CurrentDirectory,
                        DefaultExt = "png"
                    })
                    {
                        if (dialog.ShowDialog() == DialogResult.Cancel) return;
                        imgpath = dialog.FileName;
                    }
                    if (imgpath == "") return;
                    SaveAMZ.SaveBoardImage(imgpath, chessTable);
                }
                return;
            }
            if (!(chessTable.CurrentMove == ChessTable.WhoseMove.blackMove ||
                chessTable.CurrentMove == ChessTable.WhoseMove.whiteMove))
            {
                HintMessageShow("请在完成当前移动（放置Arrow）后再保存~");
                return;
            }
            string path;
            using (SaveFileDialog dialog = new SaveFileDialog
            {
                Filter = "亚马逊棋存档文件(*.amz)|*.amz|" +
                "文本文件(*.txt)|*.txt|" +
                "所有文件(*.*)|*.*",
                FileName = "quick_save.amz",
                InitialDirectory = Environment.CurrentDirectory,
                DefaultExt = "amz"
            })
            {
                if (dialog.ShowDialog() == DialogResult.Cancel) return;
                path = dialog.FileName;
            }
            if (path == "") return;
            SaveAMZ.SaveGame(path, Game);
            Saved = true;
        }

        private void NewButton_Click(object sender, EventArgs e)
        {
            var newTrd = new Thread((ThreadStart)delegate
            {
                Application.Run(new SinglePlayerForm(new ChessGame()));
            });
            newTrd.SetApartmentState(ApartmentState.STA);
            newTrd.Start();
            Close();
        }

        private bool CheckIfNotMoved()
        {
            return (chessTable.CurrentMove != ChessTable.WhoseMove.blackMove &&
                chessTable.CurrentMove != ChessTable.WhoseMove.whiteMove) ;
        }

        private void ReverseButton_Click(object sender, EventArgs e)
        {
            if (CheckIfNotMoved())
            {
                HintMessageShow();
                return;
            }
            if (!chessTable.ValidBoard)
            {
                HintMessageShow("这局游戏已经结束了！");
                return;
            }
            chessTable.UnselectSelected();
            if(!chessTable.AutoMoveOnce())
            {
                chessTable.MoveNext();
                chessTable.MoveNext();
            }
        }

        private void HintMessageShow(string text = "此操作仅当你还没有做本回合的移动时有效\r\n（可以单击“悔棋”按钮来撤回这一移动）"
            , string caption = "提示")
        {
            MessageBox.Show(text, caption,
                MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
