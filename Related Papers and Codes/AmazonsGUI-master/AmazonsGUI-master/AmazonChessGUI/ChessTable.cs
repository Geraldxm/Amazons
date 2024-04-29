using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Windows.Forms;

namespace AmazonChessGUI
{
    public partial class ChessTable : UserControl
    {
        public ChessGame Game { get; private set; }
        private SinglePlayerForm ParentSPF => (SinglePlayerForm)ParentForm;

        #region Initialization

        public ChessTable()
        {
            InitializeComponent();
            InitMatrix();

            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);

            this.MouseClick += ChessTable_MouseClick;
            this.Load += ChessTable_Load;
        }

        public bool Inited { get; private set; } = false;

        public void Init(ChessGame game)
        {
            Game = game;

            selected.nx = -1;
            selected.ny = -1;

            foreach (Move move in Game.GetMoves())
            {
                MovePaint(move);
            }

            if (Game.Text == "" || Game.Text.Trim().Split('\n').Length % 2 == 0)
                CurrentMove = WhoseMove.blackMove;
            else CurrentMove = WhoseMove.whiteMove;

            Invalidate();

            Inited = true;
        }

        private void ChessTable_Load(object sender, EventArgs e)
        {
            if (!Inited)
            {
                Init(new ChessGame());
            }

            Color color = Color.Black;
            PaintForXY(0, 2, color);
            PaintForXY(2, 0, color);
            PaintForXY(5, 0, color);
            PaintForXY(7, 2, color);
            color = Color.White;
            PaintForXY(0, 5, color);
            PaintForXY(2, 7, color);
            PaintForXY(5, 7, color);
            PaintForXY(7, 5, color);

            Invalidate();
        }

        #endregion

        // 纵线数
        public int Vertical => 9;
        // 横线数
        public int Horizontal => 9;

        public int Col { get { return Vertical - 1; } }
        public int Row { get { return Horizontal - 1; } }

        #region Matrix

        public class ChessPiece
        {
            public Color Color { set; get; }
            public bool IsOk { set; get; }
        }

        ChessPiece[,] _Matrix;

        public ref ChessPiece Piece(int nx, int ny) => ref _Matrix[ny, nx];
        public ref ChessPiece Piece(ChessLocation chess) => ref _Matrix[chess.ny, chess.nx];

        private void InitMatrix()
        {
            _Matrix = new ChessPiece[Row, Col];

            for (int i = 0; i < _Matrix.Length; i++)
            {
                Piece(i / Col, i % Col) = new ChessPiece
                {
                    Color = Color.Empty,
                    IsOk = false
                };
            }
        }

        #endregion

        #region Paint

        public float TileWidth { get { return (float)(1.0 * Width / Vertical); } }
        public float TileHeight { get { return (float)(1.0 * Height / Horizontal); } }

        // 线的颜色
        public Color ColorOfLine => Color.Black;

        protected override void OnPaint(PaintEventArgs pe)
        {
            pe.Graphics.SmoothingMode = SmoothingMode.HighSpeed;    // 绘制线

            using (Pen pen = new Pen(new SolidBrush(ColorOfLine), 2))
            {
                pen.StartCap = LineCap.Round;
                pen.EndCap = LineCap.Round;
                for (int row = 1; row <= Horizontal; row++)
                {
                    pe.Graphics.DrawLine(pen, (float)(TileWidth * 0.5), (float)(TileHeight * (row - 0.5)), (float)(TileWidth * (this.Vertical - 0.5)), (float)(TileHeight * (row - 0.5)));
                    base.OnPaint(pe);
                }

                for (int col = 1; col <= Vertical; col++)
                {
                    pe.Graphics.DrawLine(pen, (float)(TileWidth * (col - 0.5)), (float)(TileHeight * 0.5), (float)(TileWidth * (col - 0.5)), (float)(TileHeight * (this.Horizontal - 0.5)));
                    base.OnPaint(pe);
                }
            }

            pe.Graphics.SmoothingMode = SmoothingMode.HighQuality;     // 绘制棋子

            for (int ny = 0; ny < Row; ny++)
            {
                for (int nx = 0; nx < Col; nx++)
                {
                    ChessPiece piece = Piece(nx, ny);

                    if (piece.IsOk)
                    {
                        using (Pen pen = new Pen(Color.Black, (float)4.0))
                        {
                            using (SolidBrush solidBrush = new SolidBrush(piece.Color))
                            {
                                float x = (float)((nx + 0.7) * TileWidth);
                                float y = (float)((ny + 0.7) * TileHeight);
                                float width = (float)(TileWidth * 0.6);
                                float height = (float)(TileHeight * 0.6);

                                pe.Graphics.DrawEllipse(pen, x, y, width, height);
                                pe.Graphics.FillEllipse(solidBrush, x, y, width, height);
                                base.OnPaint(pe);
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

                                pe.Graphics.DrawEllipse(pen, x, y, width, height);
                                pe.Graphics.FillEllipse(solidBrush, x, y, width, height);
                                base.OnPaint(pe);
                            }
                        }
                    }
                }
            }
        }

        public void ManuallyRepaint() => OnPaint(new PaintEventArgs(
                CreateGraphics(),
                new Rectangle(0, 0, Width, Height)));

        void PaintForXY(int nx, int ny, Color color)
        {
            // check
            if (nx < 0 || ny < 0 || nx >= Horizontal - 1 || ny >= Vertical - 1)
                return;

            ChessPiece piece = Piece(nx, ny);
            // if (!piec.IsOk)
            // {
            piece.Color = color;
            piece.IsOk = true;

            Invalidate();
            // }
        }

        void MovePaint(Move move)
        {
            ChessPiece pieceSource = Piece(move.source);
            ChessPiece pieceDest = Piece(move.dest);
            ChessPiece pieceArrow = Piece(move.arrow);

            pieceDest.Color = pieceSource.Color;
            pieceDest.IsOk = true;

            pieceSource.Color = Color.Empty;
            pieceSource.IsOk = false;

            pieceArrow.Color = Color.DodgerBlue;
            pieceArrow.IsOk = true;

            Invalidate();
        }

        #endregion

        #region BasicMove

        public enum WhoseMove
        {
            blackMove = 0,
            blackArrow = 1,
            whiteMove = 2,
            whiteArrow = 3
        }

        private WhoseMove curMove = WhoseMove.blackMove;
        public WhoseMove CurrentMove
        { 
            get { return curMove; } 
            private set {
                if (value == WhoseMove.whiteMove)
                    ParentSPF.directionLabel.Text = "当前移动方\r\n白方";
                if (value == WhoseMove.blackMove)
                    ParentSPF.directionLabel.Text = "当前移动方\r\n黑方";
                curMove = value;
            } 
        }

        public static WhoseMove NextMove(WhoseMove move) => (move == WhoseMove.whiteArrow) ? WhoseMove.blackMove : (move + 1);
        public static WhoseMove PrevMove(WhoseMove move) => (move == WhoseMove.blackMove) ? WhoseMove.whiteArrow : (move - 1);

        public void MoveNext() => CurrentMove = NextMove(CurrentMove);
        public void MovePrev() => CurrentMove = PrevMove(CurrentMove);

        #endregion

        #region Check

        private bool CheckIfValid(int prev_nx, int prev_ny, int nx, int ny)
        {
            int[] moveDirection = { -1, 1, -16, 16, -15, 15, -17, 17 };
            foreach (var direction in moveDirection)
            {
                for (int step = 1; ; ++step)
                {
                    int idx = prev_nx * 16 + prev_ny + step * direction;
                    int x = idx / 16; int y = idx % 16;
                    if ((idx & 0x88) != 0 || Piece(x, y).IsOk)
                        break;
                    if (x == nx && y == ny)
                        return true;
                }
            }
            return false;
        }

        enum WhoWins
        {
            black,
            white,
            unknown
        }
        WhoWins CheckIfWinning()
        {
            bool blacklose = true, whitelose = true;
            for (int nx = 0; nx < 8; ++nx)
                for (int ny = 0; ny < 8; ++ny)
                {
                    for (int i = 0; i < 8; ++i)
                        for (int j = 0; j < 8; ++j)
                        {
                            if (Piece(i, j).IsOk && Piece(i, j).Color == Color.Black)
                                if (CheckIfValid(i, j, nx, ny))
                                    blacklose = false;

                            if (Piece(i, j).IsOk && Piece(i, j).Color == Color.White)
                                if (CheckIfValid(i, j, nx, ny))
                                    whitelose = false;
                        }
                }
            ValidBoard = false;
            if (blacklose)
                return WhoWins.white;
            if (whitelose)
                return WhoWins.black;
            ValidBoard = true;
            return WhoWins.unknown;
        }

        #endregion

        private ChessLocation selected;
        private ChessLocation lastPlacement;
        private bool validSelect = false;

        #region Unselect

        public void UnselectSelected()
        {
            if (CurrentMove == WhoseMove.blackMove || CurrentMove == WhoseMove.whiteMove)   // 该移动了
            {
                if (validSelect)
                {
                    ChessPiece piece = Piece(selected.nx, selected.ny);
                    if (CurrentMove == WhoseMove.blackMove)
                        piece.Color = Color.Black;
                    else if (CurrentMove == WhoseMove.whiteMove)
                        piece.Color = Color.White;
                    validSelect = false;

                    Invalidate();
                }
            }
        }

        #endregion

        #region MoveAndPaint

        public bool AutoMoveOnce()
        {
            WhoWins result = CheckIfWinning();
            if ((CurrentMove == WhoseMove.whiteMove && result == WhoWins.black) ||
                (CurrentMove == WhoseMove.blackMove && result == WhoWins.white))
            {
                MessageBox.Show("恭喜你，你赢了！", "YOU WIN", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }
            if ((CurrentMove == WhoseMove.whiteMove && result == WhoWins.white) ||
                (CurrentMove == WhoseMove.blackMove && result == WhoWins.black))
            {
                MessageBox.Show("不幸，你输了。", "YOU LOSE", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }
            if (!Game.AutoMoveNext(ParentSPF))
            {
                MessageBox.Show("调用计算程序失败！", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(0);
            }
            result = CheckIfWinning();
            if ((CurrentMove == WhoseMove.whiteMove && result == WhoWins.black) ||
                (CurrentMove == WhoseMove.blackMove && result == WhoWins.white))
            {
                MessageBox.Show("不幸，你输了。", "YOU LOSE", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            if ((CurrentMove == WhoseMove.whiteMove && result == WhoWins.white) ||
                (CurrentMove == WhoseMove.blackMove && result == WhoWins.black))
            {
                MessageBox.Show("恭喜你，你赢了！", "YOU WIN", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            return true;
        }

        public void CompleteAutoMoveAndPaint()
        {
            MovePaint(Game.GetMoves().Last());
            MoveNext();
            MoveNext();
        }

        public void RegretOneMoveAndPaint()
        {
            string[] moves = Game.Text.Trim().Split('\n');
            string lastMove = moves.Last();
            Game.Text = "";
            for (int i = 0; i < moves.Length - 1; ++i)
                Game.Text += moves[i] + Environment.NewLine;
            string[] coordinates = lastMove.Trim().Split(' ');

            if (coordinates.Length == 4 || coordinates.Length == 6)
            {
                ChessPiece pieceSource = Piece(
                    Convert.ToInt32(coordinates[0]), Convert.ToInt32(coordinates[1]));
                ChessPiece pieceDest = Piece(
                    Convert.ToInt32(coordinates[2]), Convert.ToInt32(coordinates[3]));

                if (coordinates.Length == 6)
                {
                    ChessPiece pieceArrow = Piece(
                        Convert.ToInt32(coordinates[4]), Convert.ToInt32(coordinates[5]));
                    pieceArrow.Color = Color.Empty;
                    pieceArrow.IsOk = false;

                    MovePrev();
                }

                pieceSource.Color = pieceDest.Color;
                pieceSource.IsOk = true;

                pieceDest.Color = Color.Empty;
                pieceDest.IsOk = false;

                MovePrev();
            }
            else
            {
                MessageBox.Show("坐标不合法！" + Environment.NewLine +
                    "（提示：请不要用文本编辑器编辑amz文件，否则可能导致此错误）", "Internal ERROR",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(0);
            }

            UnselectSelected();
            Invalidate();
        }

        #endregion

        public bool ValidBoard { get; set; } = true;

        #region MouseClick

        void ChessTable_MouseClick(object sender, MouseEventArgs e)
        {
            switch (e.Button)
            {
                case MouseButtons.Left:
                    // left -> place a chess

                    int nx = Convert.ToInt32(Math.Round(e.X / TileWidth - 1.0));
                    int ny = Convert.ToInt32(Math.Round(e.Y / TileHeight - 1.0));

                    if (nx < 0 || ny < 0 || nx >= Horizontal - 1 || ny >= Vertical - 1)
                        break;

                    float div_width = (float)((nx + 1.0) * TileWidth) - e.X;
                    float div_height = (float)((ny + 1.0) * TileHeight) - e.Y;

                    // 半径的平方值，用来确定是否容忍这次放置
                    float _RadiusSquare = (TileWidth * TileWidth + TileHeight * TileHeight) / 9;

                    if (div_width * div_width + div_height * div_height <= _RadiusSquare)
                    {
                        if (CurrentMove == WhoseMove.blackMove || CurrentMove == WhoseMove.whiteMove) // 该移动了
                        {
                            if (validSelect) //已选中
                            {
                                if (CheckIfValid(selected.nx, selected.ny, nx, ny))
                                {
                                    if (!(selected.nx == nx && selected.ny == ny))
                                    {
                                        ChessPiece piece = Piece(selected.nx, selected.ny);
                                        piece.IsOk = false;

                                        if (CurrentMove == WhoseMove.blackMove)
                                            PaintForXY(nx, ny, Color.Black);
                                        else
                                            PaintForXY(nx, ny, Color.White);

                                        Game.Text +=
                                            selected.nx + " " + selected.ny + " "
                                            + nx + " " + ny + " ";

                                        MoveNext();
                                        lastPlacement.nx = nx;
                                        lastPlacement.ny = ny;
                                        validSelect = false;
                                        ParentSPF.Saved = false;
                                    }
                                }
                                else
                                {
                                    ChessPiece piece = Piece(selected.nx, selected.ny);
                                    if (CurrentMove == WhoseMove.blackMove)
                                        piece.Color = Color.Black;
                                    else if (CurrentMove == WhoseMove.whiteMove)
                                        piece.Color = Color.White;
                                    validSelect = false;

                                    Invalidate();
                                }
                            }
                            // select
                            {
                                ChessPiece piece = Piece(nx, ny);
                                if (piece.IsOk)
                                {
                                    if (piece.Color == Color.Black && CurrentMove == WhoseMove.blackMove)
                                    {

                                        piece.Color = Color.DimGray;
                                        selected.nx = nx;
                                        selected.ny = ny;
                                        validSelect = true;

                                        Invalidate();
                                    }
                                    else if (piece.Color == Color.White && CurrentMove == WhoseMove.whiteMove)
                                    {

                                        piece.Color = Color.LemonChiffon;
                                        selected.nx = nx;
                                        selected.ny = ny;
                                        validSelect = true;

                                        Invalidate();
                                    }
                                }
                            }
                        }
                        else        // 该放障碍物了
                        {
                            ChessPiece piece = Piece(nx, ny);
                            if (!piece.IsOk)
                            {
                                if (CheckIfValid(lastPlacement.nx, lastPlacement.ny, nx, ny))
                                {
                                    PaintForXY(nx, ny, Color.DodgerBlue);
                                    ManuallyRepaint();

                                    Game.Text += nx + " " + ny + Environment.NewLine;

                                    MoveNext();

                                    AutoMoveOnce();
                                }
                            }
                        }
                    }

                    break;

                case MouseButtons.Right:
                    // right -> unselect
                    UnselectSelected();
                    break;
            }
        }
        #endregion
    }
}
