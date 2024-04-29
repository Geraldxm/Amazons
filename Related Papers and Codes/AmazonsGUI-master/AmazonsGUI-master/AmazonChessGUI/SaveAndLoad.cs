using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace AmazonChessGUI
{
    static class SaveAMZ
    {
        static public bool SaveGame(string filename, in ChessGame game)
        {
            try
            {
                StreamWriter reader = new StreamWriter(filename, false);
                reader.Write(game.Text);
                reader.Close();
                // stream.Close();
            }
            catch (Exception)
            {
                MessageBox.Show("写文件失败！", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;
        }

        static public bool SaveBoardImage(string filename, in ChessTable table)
        {
            try
            {
                Point pt = table.PointToScreen(table.Location);
                var W = 4 * table.Width;
                var H = 4 * table.Height;

                var img = GetImg(table, W, H, 6);
                SaveToFile(img, filename, GetFormat(filename));
            }
            catch (Exception)
            {
                MessageBox.Show("写文件失败！", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;
        }
        static private Image GetImg(in ChessTable table, int w, int h, int linewidth = 2)
        {
            Bitmap bm = new Bitmap(w, h);
            Graphics g = Graphics.FromImage(bm);
            g.SmoothingMode = SmoothingMode.HighQuality;

            using (SolidBrush solidBrush = new SolidBrush(Color.Peru))
            {
                g.FillRectangle(solidBrush, new Rectangle(0, 0, w, h));
            }

            var TileWidth = 1.0 * h / table.Vertical;
            var TileHeight = 1.0 * h / table.Horizontal;

            using (Pen pen = new Pen(new SolidBrush(table.ColorOfLine), linewidth))
            {
                pen.StartCap = LineCap.Round;
                pen.EndCap = LineCap.Round;
                for (int row = 1; row <= table.Horizontal; row++)
                {
                    g.DrawLine(pen, (float)(TileWidth * 0.5), (float)(TileHeight * (row - 0.5)),
                        (float)(TileWidth * (table.Vertical - 0.5)), (float)(TileHeight * (row - 0.5)));
                }

                for (int col = 1; col <= table.Vertical; col++)
                {
                    g.DrawLine(pen, (float)(TileWidth * (col - 0.5)), (float)(TileHeight * 0.5),
                        (float)(TileWidth * (col - 0.5)), (float)(TileHeight * (table.Horizontal - 0.5)));
                }
            }

            for (int ny = 0; ny < table.Row; ny++)
            {
                for (int nx = 0; nx < table.Col; nx++)
                {
                    ChessTable.ChessPiece piece = table.Piece(nx, ny);

                    if (piece.IsOk)
                    {
                        using (Pen pen = new Pen(Color.Black, linewidth * 2))
                        {
                            using (SolidBrush solidBrush = new SolidBrush(piece.Color))
                            {
                                float x = (float)((nx + 0.7) * TileWidth);
                                float y = (float)((ny + 0.7) * TileHeight);
                                float width = (float)(TileWidth * 0.6);
                                float height = (float)(TileHeight * 0.6);

                                g.DrawEllipse(pen, x, y, width, height);
                                g.FillEllipse(solidBrush, x, y, width, height);
                            }
                        }
                    }
                    else
                    {
                        using (Pen pen = new Pen(Color.Peru, (float)0.0))
                        {
                            using (SolidBrush solidBrush = new SolidBrush(Color.Peru))
                            {
                                float x = (float)((nx + 0.5 + 0.2) * TileWidth);
                                float y = (float)((ny + 0.5 + 0.2) * TileHeight);
                                float width = (float)(TileWidth * 0.6);
                                float height = (float)(TileHeight * 0.6);

                                g.DrawEllipse(pen, x, y, width, height);
                                g.FillEllipse(solidBrush, x, y, width, height);
                            }
                        }
                    }
                }
            }

            return bm;
        }

        static private void SaveToFile(Image pic, string filename, ImageFormat format)
        {
            if (File.Exists(filename))
                File.Delete(filename);

            if (format is null) format = GetFormat(filename);

            if (format == ImageFormat.MemoryBmp) pic.Save(filename);
            else pic.Save(filename, format);
        }

        static private ImageFormat GetFormat(string filename)
        {
            ImageFormat format = ImageFormat.MemoryBmp;
            string ext = Path.GetExtension(filename).ToLower();

            if (ext.Equals(".png")) format = ImageFormat.Png;
            else if (ext.Equals(".jpg") || ext.Equals(".jpeg")) format = ImageFormat.Jpeg;
            else if (ext.Equals(".bmp")) format = ImageFormat.Bmp;
            else if (ext.Equals(".gif")) format = ImageFormat.Gif;
            else if (ext.Equals(".ico")) format = ImageFormat.Icon;
            else if (ext.Equals(".emf")) format = ImageFormat.Emf;
            else if (ext.Equals(".exif")) format = ImageFormat.Exif;
            else if (ext.Equals(".tiff")) format = ImageFormat.Tiff;
            else if (ext.Equals(".wmf")) format = ImageFormat.Wmf;
            else if (ext.Equals(".memorybmp")) format = ImageFormat.MemoryBmp;

            return format;
        }
    }
    static class LoadAMZ
    {
        static public ChessGame LoadGame(string filename)
        {
            ChessGame game = new ChessGame();
            try
            {
                StreamReader reader = new StreamReader(filename);
                /*
                Stack<string> remainders = new Stack<string>();
                for (; !reader.EndOfStream;)
                {
                    string remainder = reader.ReadLine();
                    remainders.Push(remainder);
                }
                */
                game.Text = reader.ReadToEnd().Trim() + Environment.NewLine;
                if (game.Text == Environment.NewLine) game.Text = "";
                reader.Close();
                // stream.Close();
            }
            catch (FileNotFoundException)
            {
                if (MessageBox.Show("文件不存在，是否重试？", "文件不存在",
                    MessageBoxButtons.RetryCancel, MessageBoxIcon.Error, MessageBoxDefaultButton.Button2)
                    == DialogResult.Cancel)
                    return null;
                else
                    return LoadGame(filename);
            }
            catch (Exception)
            {
                MessageBox.Show("读文件失败！", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return null;
            }
            return game;
        }
    }
}
