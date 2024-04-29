namespace AmazonChessGUI
{
    partial class SinglePlayerForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SinglePlayerForm));
            this.saveButton = new System.Windows.Forms.Button();
            this.loadButton = new System.Windows.Forms.Button();
            this.newButton = new System.Windows.Forms.Button();
            this.regretButton = new System.Windows.Forms.Button();
            this.hintButton = new System.Windows.Forms.Button();
            this.helpButton = new System.Windows.Forms.Button();
            this.exitButton = new System.Windows.Forms.Button();
            this.reverseButton = new System.Windows.Forms.Button();
            this.waitingLabel = new System.Windows.Forms.Label();
            this.chessTable = new AmazonChessGUI.ChessTable();
            this.directionLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(405, 78);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 1;
            this.saveButton.Text = "保存";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.SaveButton_Click);
            // 
            // loadButton
            // 
            this.loadButton.Location = new System.Drawing.Point(405, 36);
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(75, 23);
            this.loadButton.TabIndex = 2;
            this.loadButton.Text = "读取";
            this.loadButton.UseVisualStyleBackColor = true;
            this.loadButton.Click += new System.EventHandler(this.LoadButton_Click);
            // 
            // newButton
            // 
            this.newButton.Location = new System.Drawing.Point(405, 118);
            this.newButton.Name = "newButton";
            this.newButton.Size = new System.Drawing.Size(75, 23);
            this.newButton.TabIndex = 3;
            this.newButton.Text = "新游戏";
            this.newButton.UseVisualStyleBackColor = true;
            this.newButton.Click += new System.EventHandler(this.NewButton_Click);
            // 
            // regretButton
            // 
            this.regretButton.Location = new System.Drawing.Point(405, 311);
            this.regretButton.Name = "regretButton";
            this.regretButton.Size = new System.Drawing.Size(75, 23);
            this.regretButton.TabIndex = 4;
            this.regretButton.Text = "悔棋";
            this.regretButton.UseVisualStyleBackColor = true;
            this.regretButton.Click += new System.EventHandler(this.RegretButton_Click);
            // 
            // hintButton
            // 
            this.hintButton.Location = new System.Drawing.Point(405, 349);
            this.hintButton.Name = "hintButton";
            this.hintButton.Size = new System.Drawing.Size(75, 23);
            this.hintButton.TabIndex = 5;
            this.hintButton.Text = "提示";
            this.hintButton.UseVisualStyleBackColor = true;
            this.hintButton.Click += new System.EventHandler(this.HintButton_Click);
            // 
            // helpButton
            // 
            this.helpButton.Location = new System.Drawing.Point(36, 398);
            this.helpButton.Name = "helpButton";
            this.helpButton.Size = new System.Drawing.Size(75, 23);
            this.helpButton.TabIndex = 6;
            this.helpButton.Text = "帮助";
            this.helpButton.UseVisualStyleBackColor = true;
            this.helpButton.Click += new System.EventHandler(this.HelpButton_Click);
            // 
            // exitButton
            // 
            this.exitButton.Location = new System.Drawing.Point(307, 398);
            this.exitButton.Name = "exitButton";
            this.exitButton.Size = new System.Drawing.Size(75, 23);
            this.exitButton.TabIndex = 7;
            this.exitButton.Text = "退出";
            this.exitButton.UseVisualStyleBackColor = true;
            this.exitButton.Click += new System.EventHandler(this.ExitButton_Click);
            // 
            // reverseButton
            // 
            this.reverseButton.Location = new System.Drawing.Point(405, 271);
            this.reverseButton.Name = "reverseButton";
            this.reverseButton.Size = new System.Drawing.Size(75, 23);
            this.reverseButton.TabIndex = 8;
            this.reverseButton.Text = "交换";
            this.reverseButton.UseVisualStyleBackColor = true;
            this.reverseButton.Click += new System.EventHandler(this.ReverseButton_Click);
            // 
            // waitingLabel
            // 
            this.waitingLabel.AutoSize = true;
            this.waitingLabel.Font = new System.Drawing.Font("Consolas", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.waitingLabel.Location = new System.Drawing.Point(158, 399);
            this.waitingLabel.Name = "waitingLabel";
            this.waitingLabel.Size = new System.Drawing.Size(110, 22);
            this.waitingLabel.TabIndex = 9;
            this.waitingLabel.Text = "Waiting...";
            // 
            // chessTable
            // 
            this.chessTable.Location = new System.Drawing.Point(17, 17);
            this.chessTable.Name = "chessTable";
            this.chessTable.Size = new System.Drawing.Size(382, 375);
            this.chessTable.TabIndex = 0;
            this.chessTable.ValidBoard = true;
            // 
            // directionLabel
            // 
            this.directionLabel.AutoSize = true;
            this.directionLabel.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.directionLabel.Location = new System.Drawing.Point(409, 190);
            this.directionLabel.Name = "directionLabel";
            this.directionLabel.Size = new System.Drawing.Size(68, 34);
            this.directionLabel.TabIndex = 10;
            this.directionLabel.Text = "当前移动方\r\n黑方";
            this.directionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // SinglePlayerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Peru;
            this.ClientSize = new System.Drawing.Size(498, 444);
            this.Controls.Add(this.directionLabel);
            this.Controls.Add(this.waitingLabel);
            this.Controls.Add(this.reverseButton);
            this.Controls.Add(this.exitButton);
            this.Controls.Add(this.helpButton);
            this.Controls.Add(this.hintButton);
            this.Controls.Add(this.regretButton);
            this.Controls.Add(this.newButton);
            this.Controls.Add(this.loadButton);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.chessTable);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.HelpButton = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "SinglePlayerForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Let\'s Play Amazons!";
            this.Load += new System.EventHandler(this.SinglePlayerForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public ChessTable chessTable;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.Button loadButton;
        private System.Windows.Forms.Button newButton;
        private System.Windows.Forms.Button regretButton;
        private System.Windows.Forms.Button hintButton;
        private System.Windows.Forms.Button helpButton;
        private System.Windows.Forms.Button exitButton;
        private System.Windows.Forms.Button reverseButton;
        public System.Windows.Forms.Label waitingLabel;
        public System.Windows.Forms.Label directionLabel;
    }
}