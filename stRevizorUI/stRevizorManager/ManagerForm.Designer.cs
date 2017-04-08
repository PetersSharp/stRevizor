namespace stRevizor
{
    partial class Manager
    {
        /// <summary>
        /// Требуется переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Обязательный метод для поддержки конструктора - не изменяйте
        /// содержимое данного метода при помощи редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Manager));
            this.timerDrop = new System.Windows.Forms.Timer(this.components);
            this.fileSystemWatch = new System.IO.FileSystemWatcher();
            this.EPMain = new System.Windows.Forms.ErrorProvider(this.components);
            this.NotifyMain = new System.Windows.Forms.NotifyIcon(this.components);
            this.SFDMain = new System.Windows.Forms.SaveFileDialog();
            this.timerSrvInfo = new System.Windows.Forms.Timer(this.components);
            this.OFDMain = new System.Windows.Forms.FolderBrowserDialog();
            this.formSkinMain = new stCoreUI.FormSkin();
            this.flatGroupBox3 = new stCoreUI.FlatGroupBox();
            this.pictureBoxLoadWait = new System.Windows.Forms.PictureBox();
            this.flatContextMenuSendServer = new stCoreUI.FlatContextMenuStrip();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.flatLabel44 = new stCoreUI.FlatLabel();
            this.flatLabel45 = new stCoreUI.FlatLabel();
            this.flatLabel43 = new stCoreUI.FlatLabel();
            this.flatLabel63 = new stCoreUI.FlatLabel();
            this.flatTabControlMain = new stCoreUI.FlatTabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.flatButtonReload = new stCoreUI.FlatButton();
            this.flatLabel33 = new stCoreUI.FlatLabel();
            this.flatButtonTimerDrop = new stCoreUI.FlatButton();
            this.flatNumericTimerDrop = new stCoreUI.FlatNumeric();
            this.flatButtonStartStop = new stCoreUI.FlatButton();
            this.flatAlertBoxInfo = new stCoreUI.FlatAlertBox();
            this.flatGroupBoxServaerInfo = new stCoreUI.FlatGroupBox();
            this.flatLabel40 = new stCoreUI.FlatLabel();
            this.flatLabel41 = new stCoreUI.FlatLabel();
            this.flatLabel38 = new stCoreUI.FlatLabel();
            this.ServerInfoBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.flatLabel39 = new stCoreUI.FlatLabel();
            this.flatLabel36 = new stCoreUI.FlatLabel();
            this.flatLabel37 = new stCoreUI.FlatLabel();
            this.flatLabel34 = new stCoreUI.FlatLabel();
            this.flatLabel35 = new stCoreUI.FlatLabel();
            this.flatLabel31 = new stCoreUI.FlatLabel();
            this.flatLabel32 = new stCoreUI.FlatLabel();
            this.flatLabel30 = new stCoreUI.FlatLabel();
            this.flatLabel29 = new stCoreUI.FlatLabel();
            this.flatLabel27 = new stCoreUI.FlatLabel();
            this.flatLabel25 = new stCoreUI.FlatLabel();
            this.flatLabel26 = new stCoreUI.FlatLabel();
            this.flatLabel28 = new stCoreUI.FlatLabel();
            this.flatLabel24 = new stCoreUI.FlatLabel();
            this.flatLabel23 = new stCoreUI.FlatLabel();
            this.flatLabel22 = new stCoreUI.FlatLabel();
            this.flatLabel21 = new stCoreUI.FlatLabel();
            this.flatLabel20 = new stCoreUI.FlatLabel();
            this.flatLabel19 = new stCoreUI.FlatLabel();
            this.flatLabel17 = new stCoreUI.FlatLabel();
            this.flatLabel18 = new stCoreUI.FlatLabel();
            this.flatLabel15 = new stCoreUI.FlatLabel();
            this.flatLabel16 = new stCoreUI.FlatLabel();
            this.flatLabel13 = new stCoreUI.FlatLabel();
            this.flatLabel14 = new stCoreUI.FlatLabel();
            this.flatLabel11 = new stCoreUI.FlatLabel();
            this.flatLabel12 = new stCoreUI.FlatLabel();
            this.flatLabel9 = new stCoreUI.FlatLabel();
            this.flatLabel10 = new stCoreUI.FlatLabel();
            this.flatLabel7 = new stCoreUI.FlatLabel();
            this.flatLabel8 = new stCoreUI.FlatLabel();
            this.flatLabel6 = new stCoreUI.FlatLabel();
            this.flatLabel5 = new stCoreUI.FlatLabel();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.flatGroupBox2 = new stCoreUI.FlatGroupBox();
            this.flatLabel42 = new stCoreUI.FlatLabel();
            this.flatToggle1 = new stCoreUI.FlatToggle();
            this.flatLabel1 = new stCoreUI.FlatLabel();
            this.pictureBoxFolder = new System.Windows.Forms.PictureBox();
            this.flatTextBoxFolder = new stCoreUI.FlatTextBox();
            this.flatGroupBox1 = new stCoreUI.FlatGroupBox();
            this.flatButtonSave = new stCoreUI.FlatButton();
            this.flatLabel4 = new stCoreUI.FlatLabel();
            this.flatLabel3 = new stCoreUI.FlatLabel();
            this.flatLabel2 = new stCoreUI.FlatLabel();
            this.flatRadioButtonFTP = new stCoreUI.FlatRadioButton();
            this.flatRadioButtonTFTP = new stCoreUI.FlatRadioButton();
            this.flatTextBoxServerFTPPass = new stCoreUI.FlatTextBox();
            this.flatTextBoxServerFTPLogin = new stCoreUI.FlatTextBox();
            this.flatTextBoxServer = new stCoreUI.FlatTextBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.ListBoxLog = new System.Windows.Forms.ListBox();
            this.flatContextMenuLog = new stCoreUI.FlatContextMenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.labelVerion = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.linkLabel2 = new System.Windows.Forms.LinkLabel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.flatStatusBarInfo = new stCoreUI.FlatStatusBar();
            this.flatClose1 = new stCoreUI.FlatClose();
            this.flatMinimize = new stCoreUI.FlatMini();
            this.timerSendServer = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatch)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EPMain)).BeginInit();
            this.formSkinMain.SuspendLayout();
            this.flatGroupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLoadWait)).BeginInit();
            this.flatContextMenuSendServer.SuspendLayout();
            this.flatTabControlMain.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.flatGroupBoxServaerInfo.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ServerInfoBindingSource)).BeginInit();
            this.tabPage2.SuspendLayout();
            this.flatGroupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxFolder)).BeginInit();
            this.flatGroupBox1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.flatContextMenuLog.SuspendLayout();
            this.tabPage4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // timerDrop
            // 
            this.timerDrop.Interval = 1000;
            this.timerDrop.Tick += new System.EventHandler(this.timerDrop_Tick);
            // 
            // fileSystemWatch
            // 
            this.fileSystemWatch.EnableRaisingEvents = true;
            this.fileSystemWatch.NotifyFilter = System.IO.NotifyFilters.FileName;
            this.fileSystemWatch.SynchronizingObject = this;
            this.fileSystemWatch.Changed += new System.IO.FileSystemEventHandler(this.fileSystemWatch_Processed);
            this.fileSystemWatch.Created += new System.IO.FileSystemEventHandler(this.fileSystemWatch_Processed);
            // 
            // EPMain
            // 
            this.EPMain.ContainerControl = this;
            // 
            // NotifyMain
            // 
            this.NotifyMain.Click += new System.EventHandler(this.NotifyMain_Click);
            // 
            // SFDMain
            // 
            this.SFDMain.CreatePrompt = true;
            resources.ApplyResources(this.SFDMain, "SFDMain");
            this.SFDMain.RestoreDirectory = true;
            // 
            // timerSrvInfo
            // 
            this.timerSrvInfo.Interval = 3600000;
            this.timerSrvInfo.Tick += new System.EventHandler(this.timerSrvInfo_Tick);
            // 
            // OFDMain
            // 
            this.OFDMain.SelectedPath = global::stRevizor.Properties.Settings.Default.SetupFolder;
            // 
            // formSkinMain
            // 
            this.formSkinMain.BackColor = System.Drawing.Color.White;
            this.formSkinMain.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.formSkinMain.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(53)))), ((int)(((byte)(58)))), ((int)(((byte)(60)))));
            this.formSkinMain.Controls.Add(this.flatGroupBox3);
            this.formSkinMain.Controls.Add(this.flatTabControlMain);
            this.formSkinMain.Controls.Add(this.flatStatusBarInfo);
            this.formSkinMain.Controls.Add(this.flatClose1);
            this.formSkinMain.Controls.Add(this.flatMinimize);
            resources.ApplyResources(this.formSkinMain, "formSkinMain");
            this.formSkinMain.DoubleBufferAlways = false;
            this.formSkinMain.FlatColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.formSkinMain.HeaderColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.formSkinMain.HeaderMaximize = false;
            this.formSkinMain.Name = "formSkinMain";
            this.formSkinMain.WinShadow = false;
            // 
            // flatGroupBox3
            // 
            this.flatGroupBox3.BackColor = System.Drawing.Color.Transparent;
            this.flatGroupBox3.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(0)))));
            this.flatGroupBox3.Controls.Add(this.pictureBoxLoadWait);
            this.flatGroupBox3.Controls.Add(this.flatLabel44);
            this.flatGroupBox3.Controls.Add(this.flatLabel45);
            this.flatGroupBox3.Controls.Add(this.flatLabel43);
            this.flatGroupBox3.Controls.Add(this.flatLabel63);
            resources.ApplyResources(this.flatGroupBox3, "flatGroupBox3");
            this.flatGroupBox3.Name = "flatGroupBox3";
            this.flatGroupBox3.ShowText = true;
            // 
            // pictureBoxLoadWait
            // 
            this.pictureBoxLoadWait.Cursor = System.Windows.Forms.Cursors.Default;
            resources.ApplyResources(this.pictureBoxLoadWait, "pictureBoxLoadWait");
            this.pictureBoxLoadWait.Name = "pictureBoxLoadWait";
            this.pictureBoxLoadWait.TabStop = false;
            // 
            // flatContextMenuSendServer
            // 
            resources.ApplyResources(this.flatContextMenuSendServer, "flatContextMenuSendServer");
            this.flatContextMenuSendServer.ForeColor = System.Drawing.Color.White;
            this.flatContextMenuSendServer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem3});
            this.flatContextMenuSendServer.Name = "flatContextMenuSendServer";
            this.flatContextMenuSendServer.ShowImageMargin = false;
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Image = global::stRevizor.Properties.Resources.imgResendServer;
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            resources.ApplyResources(this.toolStripMenuItem3, "toolStripMenuItem3");
            this.toolStripMenuItem3.Click += new System.EventHandler(this.toolStripMenuItem3_Click);
            // 
            // flatLabel44
            // 
            this.flatLabel44.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel44.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "DateUpServer", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatLabel44, "flatLabel44");
            this.flatLabel44.ForeColor = System.Drawing.Color.LimeGreen;
            this.flatLabel44.Name = "flatLabel44";
            this.flatLabel44.Text = global::stRevizor.Properties.Settings.Default.DateUpServer;
            // 
            // flatLabel45
            // 
            resources.ApplyResources(this.flatLabel45, "flatLabel45");
            this.flatLabel45.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel45.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel45.Name = "flatLabel45";
            // 
            // flatLabel43
            // 
            this.flatLabel43.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel43.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "DateUpList", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatLabel43, "flatLabel43");
            this.flatLabel43.ForeColor = System.Drawing.Color.LimeGreen;
            this.flatLabel43.Name = "flatLabel43";
            this.flatLabel43.Text = global::stRevizor.Properties.Settings.Default.DateUpList;
            // 
            // flatLabel63
            // 
            resources.ApplyResources(this.flatLabel63, "flatLabel63");
            this.flatLabel63.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel63.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel63.Name = "flatLabel63";
            // 
            // flatTabControlMain
            // 
            this.flatTabControlMain.ActiveColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatTabControlMain.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.flatTabControlMain.Controls.Add(this.tabPage1);
            this.flatTabControlMain.Controls.Add(this.tabPage2);
            this.flatTabControlMain.Controls.Add(this.tabPage3);
            this.flatTabControlMain.Controls.Add(this.tabPage4);
            this.flatTabControlMain.Cursor = System.Windows.Forms.Cursors.Default;
            resources.ApplyResources(this.flatTabControlMain, "flatTabControlMain");
            this.flatTabControlMain.Name = "flatTabControlMain";
            this.flatTabControlMain.SelectedIndex = 0;
            this.flatTabControlMain.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage1.Controls.Add(this.flatButtonReload);
            this.tabPage1.Controls.Add(this.flatLabel33);
            this.tabPage1.Controls.Add(this.flatButtonTimerDrop);
            this.tabPage1.Controls.Add(this.flatNumericTimerDrop);
            this.tabPage1.Controls.Add(this.flatButtonStartStop);
            this.tabPage1.Controls.Add(this.flatAlertBoxInfo);
            this.tabPage1.Controls.Add(this.flatGroupBoxServaerInfo);
            resources.ApplyResources(this.tabPage1, "tabPage1");
            this.tabPage1.Name = "tabPage1";
            // 
            // flatButtonReload
            // 
            this.flatButtonReload.BackColor = System.Drawing.Color.Transparent;
            this.flatButtonReload.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatButtonReload.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatButtonReload, "flatButtonReload");
            this.flatButtonReload.Name = "flatButtonReload";
            this.flatButtonReload.Rounded = false;
            this.flatButtonReload.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.flatButtonReload.Click += new System.EventHandler(this.flatButtonReload_Click);
            // 
            // flatLabel33
            // 
            this.flatLabel33.BackColor = System.Drawing.Color.Transparent;
            resources.ApplyResources(this.flatLabel33, "flatLabel33");
            this.flatLabel33.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel33.Name = "flatLabel33";
            // 
            // flatButtonTimerDrop
            // 
            this.flatButtonTimerDrop.BackColor = System.Drawing.Color.Transparent;
            this.flatButtonTimerDrop.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatButtonTimerDrop.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatButtonTimerDrop, "flatButtonTimerDrop");
            this.flatButtonTimerDrop.Name = "flatButtonTimerDrop";
            this.flatButtonTimerDrop.Rounded = false;
            this.flatButtonTimerDrop.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.flatButtonTimerDrop.Click += new System.EventHandler(this.flatButtonTimerDrop_Click);
            // 
            // flatNumericTimerDrop
            // 
            this.flatNumericTimerDrop.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatNumericTimerDrop.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.flatNumericTimerDrop.ButtonColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            resources.ApplyResources(this.flatNumericTimerDrop, "flatNumericTimerDrop");
            this.flatNumericTimerDrop.ForeColor = System.Drawing.Color.White;
            this.flatNumericTimerDrop.Maximum = ((long)(9999999));
            this.flatNumericTimerDrop.Minimum = ((long)(0));
            this.flatNumericTimerDrop.Name = "flatNumericTimerDrop";
            this.flatNumericTimerDrop.Value = ((long)(0));
            // 
            // flatButtonStartStop
            // 
            this.flatButtonStartStop.BackColor = System.Drawing.Color.Transparent;
            this.flatButtonStartStop.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatButtonStartStop.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatButtonStartStop, "flatButtonStartStop");
            this.flatButtonStartStop.Name = "flatButtonStartStop";
            this.flatButtonStartStop.Rounded = false;
            this.flatButtonStartStop.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.flatButtonStartStop.Click += new System.EventHandler(this.flatButtonStartStop_Click);
            // 
            // flatAlertBoxInfo
            // 
            this.flatAlertBoxInfo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatAlertBoxInfo.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatAlertBoxInfo, "flatAlertBoxInfo");
            this.flatAlertBoxInfo.kind = stCoreUI.FlatAlertBox._Kind.Success;
            this.flatAlertBoxInfo.Name = "flatAlertBoxInfo";
            // 
            // flatGroupBoxServaerInfo
            // 
            this.flatGroupBoxServaerInfo.BackColor = System.Drawing.Color.Transparent;
            this.flatGroupBoxServaerInfo.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(0)))));
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel40);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel41);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel38);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel39);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel36);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel37);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel34);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel35);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel31);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel32);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel30);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel29);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel27);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel25);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel26);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel28);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel24);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel23);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel22);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel21);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel20);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel19);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel17);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel18);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel15);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel16);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel13);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel14);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel11);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel12);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel9);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel10);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel7);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel8);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel6);
            this.flatGroupBoxServaerInfo.Controls.Add(this.flatLabel5);
            resources.ApplyResources(this.flatGroupBoxServaerInfo, "flatGroupBoxServaerInfo");
            this.flatGroupBoxServaerInfo.Name = "flatGroupBoxServaerInfo";
            this.flatGroupBoxServaerInfo.ShowText = true;
            // 
            // flatLabel40
            // 
            this.flatLabel40.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel40.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupFolder", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatLabel40, "flatLabel40");
            this.flatLabel40.ForeColor = System.Drawing.Color.LimeGreen;
            this.flatLabel40.Name = "flatLabel40";
            this.flatLabel40.Text = global::stRevizor.Properties.Settings.Default.SetupFolder;
            // 
            // flatLabel41
            // 
            resources.ApplyResources(this.flatLabel41, "flatLabel41");
            this.flatLabel41.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel41.ForeColor = System.Drawing.Color.MediumAquamarine;
            this.flatLabel41.Name = "flatLabel41";
            // 
            // flatLabel38
            // 
            resources.ApplyResources(this.flatLabel38, "flatLabel38");
            this.flatLabel38.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel38.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Filter", true));
            this.flatLabel38.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel38.Name = "flatLabel38";
            // 
            // ServerInfoBindingSource
            // 
            this.ServerInfoBindingSource.DataSource = typeof(stRevizor.ServerInfo.SiRoot);
            // 
            // flatLabel39
            // 
            resources.ApplyResources(this.flatLabel39, "flatLabel39");
            this.flatLabel39.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel39.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel39.Name = "flatLabel39";
            // 
            // flatLabel36
            // 
            resources.ApplyResources(this.flatLabel36, "flatLabel36");
            this.flatLabel36.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel36.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Dnsfilter.Filter", true));
            this.flatLabel36.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel36.Name = "flatLabel36";
            // 
            // flatLabel37
            // 
            resources.ApplyResources(this.flatLabel37, "flatLabel37");
            this.flatLabel37.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel37.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel37.Name = "flatLabel37";
            // 
            // flatLabel34
            // 
            resources.ApplyResources(this.flatLabel34, "flatLabel34");
            this.flatLabel34.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel34.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Urlfilter.Filter", true));
            this.flatLabel34.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel34.Name = "flatLabel34";
            // 
            // flatLabel35
            // 
            resources.ApplyResources(this.flatLabel35, "flatLabel35");
            this.flatLabel35.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel35.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel35.Name = "flatLabel35";
            // 
            // flatLabel31
            // 
            resources.ApplyResources(this.flatLabel31, "flatLabel31");
            this.flatLabel31.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel31.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Dnsfilter.Wild", true));
            this.flatLabel31.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel31.Name = "flatLabel31";
            // 
            // flatLabel32
            // 
            resources.ApplyResources(this.flatLabel32, "flatLabel32");
            this.flatLabel32.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel32.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel32.Name = "flatLabel32";
            // 
            // flatLabel30
            // 
            resources.ApplyResources(this.flatLabel30, "flatLabel30");
            this.flatLabel30.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel30.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Urlfilter.Uri", true));
            this.flatLabel30.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel30.Name = "flatLabel30";
            // 
            // flatLabel29
            // 
            resources.ApplyResources(this.flatLabel29, "flatLabel29");
            this.flatLabel29.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel29.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Dnsfilter.Host", true));
            this.flatLabel29.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel29.Name = "flatLabel29";
            // 
            // flatLabel27
            // 
            resources.ApplyResources(this.flatLabel27, "flatLabel27");
            this.flatLabel27.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel27.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Urlfilter.Host", true));
            this.flatLabel27.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel27.Name = "flatLabel27";
            // 
            // flatLabel25
            // 
            resources.ApplyResources(this.flatLabel25, "flatLabel25");
            this.flatLabel25.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel25.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Urlfilter.Port", true));
            this.flatLabel25.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel25.Name = "flatLabel25";
            // 
            // flatLabel26
            // 
            resources.ApplyResources(this.flatLabel26, "flatLabel26");
            this.flatLabel26.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel26.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel26.Name = "flatLabel26";
            // 
            // flatLabel28
            // 
            resources.ApplyResources(this.flatLabel28, "flatLabel28");
            this.flatLabel28.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel28.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel28.Name = "flatLabel28";
            // 
            // flatLabel24
            // 
            resources.ApplyResources(this.flatLabel24, "flatLabel24");
            this.flatLabel24.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel24.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel24.Name = "flatLabel24";
            // 
            // flatLabel23
            // 
            resources.ApplyResources(this.flatLabel23, "flatLabel23");
            this.flatLabel23.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel23.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel23.Name = "flatLabel23";
            // 
            // flatLabel22
            // 
            resources.ApplyResources(this.flatLabel22, "flatLabel22");
            this.flatLabel22.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel22.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel22.Name = "flatLabel22";
            // 
            // flatLabel21
            // 
            resources.ApplyResources(this.flatLabel21, "flatLabel21");
            this.flatLabel21.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel21.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel21.Name = "flatLabel21";
            // 
            // flatLabel20
            // 
            resources.ApplyResources(this.flatLabel20, "flatLabel20");
            this.flatLabel20.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel20.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel20.Name = "flatLabel20";
            // 
            // flatLabel19
            // 
            this.flatLabel19.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel19.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupServer", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.flatLabel19.DataBindings.Add(new System.Windows.Forms.Binding("Tag", this.ServerInfoBindingSource, "Filter", true));
            resources.ApplyResources(this.flatLabel19, "flatLabel19");
            this.flatLabel19.ForeColor = System.Drawing.Color.LimeGreen;
            this.flatLabel19.Name = "flatLabel19";
            this.flatLabel19.Text = global::stRevizor.Properties.Settings.Default.SetupServer;
            // 
            // flatLabel17
            // 
            resources.ApplyResources(this.flatLabel17, "flatLabel17");
            this.flatLabel17.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel17.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel17.Name = "flatLabel17";
            // 
            // flatLabel18
            // 
            resources.ApplyResources(this.flatLabel18, "flatLabel18");
            this.flatLabel18.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel18.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Datelist", true));
            this.flatLabel18.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel18.Name = "flatLabel18";
            // 
            // flatLabel15
            // 
            resources.ApplyResources(this.flatLabel15, "flatLabel15");
            this.flatLabel15.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel15.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel15.Name = "flatLabel15";
            // 
            // flatLabel16
            // 
            resources.ApplyResources(this.flatLabel16, "flatLabel16");
            this.flatLabel16.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel16.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Xmlparser.Enable", true));
            this.flatLabel16.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel16.Name = "flatLabel16";
            // 
            // flatLabel13
            // 
            resources.ApplyResources(this.flatLabel13, "flatLabel13");
            this.flatLabel13.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel13.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel13.Name = "flatLabel13";
            // 
            // flatLabel14
            // 
            resources.ApplyResources(this.flatLabel14, "flatLabel14");
            this.flatLabel14.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel14.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Urlfilter.Enable", true));
            this.flatLabel14.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel14.Name = "flatLabel14";
            // 
            // flatLabel11
            // 
            resources.ApplyResources(this.flatLabel11, "flatLabel11");
            this.flatLabel11.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel11.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel11.Name = "flatLabel11";
            // 
            // flatLabel12
            // 
            resources.ApplyResources(this.flatLabel12, "flatLabel12");
            this.flatLabel12.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel12.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Dnsfilter.Enable", true));
            this.flatLabel12.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel12.Name = "flatLabel12";
            // 
            // flatLabel9
            // 
            resources.ApplyResources(this.flatLabel9, "flatLabel9");
            this.flatLabel9.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel9.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel9.Name = "flatLabel9";
            // 
            // flatLabel10
            // 
            resources.ApplyResources(this.flatLabel10, "flatLabel10");
            this.flatLabel10.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel10.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Ips", true));
            this.flatLabel10.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel10.Name = "flatLabel10";
            // 
            // flatLabel7
            // 
            resources.ApplyResources(this.flatLabel7, "flatLabel7");
            this.flatLabel7.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel7.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel7.Name = "flatLabel7";
            // 
            // flatLabel8
            // 
            resources.ApplyResources(this.flatLabel8, "flatLabel8");
            this.flatLabel8.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel8.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Ipt", true));
            this.flatLabel8.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel8.Name = "flatLabel8";
            // 
            // flatLabel6
            // 
            resources.ApplyResources(this.flatLabel6, "flatLabel6");
            this.flatLabel6.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel6.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel6.Name = "flatLabel6";
            // 
            // flatLabel5
            // 
            resources.ApplyResources(this.flatLabel5, "flatLabel5");
            this.flatLabel5.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel5.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.ServerInfoBindingSource, "Ipfw", true));
            this.flatLabel5.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel5.Name = "flatLabel5";
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage2.Controls.Add(this.flatGroupBox2);
            this.tabPage2.Controls.Add(this.flatGroupBox1);
            resources.ApplyResources(this.tabPage2, "tabPage2");
            this.tabPage2.Name = "tabPage2";
            // 
            // flatGroupBox2
            // 
            this.flatGroupBox2.BackColor = System.Drawing.Color.Transparent;
            this.flatGroupBox2.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatGroupBox2.Controls.Add(this.flatLabel42);
            this.flatGroupBox2.Controls.Add(this.flatToggle1);
            this.flatGroupBox2.Controls.Add(this.flatLabel1);
            this.flatGroupBox2.Controls.Add(this.pictureBoxFolder);
            this.flatGroupBox2.Controls.Add(this.flatTextBoxFolder);
            resources.ApplyResources(this.flatGroupBox2, "flatGroupBox2");
            this.flatGroupBox2.Name = "flatGroupBox2";
            this.flatGroupBox2.ShowText = true;
            // 
            // flatLabel42
            // 
            this.flatLabel42.BackColor = System.Drawing.Color.Transparent;
            resources.ApplyResources(this.flatLabel42, "flatLabel42");
            this.flatLabel42.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel42.Name = "flatLabel42";
            // 
            // flatToggle1
            // 
            this.flatToggle1.BackColor = System.Drawing.Color.Transparent;
            this.flatToggle1.Checked = global::stRevizor.Properties.Settings.Default.SetupLogEnable;
            this.flatToggle1.Cursor = System.Windows.Forms.Cursors.Hand;
            this.flatToggle1.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::stRevizor.Properties.Settings.Default, "SetupLogEnable", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatToggle1, "flatToggle1");
            this.flatToggle1.Name = "flatToggle1";
            this.flatToggle1.Options = stCoreUI.FlatToggle._Options.Style1;
            // 
            // flatLabel1
            // 
            this.flatLabel1.BackColor = System.Drawing.Color.Transparent;
            resources.ApplyResources(this.flatLabel1, "flatLabel1");
            this.flatLabel1.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel1.Name = "flatLabel1";
            // 
            // pictureBoxFolder
            // 
            resources.ApplyResources(this.pictureBoxFolder, "pictureBoxFolder");
            this.pictureBoxFolder.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pictureBoxFolder.Image = global::stRevizor.Properties.Resources.imgFolder;
            this.pictureBoxFolder.Name = "pictureBoxFolder";
            this.pictureBoxFolder.TabStop = false;
            this.pictureBoxFolder.Click += new System.EventHandler(this.pictureBoxFolder_Click);
            // 
            // flatTextBoxFolder
            // 
            this.flatTextBoxFolder.BackColor = System.Drawing.Color.Transparent;
            this.flatTextBoxFolder.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupFolder", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.flatTextBoxFolder.FocusOnHover = false;
            resources.ApplyResources(this.flatTextBoxFolder, "flatTextBoxFolder");
            this.flatTextBoxFolder.MaxLength = 32767;
            this.flatTextBoxFolder.Multiline = false;
            this.flatTextBoxFolder.Name = "flatTextBoxFolder";
            this.flatTextBoxFolder.ReadOnly = true;
            this.flatTextBoxFolder.SelectedText = "";
            this.flatTextBoxFolder.SelectionLength = 0;
            this.flatTextBoxFolder.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("flatTextBoxFolder.SpellMark")));
            this.flatTextBoxFolder.Text = global::stRevizor.Properties.Settings.Default.SetupFolder;
            this.flatTextBoxFolder.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.flatTextBoxFolder.TextCaption = null;
            this.flatTextBoxFolder.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.flatTextBoxFolder.UseSystemPasswordChar = false;
            // 
            // flatGroupBox1
            // 
            this.flatGroupBox1.BackColor = System.Drawing.Color.Transparent;
            this.flatGroupBox1.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatGroupBox1.Controls.Add(this.flatButtonSave);
            this.flatGroupBox1.Controls.Add(this.flatLabel4);
            this.flatGroupBox1.Controls.Add(this.flatLabel3);
            this.flatGroupBox1.Controls.Add(this.flatLabel2);
            this.flatGroupBox1.Controls.Add(this.flatRadioButtonFTP);
            this.flatGroupBox1.Controls.Add(this.flatRadioButtonTFTP);
            this.flatGroupBox1.Controls.Add(this.flatTextBoxServerFTPPass);
            this.flatGroupBox1.Controls.Add(this.flatTextBoxServerFTPLogin);
            this.flatGroupBox1.Controls.Add(this.flatTextBoxServer);
            resources.ApplyResources(this.flatGroupBox1, "flatGroupBox1");
            this.flatGroupBox1.Name = "flatGroupBox1";
            this.flatGroupBox1.ShowText = true;
            // 
            // flatButtonSave
            // 
            this.flatButtonSave.BackColor = System.Drawing.Color.Transparent;
            this.flatButtonSave.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatButtonSave.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatButtonSave, "flatButtonSave");
            this.flatButtonSave.Name = "flatButtonSave";
            this.flatButtonSave.Rounded = false;
            this.flatButtonSave.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.flatButtonSave.Click += new System.EventHandler(this.flatButtonSave_Click);
            // 
            // flatLabel4
            // 
            resources.ApplyResources(this.flatLabel4, "flatLabel4");
            this.flatLabel4.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel4.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel4.Name = "flatLabel4";
            // 
            // flatLabel3
            // 
            resources.ApplyResources(this.flatLabel3, "flatLabel3");
            this.flatLabel3.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel3.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel3.Name = "flatLabel3";
            // 
            // flatLabel2
            // 
            resources.ApplyResources(this.flatLabel2, "flatLabel2");
            this.flatLabel2.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel2.ForeColor = System.Drawing.Color.Gainsboro;
            this.flatLabel2.Name = "flatLabel2";
            // 
            // flatRadioButtonFTP
            // 
            this.flatRadioButtonFTP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatRadioButtonFTP.Checked = global::stRevizor.Properties.Settings.Default.SetupServerType;
            this.flatRadioButtonFTP.Cursor = System.Windows.Forms.Cursors.Hand;
            this.flatRadioButtonFTP.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::stRevizor.Properties.Settings.Default, "SetupServerType", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatRadioButtonFTP, "flatRadioButtonFTP");
            this.flatRadioButtonFTP.Name = "flatRadioButtonFTP";
            this.flatRadioButtonFTP.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.flatRadioButtonFTP.CheckedChanged += new System.EventHandler(this.flatRadioButtonFTP_CheckedChanged);
            // 
            // flatRadioButtonTFTP
            // 
            this.flatRadioButtonTFTP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatRadioButtonTFTP.Checked = false;
            this.flatRadioButtonTFTP.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.flatRadioButtonTFTP, "flatRadioButtonTFTP");
            this.flatRadioButtonTFTP.Name = "flatRadioButtonTFTP";
            this.flatRadioButtonTFTP.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.flatRadioButtonTFTP.CheckedChanged += new System.EventHandler(this.flatRadioButtonTFTP_CheckedChanged);
            // 
            // flatTextBoxServerFTPPass
            // 
            this.flatTextBoxServerFTPPass.BackColor = System.Drawing.Color.Transparent;
            this.flatTextBoxServerFTPPass.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupServerFTPPass", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatTextBoxServerFTPPass, "flatTextBoxServerFTPPass");
            this.flatTextBoxServerFTPPass.FocusOnHover = false;
            this.flatTextBoxServerFTPPass.MaxLength = 32767;
            this.flatTextBoxServerFTPPass.Multiline = false;
            this.flatTextBoxServerFTPPass.Name = "flatTextBoxServerFTPPass";
            this.flatTextBoxServerFTPPass.ReadOnly = false;
            this.flatTextBoxServerFTPPass.SelectedText = "";
            this.flatTextBoxServerFTPPass.SelectionLength = 0;
            this.flatTextBoxServerFTPPass.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("flatTextBoxServerFTPPass.SpellMark")));
            this.flatTextBoxServerFTPPass.Text = global::stRevizor.Properties.Settings.Default.SetupServerFTPPass;
            this.flatTextBoxServerFTPPass.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.flatTextBoxServerFTPPass.TextCaption = null;
            this.flatTextBoxServerFTPPass.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.flatTextBoxServerFTPPass.UseSystemPasswordChar = true;
            // 
            // flatTextBoxServerFTPLogin
            // 
            this.flatTextBoxServerFTPLogin.BackColor = System.Drawing.Color.Transparent;
            this.flatTextBoxServerFTPLogin.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupServerFTPLogin", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            resources.ApplyResources(this.flatTextBoxServerFTPLogin, "flatTextBoxServerFTPLogin");
            this.flatTextBoxServerFTPLogin.FocusOnHover = false;
            this.flatTextBoxServerFTPLogin.MaxLength = 32767;
            this.flatTextBoxServerFTPLogin.Multiline = false;
            this.flatTextBoxServerFTPLogin.Name = "flatTextBoxServerFTPLogin";
            this.flatTextBoxServerFTPLogin.ReadOnly = false;
            this.flatTextBoxServerFTPLogin.SelectedText = "";
            this.flatTextBoxServerFTPLogin.SelectionLength = 0;
            this.flatTextBoxServerFTPLogin.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("flatTextBoxServerFTPLogin.SpellMark")));
            this.flatTextBoxServerFTPLogin.Text = global::stRevizor.Properties.Settings.Default.SetupServerFTPLogin;
            this.flatTextBoxServerFTPLogin.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.flatTextBoxServerFTPLogin.TextCaption = null;
            this.flatTextBoxServerFTPLogin.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.flatTextBoxServerFTPLogin.UseSystemPasswordChar = false;
            // 
            // flatTextBoxServer
            // 
            this.flatTextBoxServer.BackColor = System.Drawing.Color.Transparent;
            this.flatTextBoxServer.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::stRevizor.Properties.Settings.Default, "SetupServer", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.flatTextBoxServer.FocusOnHover = false;
            resources.ApplyResources(this.flatTextBoxServer, "flatTextBoxServer");
            this.flatTextBoxServer.MaxLength = 32767;
            this.flatTextBoxServer.Multiline = false;
            this.flatTextBoxServer.Name = "flatTextBoxServer";
            this.flatTextBoxServer.ReadOnly = false;
            this.flatTextBoxServer.SelectedText = "";
            this.flatTextBoxServer.SelectionLength = 0;
            this.flatTextBoxServer.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("flatTextBoxServer.SpellMark")));
            this.flatTextBoxServer.Text = global::stRevizor.Properties.Settings.Default.SetupServer;
            this.flatTextBoxServer.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.flatTextBoxServer.TextCaption = null;
            this.flatTextBoxServer.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.flatTextBoxServer.UseSystemPasswordChar = false;
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage3.Controls.Add(this.ListBoxLog);
            resources.ApplyResources(this.tabPage3, "tabPage3");
            this.tabPage3.Name = "tabPage3";
            // 
            // ListBoxLog
            // 
            this.ListBoxLog.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.ListBoxLog.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ListBoxLog.ContextMenuStrip = this.flatContextMenuLog;
            this.ListBoxLog.ForeColor = System.Drawing.Color.Gainsboro;
            this.ListBoxLog.FormattingEnabled = true;
            resources.ApplyResources(this.ListBoxLog, "ListBoxLog");
            this.ListBoxLog.Name = "ListBoxLog";
            // 
            // flatContextMenuLog
            // 
            resources.ApplyResources(this.flatContextMenuLog, "flatContextMenuLog");
            this.flatContextMenuLog.ForeColor = System.Drawing.Color.White;
            this.flatContextMenuLog.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.toolStripMenuItem2});
            this.flatContextMenuLog.Name = "flatContextMenuLog";
            this.flatContextMenuLog.ShowImageMargin = false;
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Image = global::stRevizor.Properties.Resources.imgFolderOpen;
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            resources.ApplyResources(this.toolStripMenuItem1, "toolStripMenuItem1");
            this.toolStripMenuItem1.Click += new System.EventHandler(this.toolStripMenuItem1_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Image = global::stRevizor.Properties.Resources.imgClear;
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            resources.ApplyResources(this.toolStripMenuItem2, "toolStripMenuItem2");
            this.toolStripMenuItem2.Click += new System.EventHandler(this.toolStripMenuItem2_Click);
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage4.Controls.Add(this.labelVerion);
            this.tabPage4.Controls.Add(this.label11);
            this.tabPage4.Controls.Add(this.label10);
            this.tabPage4.Controls.Add(this.label9);
            this.tabPage4.Controls.Add(this.label8);
            this.tabPage4.Controls.Add(this.label7);
            this.tabPage4.Controls.Add(this.label6);
            this.tabPage4.Controls.Add(this.label5);
            this.tabPage4.Controls.Add(this.label4);
            this.tabPage4.Controls.Add(this.label3);
            this.tabPage4.Controls.Add(this.label2);
            this.tabPage4.Controls.Add(this.label1);
            this.tabPage4.Controls.Add(this.linkLabel2);
            this.tabPage4.Controls.Add(this.pictureBox1);
            this.tabPage4.Controls.Add(this.linkLabel1);
            resources.ApplyResources(this.tabPage4, "tabPage4");
            this.tabPage4.Name = "tabPage4";
            // 
            // labelVerion
            // 
            resources.ApplyResources(this.labelVerion, "labelVerion");
            this.labelVerion.ForeColor = System.Drawing.Color.MediumSeaGreen;
            this.labelVerion.Name = "labelVerion";
            // 
            // label11
            // 
            resources.ApplyResources(this.label11, "label11");
            this.label11.ForeColor = System.Drawing.Color.LimeGreen;
            this.label11.Name = "label11";
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.ForeColor = System.Drawing.Color.Gainsboro;
            this.label10.Name = "label10";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.ForeColor = System.Drawing.Color.Gainsboro;
            this.label9.Name = "label9";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.ForeColor = System.Drawing.Color.Gainsboro;
            this.label8.Name = "label8";
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.ForeColor = System.Drawing.Color.Gainsboro;
            this.label7.Name = "label7";
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.ForeColor = System.Drawing.Color.Gainsboro;
            this.label6.Name = "label6";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.ForeColor = System.Drawing.Color.Gainsboro;
            this.label5.Name = "label5";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.ForeColor = System.Drawing.Color.Gainsboro;
            this.label4.Name = "label4";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.ForeColor = System.Drawing.Color.Gainsboro;
            this.label3.Name = "label3";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.ForeColor = System.Drawing.Color.Gainsboro;
            this.label2.Name = "label2";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.ForeColor = System.Drawing.Color.Gainsboro;
            this.label1.Name = "label1";
            // 
            // linkLabel2
            // 
            resources.ApplyResources(this.linkLabel2, "linkLabel2");
            this.linkLabel2.ForeColor = System.Drawing.Color.Gainsboro;
            this.linkLabel2.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkLabel2.Name = "linkLabel2";
            this.linkLabel2.TabStop = true;
            this.linkLabel2.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkLabel2.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.@__LinkUrlClicked);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::stRevizor.Properties.Resources.firewall;
            resources.ApplyResources(this.pictureBox1, "pictureBox1");
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.TabStop = false;
            // 
            // linkLabel1
            // 
            resources.ApplyResources(this.linkLabel1, "linkLabel1");
            this.linkLabel1.ForeColor = System.Drawing.Color.Gainsboro;
            this.linkLabel1.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.TabStop = true;
            this.linkLabel1.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.@__LinkUrlClicked);
            // 
            // flatStatusBarInfo
            // 
            this.flatStatusBarInfo.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            resources.ApplyResources(this.flatStatusBarInfo, "flatStatusBarInfo");
            this.flatStatusBarInfo.ForeColor = System.Drawing.Color.White;
            this.flatStatusBarInfo.Name = "flatStatusBarInfo";
            this.flatStatusBarInfo.RectColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatStatusBarInfo.ShowTimeDate = false;
            this.flatStatusBarInfo.TextColor = System.Drawing.Color.White;
            // 
            // flatClose1
            // 
            resources.ApplyResources(this.flatClose1, "flatClose1");
            this.flatClose1.BackColor = System.Drawing.Color.White;
            this.flatClose1.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(168)))), ((int)(((byte)(35)))), ((int)(((byte)(35)))));
            this.flatClose1.Name = "flatClose1";
            this.flatClose1.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            // 
            // flatMinimize
            // 
            resources.ApplyResources(this.flatMinimize, "flatMinimize");
            this.flatMinimize.BackColor = System.Drawing.Color.White;
            this.flatMinimize.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.flatMinimize.Name = "flatMinimize";
            this.flatMinimize.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.flatMinimize.Click += new System.EventHandler(this.flatMinimize_Click);
            // 
            // timerSendServer
            // 
            this.timerSendServer.Interval = 600000;
            this.timerSendServer.Tick += new System.EventHandler(this.timerSendServer_Tick);
            // 
            // Manager
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.formSkinMain);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Manager";
            this.TransparencyKey = System.Drawing.Color.Fuchsia;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Manager_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatch)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EPMain)).EndInit();
            this.formSkinMain.ResumeLayout(false);
            this.flatGroupBox3.ResumeLayout(false);
            this.flatGroupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLoadWait)).EndInit();
            this.flatContextMenuSendServer.ResumeLayout(false);
            this.flatTabControlMain.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.flatGroupBoxServaerInfo.ResumeLayout(false);
            this.flatGroupBoxServaerInfo.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ServerInfoBindingSource)).EndInit();
            this.tabPage2.ResumeLayout(false);
            this.flatGroupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxFolder)).EndInit();
            this.flatGroupBox1.ResumeLayout(false);
            this.flatGroupBox1.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.flatContextMenuLog.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private stCoreUI.FormSkin formSkinMain;
        private stCoreUI.FlatClose flatClose1;
        private stCoreUI.FlatMini flatMinimize;
        private stCoreUI.FlatStatusBar flatStatusBarInfo;
        private stCoreUI.FlatTabControl flatTabControlMain;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage3;
        private stCoreUI.FlatGroupBox flatGroupBox1;
        private stCoreUI.FlatTextBox flatTextBoxServerFTPPass;
        private stCoreUI.FlatTextBox flatTextBoxServerFTPLogin;
        private stCoreUI.FlatTextBox flatTextBoxServer;
        private stCoreUI.FlatGroupBox flatGroupBox2;
        private stCoreUI.FlatTextBox flatTextBoxFolder;
        private stCoreUI.FlatRadioButton flatRadioButtonFTP;
        private stCoreUI.FlatRadioButton flatRadioButtonTFTP;
        private System.Windows.Forms.PictureBox pictureBoxFolder;
        private stCoreUI.FlatLabel flatLabel1;
        private stCoreUI.FlatLabel flatLabel2;
        private stCoreUI.FlatLabel flatLabel4;
        private stCoreUI.FlatLabel flatLabel3;
        private System.Windows.Forms.FolderBrowserDialog OFDMain;
        private System.Windows.Forms.TabPage tabPage4;
        private stCoreUI.FlatGroupBox flatGroupBoxServaerInfo;
        private stCoreUI.FlatAlertBox flatAlertBoxInfo;
        private stCoreUI.FlatButton flatButtonStartStop;
        private stCoreUI.FlatLabel flatLabel5;
        private stCoreUI.FlatLabel flatLabel6;
        private System.Windows.Forms.BindingSource ServerInfoBindingSource;
        private stCoreUI.FlatLabel flatLabel15;
        private stCoreUI.FlatLabel flatLabel16;
        private stCoreUI.FlatLabel flatLabel13;
        private stCoreUI.FlatLabel flatLabel14;
        private stCoreUI.FlatLabel flatLabel11;
        private stCoreUI.FlatLabel flatLabel12;
        private stCoreUI.FlatLabel flatLabel9;
        private stCoreUI.FlatLabel flatLabel10;
        private stCoreUI.FlatLabel flatLabel7;
        private stCoreUI.FlatLabel flatLabel8;
        private stCoreUI.FlatLabel flatLabel17;
        private stCoreUI.FlatLabel flatLabel18;
        private stCoreUI.FlatLabel flatLabel19;
        private stCoreUI.FlatButton flatButtonSave;
        private stCoreUI.FlatLabel flatLabel20;
        private stCoreUI.FlatLabel flatLabel22;
        private stCoreUI.FlatLabel flatLabel21;
        private stCoreUI.FlatLabel flatLabel26;
        private stCoreUI.FlatLabel flatLabel28;
        private stCoreUI.FlatLabel flatLabel24;
        private stCoreUI.FlatLabel flatLabel23;
        private stCoreUI.FlatLabel flatLabel30;
        private stCoreUI.FlatLabel flatLabel29;
        private stCoreUI.FlatLabel flatLabel27;
        private stCoreUI.FlatLabel flatLabel25;
        private stCoreUI.FlatLabel flatLabel31;
        private stCoreUI.FlatLabel flatLabel32;
        private stCoreUI.FlatButton flatButtonTimerDrop;
        private stCoreUI.FlatNumeric flatNumericTimerDrop;
        private stCoreUI.FlatLabel flatLabel33;
        private stCoreUI.FlatLabel flatLabel36;
        private stCoreUI.FlatLabel flatLabel37;
        private stCoreUI.FlatLabel flatLabel34;
        private stCoreUI.FlatLabel flatLabel35;
        private stCoreUI.FlatLabel flatLabel38;
        private stCoreUI.FlatLabel flatLabel39;
        private System.Windows.Forms.Timer timerDrop;
        private System.IO.FileSystemWatcher fileSystemWatch;
        private stCoreUI.FlatLabel flatLabel40;
        private stCoreUI.FlatLabel flatLabel41;
        private System.Windows.Forms.ErrorProvider EPMain;
        private stCoreUI.FlatLabel flatLabel42;
        private stCoreUI.FlatToggle flatToggle1;
        private System.Windows.Forms.ListBox ListBoxLog;
        private System.Windows.Forms.NotifyIcon NotifyMain;
        private stCoreUI.FlatContextMenuStrip flatContextMenuLog;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.SaveFileDialog SFDMain;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.LinkLabel linkLabel2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Timer timerSrvInfo;
        private stCoreUI.FlatButton flatButtonReload;
        private System.Windows.Forms.Label labelVerion;
        private stCoreUI.FlatGroupBox flatGroupBox3;
        private stCoreUI.FlatLabel flatLabel43;
        private stCoreUI.FlatLabel flatLabel63;
        private stCoreUI.FlatLabel flatLabel44;
        private stCoreUI.FlatLabel flatLabel45;
        private System.Windows.Forms.Timer timerSendServer;
        private System.Windows.Forms.PictureBox pictureBoxLoadWait;
        private stCoreUI.FlatContextMenuStrip flatContextMenuSendServer;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
    }
}

