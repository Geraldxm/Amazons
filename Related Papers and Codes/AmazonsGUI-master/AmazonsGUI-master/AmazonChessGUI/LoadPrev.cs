using System;
using System.Drawing;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using AmazonChessGUI.Properties;

namespace AmazonChessGUI
{
    public partial class LoadPrev : Form
    {
        public LoadPrev()
        {
            InitializeComponent();
        }

        #region 响应鼠标拖拽

        // 这段代码为什么能工作？这我就不知道了，是从Github上下载的，反正能用就行了（x

        const int CURSOR_HTLEFT = 10;
        const int CURSOR_HTRIGHT = 11;
        const int CURSOR_HTTOP = 12;
        const int CURSOR_HTTOPLEFT = 13;
        const int CURSOR_HTTOPRIGHT = 14;
        const int CURSOR_HTBOTTOM = 15;
        const int CURSOR_HTBOTTOMLEFT = 16;
        const int CURSOR_HTBOTTOMRIGHT = 17;

        protected override void WndProc(ref Message msg)
        {
            switch (msg.Msg)
            {
                case 0x0084:
                    base.WndProc(ref msg);
                    Point vPoint = new Point((int)msg.LParam & 0xFFFF,
                        (int)msg.LParam >> 16 & 0xFFFF);
                    vPoint = PointToClient(vPoint);
                    if (vPoint.X <= 5)
                        if (vPoint.Y <= 5)
                            msg.Result = (IntPtr)CURSOR_HTTOPLEFT;
                        else if (vPoint.Y >= ClientSize.Height - 5)
                            msg.Result = (IntPtr)CURSOR_HTBOTTOMLEFT;
                        else msg.Result = (IntPtr)CURSOR_HTLEFT;
                    else if (vPoint.X >= ClientSize.Width - 5)
                        if (vPoint.Y <= 5)
                            msg.Result = (IntPtr)CURSOR_HTTOPRIGHT;
                        else if (vPoint.Y >= ClientSize.Height - 5)
                            msg.Result = (IntPtr)CURSOR_HTBOTTOMRIGHT;
                        else msg.Result = (IntPtr)CURSOR_HTRIGHT;
                    else if (vPoint.Y <= 5)
                        msg.Result = (IntPtr)CURSOR_HTTOP;
                    else if (vPoint.Y >= ClientSize.Height - 5)
                        msg.Result = (IntPtr)CURSOR_HTBOTTOM;
                    break;
                case 0x0201:
                    msg.Msg = 0x00A1;
                    msg.LParam = IntPtr.Zero;
                    msg.WParam = new IntPtr(2);
                    base.WndProc(ref msg);
                    break;
                default:
                    base.WndProc(ref msg);
                    break;
            }
        }

        #endregion

        private void LoadButton_Click(object sender, EventArgs e)
        {
            string path;
            /*
            if (MessageBox.Show("是否进行快速载入？", "提示",
                MessageBoxButtons.YesNo, MessageBoxIcon.Question,
                MessageBoxDefaultButton.Button1) == DialogResult.Yes)
                path = Environment.CurrentDirectory + "\\quick_save.amz";
            else
            */
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

        private void LoadPrev_Load(object sender, EventArgs e)
        {
            try
            {
                if (File.Exists("Amazons_recover.exe"))
                    File.Delete("Amazons_recover.exe");
                var fs = File.OpenWrite("Amazons_recover.exe");
                fs.Write(Resources.Amazons_recover_gcc, 0, Resources.Amazons_recover_gcc.Length);
                fs.Close();
            }
            catch (Exception)
            {
                MessageBox.Show("写文件失败", "启动失败", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(0);
            }
        }
    }
}
