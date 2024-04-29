# AmazonsGUI
下亚马逊棋的程序的配套UI

#### 软件各源文件的主要内容和功能：

**Program.cs**  应用程序的入口点

**LoadPrev.cs**  入口窗体，询问用户是开始新游戏还是加载已保存的游戏

**SinglePlayerForm.cs**  Amazons游戏的主窗体

**HelpForm.cs**  “帮助”窗体

**xx.Designer.cs**  自动生成的窗体初始化器

**SaveAndLoad.cs**  实现读盘和存盘

**Game.cs**  定义ChessGame等结构，与用于计算智能落子的C++程序Amazons_recover.exe的交互部分也在这里

**ChessTable.cs**  GUI部分的核心代码所在，完成绘制棋盘、初始化棋局、响应鼠标点击等重要功能

**AmazonEntry.cs** **MultiplePlayesForm.cs**  暂时废弃
