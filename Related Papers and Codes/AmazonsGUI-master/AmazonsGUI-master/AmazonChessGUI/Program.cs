using System;
using System.Windows.Forms;
using System.IO;

namespace AmazonChessGUI
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(Exit);
            Application.Run(new LoadPrev());
        }

        static void Exit(object sender, EventArgs e)
        {
            try
            {
                if (File.Exists("Amazons_recover.exe"))
                    File.Delete("Amazons_recover.exe");
            }
            catch (Exception) { }
        }
    }
}
