namespace AmazonChessGUI
{
    partial class AmazonEntry
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.SinglePlayerButton = new System.Windows.Forms.Button();
            this.MultiPlayersButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("微软雅黑", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.label1.Location = new System.Drawing.Point(81, 56);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(217, 28);
            this.label1.TabIndex = 0;
            this.label1.Text = "请选择你的游戏模式~";
            // 
            // SinglePlayerButton
            // 
            this.SinglePlayerButton.Location = new System.Drawing.Point(86, 120);
            this.SinglePlayerButton.Name = "SinglePlayerButton";
            this.SinglePlayerButton.Size = new System.Drawing.Size(75, 23);
            this.SinglePlayerButton.TabIndex = 1;
            this.SinglePlayerButton.Text = "单人！";
            this.SinglePlayerButton.UseVisualStyleBackColor = true;
            // 
            // MultiPlayersButton
            // 
            this.MultiPlayersButton.Location = new System.Drawing.Point(218, 120);
            this.MultiPlayersButton.Name = "MultiPlayersButton";
            this.MultiPlayersButton.Size = new System.Drawing.Size(75, 23);
            this.MultiPlayersButton.TabIndex = 2;
            this.MultiPlayersButton.Text = "多人！";
            this.MultiPlayersButton.UseVisualStyleBackColor = true;
            this.MultiPlayersButton.Click += new System.EventHandler(this.MultiPlayersButton_Click);
            // 
            // AmazonEntry
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DodgerBlue;
            this.ClientSize = new System.Drawing.Size(380, 192);
            this.Controls.Add(this.MultiPlayersButton);
            this.Controls.Add(this.SinglePlayerButton);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AmazonEntry";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "AmazonEntry";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button SinglePlayerButton;
        private System.Windows.Forms.Button MultiPlayersButton;
    }
}

