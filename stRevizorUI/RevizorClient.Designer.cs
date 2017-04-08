namespace RevizorUI
{
    partial class RevizorClient
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RevizorClient));
            this.formSkinMain = new stCoreUI.FormSkin();
            this.flatMini1 = new stCoreUI.FlatMini();
            this.flatClose1 = new stCoreUI.FlatClose();
            this.flatTabControlMain = new stCoreUI.FlatTabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.FLBLImportTypeSet = new stCoreUI.FlatLabel();
            this.FLBLImportFileSet = new stCoreUI.FlatLabel();
            this.FLBLImportTypeTitle = new stCoreUI.FlatLabel();
            this.FLBLImportFileTitle = new stCoreUI.FlatLabel();
            this.FLBLUrlSet = new stCoreUI.FlatLabel();
            this.FLBLIpSet = new stCoreUI.FlatLabel();
            this.FLBLUrlTitle = new stCoreUI.FlatLabel();
            this.FLBLIpTitle = new stCoreUI.FlatLabel();
            this.FLBLDateSet = new stCoreUI.FlatLabel();
            this.PBInputClear = new System.Windows.Forms.PictureBox();
            this.FLBLDateTitle = new stCoreUI.FlatLabel();
            this.FABMain = new stCoreUI.FlatAlertBox();
            this.FBIpSave = new stCoreUI.FlatButton();
            this.FBUrlSave = new stCoreUI.FlatButton();
            this.FSBStart = new stCoreUI.FlatStickyButton();
            this.FTBInput = new stCoreUI.FlatTextBox();
            this.FBInput = new stCoreUI.FlatButton();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.FRBAgregate8 = new stCoreUI.FlatRadioButton();
            this.flatLabel10 = new stCoreUI.FlatLabel();
            this.flatLabel9 = new stCoreUI.FlatLabel();
            this.FRBAgregate16 = new stCoreUI.FlatRadioButton();
            this.FRBAgregate24 = new stCoreUI.FlatRadioButton();
            this.FNTftpRetryMaxCount = new stCoreUI.FlatNumeric();
            this.FRBAgregate0 = new stCoreUI.FlatRadioButton();
            this.FNTftpRetryMinutes = new stCoreUI.FlatNumeric();
            this.flatGroupBox1 = new stCoreUI.FlatGroupBox();
            this.flatLabel8 = new stCoreUI.FlatLabel();
            this.flatLabel7 = new stCoreUI.FlatLabel();
            this.flatLabel6 = new stCoreUI.FlatLabel();
            this.flatLabel5 = new stCoreUI.FlatLabel();
            this.FTDeleteSourceEnable = new stCoreUI.FlatToggle();
            this.FTSaveDefaultName = new stCoreUI.FlatToggle();
            this.FTUrlListEnable = new stCoreUI.FlatToggle();
            this.PBSetupFolderOutput = new System.Windows.Forms.PictureBox();
            this.PBSetupFolderInput = new System.Windows.Forms.PictureBox();
            this.FBSetupRestore = new stCoreUI.FlatButton();
            this.FBSetupSave = new stCoreUI.FlatButton();
            this.flatLabel4 = new stCoreUI.FlatLabel();
            this.FTBSetupIpTftp = new stCoreUI.FlatTextBox();
            this.flatLabel3 = new stCoreUI.FlatLabel();
            this.FTBSetupOutput = new stCoreUI.FlatTextBox();
            this.flatLabel2 = new stCoreUI.FlatLabel();
            this.FTBSetupInput = new stCoreUI.FlatTextBox();
            this.flatLabel1 = new stCoreUI.FlatLabel();
            this.FTBSetupLog = new stCoreUI.FlatTextBox();
            this.FPBMain = new stCoreUI.FlatProgressBar();
            this.BWMain = new System.ComponentModel.BackgroundWorker();
            this.OFDMain = new System.Windows.Forms.OpenFileDialog();
            this.SFDMain = new System.Windows.Forms.SaveFileDialog();
            this.EPMain = new System.Windows.Forms.ErrorProvider(this.components);
            this.BGWMain = new System.ComponentModel.BackgroundWorker();
            this.formSkinMain.SuspendLayout();
            this.flatTabControlMain.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PBInputClear)).BeginInit();
            this.tabPage2.SuspendLayout();
            this.flatGroupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PBSetupFolderOutput)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PBSetupFolderInput)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EPMain)).BeginInit();
            this.SuspendLayout();
            // 
            // formSkinMain
            // 
            this.formSkinMain.BackColor = System.Drawing.Color.White;
            this.formSkinMain.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.formSkinMain.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(53)))), ((int)(((byte)(58)))), ((int)(((byte)(60)))));
            this.formSkinMain.Controls.Add(this.flatMini1);
            this.formSkinMain.Controls.Add(this.flatClose1);
            this.formSkinMain.Controls.Add(this.flatTabControlMain);
            this.formSkinMain.Controls.Add(this.FPBMain);
            this.formSkinMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.formSkinMain.DoubleBufferAlways = false;
            this.formSkinMain.FlatColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.formSkinMain.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.formSkinMain.HeaderColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.formSkinMain.HeaderMaximize = false;
            this.formSkinMain.Location = new System.Drawing.Point(0, 0);
            this.formSkinMain.Name = "formSkinMain";
            this.formSkinMain.Size = new System.Drawing.Size(709, 412);
            this.formSkinMain.TabIndex = 0;
            this.formSkinMain.Text = "Реестр КОМНАДЗОР";
            this.formSkinMain.WinShadow = false;
            // 
            // flatMini1
            // 
            this.flatMini1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.flatMini1.BackColor = System.Drawing.Color.White;
            this.flatMini1.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.flatMini1.Font = new System.Drawing.Font("Marlett", 12F);
            this.flatMini1.Location = new System.Drawing.Point(655, 12);
            this.flatMini1.Name = "flatMini1";
            this.flatMini1.Size = new System.Drawing.Size(18, 18);
            this.flatMini1.TabIndex = 3;
            this.flatMini1.Text = "flatMini1";
            this.flatMini1.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            // 
            // flatClose1
            // 
            this.flatClose1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.flatClose1.BackColor = System.Drawing.Color.White;
            this.flatClose1.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(168)))), ((int)(((byte)(35)))), ((int)(((byte)(35)))));
            this.flatClose1.Font = new System.Drawing.Font("Marlett", 10F);
            this.flatClose1.Location = new System.Drawing.Point(679, 13);
            this.flatClose1.Name = "flatClose1";
            this.flatClose1.Size = new System.Drawing.Size(18, 18);
            this.flatClose1.TabIndex = 2;
            this.flatClose1.Text = "flatClose1";
            this.flatClose1.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            // 
            // flatTabControlMain
            // 
            this.flatTabControlMain.ActiveColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.flatTabControlMain.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.flatTabControlMain.Controls.Add(this.tabPage1);
            this.flatTabControlMain.Controls.Add(this.tabPage2);
            this.flatTabControlMain.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.flatTabControlMain.ItemSize = new System.Drawing.Size(120, 40);
            this.flatTabControlMain.Location = new System.Drawing.Point(4, 62);
            this.flatTabControlMain.Name = "flatTabControlMain";
            this.flatTabControlMain.SelectedIndex = 0;
            this.flatTabControlMain.Size = new System.Drawing.Size(702, 302);
            this.flatTabControlMain.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.flatTabControlMain.TabIndex = 1;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage1.Controls.Add(this.FLBLImportTypeSet);
            this.tabPage1.Controls.Add(this.FLBLImportFileSet);
            this.tabPage1.Controls.Add(this.FLBLImportTypeTitle);
            this.tabPage1.Controls.Add(this.FLBLImportFileTitle);
            this.tabPage1.Controls.Add(this.FLBLUrlSet);
            this.tabPage1.Controls.Add(this.FLBLIpSet);
            this.tabPage1.Controls.Add(this.FLBLUrlTitle);
            this.tabPage1.Controls.Add(this.FLBLIpTitle);
            this.tabPage1.Controls.Add(this.FLBLDateSet);
            this.tabPage1.Controls.Add(this.PBInputClear);
            this.tabPage1.Controls.Add(this.FLBLDateTitle);
            this.tabPage1.Controls.Add(this.FABMain);
            this.tabPage1.Controls.Add(this.FBIpSave);
            this.tabPage1.Controls.Add(this.FBUrlSave);
            this.tabPage1.Controls.Add(this.FSBStart);
            this.tabPage1.Controls.Add(this.FTBInput);
            this.tabPage1.Controls.Add(this.FBInput);
            this.tabPage1.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tabPage1.Location = new System.Drawing.Point(4, 44);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(694, 254);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Обновления";
            // 
            // FLBLImportTypeSet
            // 
            this.FLBLImportTypeSet.AutoSize = true;
            this.FLBLImportTypeSet.BackColor = System.Drawing.Color.Transparent;
            this.FLBLImportTypeSet.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLImportTypeSet.ForeColor = System.Drawing.Color.White;
            this.FLBLImportTypeSet.Location = new System.Drawing.Point(189, 134);
            this.FLBLImportTypeSet.Name = "FLBLImportTypeSet";
            this.FLBLImportTypeSet.Size = new System.Drawing.Size(13, 17);
            this.FLBLImportTypeSet.TabIndex = 16;
            this.FLBLImportTypeSet.Text = "-";
            // 
            // FLBLImportFileSet
            // 
            this.FLBLImportFileSet.AutoSize = true;
            this.FLBLImportFileSet.BackColor = System.Drawing.Color.Transparent;
            this.FLBLImportFileSet.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLImportFileSet.ForeColor = System.Drawing.Color.White;
            this.FLBLImportFileSet.Location = new System.Drawing.Point(189, 117);
            this.FLBLImportFileSet.Name = "FLBLImportFileSet";
            this.FLBLImportFileSet.Size = new System.Drawing.Size(13, 17);
            this.FLBLImportFileSet.TabIndex = 15;
            this.FLBLImportFileSet.Text = "-";
            // 
            // FLBLImportTypeTitle
            // 
            this.FLBLImportTypeTitle.AutoSize = true;
            this.FLBLImportTypeTitle.BackColor = System.Drawing.Color.Transparent;
            this.FLBLImportTypeTitle.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLImportTypeTitle.ForeColor = System.Drawing.Color.White;
            this.FLBLImportTypeTitle.Location = new System.Drawing.Point(17, 134);
            this.FLBLImportTypeTitle.Name = "FLBLImportTypeTitle";
            this.FLBLImportTypeTitle.Size = new System.Drawing.Size(115, 17);
            this.FLBLImportTypeTitle.TabIndex = 14;
            this.FLBLImportTypeTitle.Text = "Тип файла данных:";
            // 
            // FLBLImportFileTitle
            // 
            this.FLBLImportFileTitle.AutoSize = true;
            this.FLBLImportFileTitle.BackColor = System.Drawing.Color.Transparent;
            this.FLBLImportFileTitle.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLImportFileTitle.ForeColor = System.Drawing.Color.White;
            this.FLBLImportFileTitle.Location = new System.Drawing.Point(17, 116);
            this.FLBLImportFileTitle.Name = "FLBLImportFileTitle";
            this.FLBLImportFileTitle.Size = new System.Drawing.Size(137, 17);
            this.FLBLImportFileTitle.TabIndex = 13;
            this.FLBLImportFileTitle.Text = "Импорт файла данных:";
            // 
            // FLBLUrlSet
            // 
            this.FLBLUrlSet.AutoSize = true;
            this.FLBLUrlSet.BackColor = System.Drawing.Color.Transparent;
            this.FLBLUrlSet.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLUrlSet.ForeColor = System.Drawing.Color.White;
            this.FLBLUrlSet.Location = new System.Drawing.Point(189, 100);
            this.FLBLUrlSet.Name = "FLBLUrlSet";
            this.FLBLUrlSet.Size = new System.Drawing.Size(13, 17);
            this.FLBLUrlSet.TabIndex = 12;
            this.FLBLUrlSet.Text = "-";
            // 
            // FLBLIpSet
            // 
            this.FLBLIpSet.AutoSize = true;
            this.FLBLIpSet.BackColor = System.Drawing.Color.Transparent;
            this.FLBLIpSet.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLIpSet.ForeColor = System.Drawing.Color.White;
            this.FLBLIpSet.Location = new System.Drawing.Point(189, 83);
            this.FLBLIpSet.Name = "FLBLIpSet";
            this.FLBLIpSet.Size = new System.Drawing.Size(13, 17);
            this.FLBLIpSet.TabIndex = 11;
            this.FLBLIpSet.Text = "-";
            // 
            // FLBLUrlTitle
            // 
            this.FLBLUrlTitle.AutoSize = true;
            this.FLBLUrlTitle.BackColor = System.Drawing.Color.Transparent;
            this.FLBLUrlTitle.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLUrlTitle.ForeColor = System.Drawing.Color.White;
            this.FLBLUrlTitle.Location = new System.Drawing.Point(17, 100);
            this.FLBLUrlTitle.Name = "FLBLUrlTitle";
            this.FLBLUrlTitle.Size = new System.Drawing.Size(155, 17);
            this.FLBLUrlTitle.TabIndex = 10;
            this.FLBLUrlTitle.Text = "Обработано URL адресов:";
            // 
            // FLBLIpTitle
            // 
            this.FLBLIpTitle.AutoSize = true;
            this.FLBLIpTitle.BackColor = System.Drawing.Color.Transparent;
            this.FLBLIpTitle.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLIpTitle.ForeColor = System.Drawing.Color.White;
            this.FLBLIpTitle.Location = new System.Drawing.Point(17, 83);
            this.FLBLIpTitle.Name = "FLBLIpTitle";
            this.FLBLIpTitle.Size = new System.Drawing.Size(144, 17);
            this.FLBLIpTitle.TabIndex = 9;
            this.FLBLIpTitle.Text = "Обработано IP адресов:";
            // 
            // FLBLDateSet
            // 
            this.FLBLDateSet.AutoSize = true;
            this.FLBLDateSet.BackColor = System.Drawing.Color.Transparent;
            this.FLBLDateSet.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLDateSet.ForeColor = System.Drawing.Color.White;
            this.FLBLDateSet.Location = new System.Drawing.Point(189, 66);
            this.FLBLDateSet.Name = "FLBLDateSet";
            this.FLBLDateSet.Size = new System.Drawing.Size(13, 17);
            this.FLBLDateSet.TabIndex = 8;
            this.FLBLDateSet.Text = "-";
            // 
            // PBInputClear
            // 
            this.PBInputClear.Cursor = System.Windows.Forms.Cursors.Hand;
            this.PBInputClear.Image = global::RevizorUI.Properties.Resources.imgClear;
            this.PBInputClear.Location = new System.Drawing.Point(4, 19);
            this.PBInputClear.Name = "PBInputClear";
            this.PBInputClear.Size = new System.Drawing.Size(29, 29);
            this.PBInputClear.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.PBInputClear.TabIndex = 7;
            this.PBInputClear.TabStop = false;
            this.PBInputClear.Click += new System.EventHandler(this.PBInputClear_Click);
            // 
            // FLBLDateTitle
            // 
            this.FLBLDateTitle.AutoSize = true;
            this.FLBLDateTitle.BackColor = System.Drawing.Color.Transparent;
            this.FLBLDateTitle.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FLBLDateTitle.ForeColor = System.Drawing.Color.White;
            this.FLBLDateTitle.Location = new System.Drawing.Point(17, 66);
            this.FLBLDateTitle.Name = "FLBLDateTitle";
            this.FLBLDateTitle.Size = new System.Drawing.Size(118, 17);
            this.FLBLDateTitle.TabIndex = 6;
            this.FLBLDateTitle.Text = "Время обновления:";
            // 
            // FABMain
            // 
            this.FABMain.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FABMain.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FABMain.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FABMain.kind = stCoreUI.FlatAlertBox._Kind.Success;
            this.FABMain.Location = new System.Drawing.Point(6, 205);
            this.FABMain.Name = "FABMain";
            this.FABMain.Size = new System.Drawing.Size(496, 42);
            this.FABMain.TabIndex = 5;
            this.FABMain.Visible = false;
            // 
            // FBIpSave
            // 
            this.FBIpSave.BackColor = System.Drawing.Color.Transparent;
            this.FBIpSave.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FBIpSave.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FBIpSave.Enabled = false;
            this.FBIpSave.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.FBIpSave.Location = new System.Drawing.Point(508, 181);
            this.FBIpSave.Name = "FBIpSave";
            this.FBIpSave.Rounded = false;
            this.FBIpSave.Size = new System.Drawing.Size(180, 32);
            this.FBIpSave.TabIndex = 4;
            this.FBIpSave.Text = "Сохранить IP лист";
            this.FBIpSave.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FBIpSave.Click += new System.EventHandler(this.FBIpSave_Click);
            // 
            // FBUrlSave
            // 
            this.FBUrlSave.BackColor = System.Drawing.Color.Transparent;
            this.FBUrlSave.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FBUrlSave.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FBUrlSave.Enabled = false;
            this.FBUrlSave.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.FBUrlSave.Location = new System.Drawing.Point(508, 216);
            this.FBUrlSave.Name = "FBUrlSave";
            this.FBUrlSave.Rounded = false;
            this.FBUrlSave.Size = new System.Drawing.Size(180, 32);
            this.FBUrlSave.TabIndex = 3;
            this.FBUrlSave.Text = "Сохранить Url лист";
            this.FBUrlSave.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FBUrlSave.Click += new System.EventHandler(this.FBUrlSave_Click);
            // 
            // FSBStart
            // 
            this.FSBStart.BackColor = System.Drawing.Color.Transparent;
            this.FSBStart.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FSBStart.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FSBStart.Enabled = false;
            this.FSBStart.Font = new System.Drawing.Font("Segoe UI", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.FSBStart.Location = new System.Drawing.Point(508, 54);
            this.FSBStart.Name = "FSBStart";
            this.FSBStart.Rounded = false;
            this.FSBStart.Size = new System.Drawing.Size(180, 121);
            this.FSBStart.TabIndex = 2;
            this.FSBStart.Text = "Старт!";
            this.FSBStart.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FSBStart.Click += new System.EventHandler(this.FSBStart_Click);
            // 
            // FTBInput
            // 
            this.FTBInput.BackColor = System.Drawing.Color.Transparent;
            this.FTBInput.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::RevizorUI.Properties.Settings.Default, "setInputFile", true));
            this.FTBInput.FocusOnHover = false;
            this.EPMain.SetIconAlignment(this.FTBInput, System.Windows.Forms.ErrorIconAlignment.MiddleLeft);
            this.FTBInput.Location = new System.Drawing.Point(33, 19);
            this.FTBInput.MaxLength = 32767;
            this.FTBInput.Multiline = false;
            this.FTBInput.Name = "FTBInput";
            this.FTBInput.ReadOnly = true;
            this.FTBInput.SelectedText = "";
            this.FTBInput.SelectionLength = 0;
            this.FTBInput.Size = new System.Drawing.Size(469, 29);
            this.FTBInput.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("FTBInput.SpellMark")));
            this.FTBInput.TabIndex = 1;
            this.FTBInput.TabStop = false;
            this.FTBInput.Text = global::RevizorUI.Properties.Settings.Default.setInputFile;
            this.FTBInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.FTBInput.TextCaption = "источник реестра КомНадзора";
            this.FTBInput.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.FTBInput.UseSystemPasswordChar = false;
            // 
            // FBInput
            // 
            this.FBInput.BackColor = System.Drawing.Color.Transparent;
            this.FBInput.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FBInput.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FBInput.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.FBInput.Location = new System.Drawing.Point(508, 16);
            this.FBInput.Name = "FBInput";
            this.FBInput.Rounded = false;
            this.FBInput.Size = new System.Drawing.Size(180, 32);
            this.FBInput.TabIndex = 0;
            this.FBInput.Text = "Загрузить Реестр";
            this.FBInput.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FBInput.Click += new System.EventHandler(this.FBInput_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.tabPage2.Controls.Add(this.FRBAgregate8);
            this.tabPage2.Controls.Add(this.flatLabel10);
            this.tabPage2.Controls.Add(this.flatLabel9);
            this.tabPage2.Controls.Add(this.FRBAgregate16);
            this.tabPage2.Controls.Add(this.FRBAgregate24);
            this.tabPage2.Controls.Add(this.FNTftpRetryMaxCount);
            this.tabPage2.Controls.Add(this.FRBAgregate0);
            this.tabPage2.Controls.Add(this.FNTftpRetryMinutes);
            this.tabPage2.Controls.Add(this.flatGroupBox1);
            this.tabPage2.Controls.Add(this.flatLabel7);
            this.tabPage2.Controls.Add(this.flatLabel6);
            this.tabPage2.Controls.Add(this.flatLabel5);
            this.tabPage2.Controls.Add(this.FTDeleteSourceEnable);
            this.tabPage2.Controls.Add(this.FTSaveDefaultName);
            this.tabPage2.Controls.Add(this.FTUrlListEnable);
            this.tabPage2.Controls.Add(this.PBSetupFolderOutput);
            this.tabPage2.Controls.Add(this.PBSetupFolderInput);
            this.tabPage2.Controls.Add(this.FBSetupRestore);
            this.tabPage2.Controls.Add(this.FBSetupSave);
            this.tabPage2.Controls.Add(this.flatLabel4);
            this.tabPage2.Controls.Add(this.FTBSetupIpTftp);
            this.tabPage2.Controls.Add(this.flatLabel3);
            this.tabPage2.Controls.Add(this.FTBSetupOutput);
            this.tabPage2.Controls.Add(this.flatLabel2);
            this.tabPage2.Controls.Add(this.FTBSetupInput);
            this.tabPage2.Controls.Add(this.flatLabel1);
            this.tabPage2.Controls.Add(this.FTBSetupLog);
            this.tabPage2.Location = new System.Drawing.Point(4, 44);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(694, 254);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Настройки";
            // 
            // FRBAgregate8
            // 
            this.FRBAgregate8.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FRBAgregate8.Checked = global::RevizorUI.Properties.Settings.Default.setAggregateNet8;
            this.FRBAgregate8.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FRBAgregate8.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setAggregateNet8", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FRBAgregate8.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FRBAgregate8.Location = new System.Drawing.Point(241, 184);
            this.FRBAgregate8.Name = "FRBAgregate8";
            this.FRBAgregate8.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.FRBAgregate8.Size = new System.Drawing.Size(76, 22);
            this.FRBAgregate8.TabIndex = 35;
            this.FRBAgregate8.Tag = "16";
            this.FRBAgregate8.Text = "сеть/8";
            // 
            // flatLabel10
            // 
            this.flatLabel10.AutoEllipsis = true;
            this.flatLabel10.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel10.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel10.ForeColor = System.Drawing.Color.White;
            this.flatLabel10.Location = new System.Drawing.Point(495, 176);
            this.flatLabel10.Name = "flatLabel10";
            this.flatLabel10.Size = new System.Drawing.Size(187, 41);
            this.flatLabel10.TabIndex = 39;
            this.flatLabel10.Text = "колчество попыток повторной отправки данных";
            // 
            // flatLabel9
            // 
            this.flatLabel9.AutoEllipsis = true;
            this.flatLabel9.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel9.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel9.ForeColor = System.Drawing.Color.White;
            this.flatLabel9.Location = new System.Drawing.Point(495, 140);
            this.flatLabel9.Name = "flatLabel9";
            this.flatLabel9.Size = new System.Drawing.Size(187, 41);
            this.flatLabel9.TabIndex = 38;
            this.flatLabel9.Text = "время ожидания перед повторной отправкой (минут)";
            // 
            // FRBAgregate16
            // 
            this.FRBAgregate16.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FRBAgregate16.Checked = global::RevizorUI.Properties.Settings.Default.setAggregateNet16;
            this.FRBAgregate16.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FRBAgregate16.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setAggregateNet16", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FRBAgregate16.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FRBAgregate16.Location = new System.Drawing.Point(159, 184);
            this.FRBAgregate16.Name = "FRBAgregate16";
            this.FRBAgregate16.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.FRBAgregate16.Size = new System.Drawing.Size(76, 22);
            this.FRBAgregate16.TabIndex = 34;
            this.FRBAgregate16.Tag = "16";
            this.FRBAgregate16.Text = "сеть/16";
            // 
            // FRBAgregate24
            // 
            this.FRBAgregate24.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FRBAgregate24.Checked = global::RevizorUI.Properties.Settings.Default.setAggregateNet24;
            this.FRBAgregate24.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FRBAgregate24.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setAggregateNet24", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FRBAgregate24.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FRBAgregate24.Location = new System.Drawing.Point(79, 184);
            this.FRBAgregate24.Name = "FRBAgregate24";
            this.FRBAgregate24.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.FRBAgregate24.Size = new System.Drawing.Size(74, 22);
            this.FRBAgregate24.TabIndex = 33;
            this.FRBAgregate24.Tag = "24";
            this.FRBAgregate24.Text = "сеть/24";
            // 
            // FNTftpRetryMaxCount
            // 
            this.FNTftpRetryMaxCount.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FNTftpRetryMaxCount.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.FNTftpRetryMaxCount.ButtonColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FNTftpRetryMaxCount.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::RevizorUI.Properties.Settings.Default, "setTftpRetryMaxCount", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FNTftpRetryMaxCount.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FNTftpRetryMaxCount.ForeColor = System.Drawing.Color.White;
            this.FNTftpRetryMaxCount.Location = new System.Drawing.Point(413, 176);
            this.FNTftpRetryMaxCount.Maximum = ((long)(1000));
            this.FNTftpRetryMaxCount.Minimum = ((long)(5));
            this.FNTftpRetryMaxCount.Name = "FNTftpRetryMaxCount";
            this.FNTftpRetryMaxCount.Size = new System.Drawing.Size(76, 30);
            this.FNTftpRetryMaxCount.TabIndex = 37;
            this.FNTftpRetryMaxCount.Value = global::RevizorUI.Properties.Settings.Default.setTftpRetryMaxCount;
            // 
            // FRBAgregate0
            // 
            this.FRBAgregate0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FRBAgregate0.Checked = global::RevizorUI.Properties.Settings.Default.setAggregateNet0;
            this.FRBAgregate0.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FRBAgregate0.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setAggregateNet0", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FRBAgregate0.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FRBAgregate0.Location = new System.Drawing.Point(16, 184);
            this.FRBAgregate0.Name = "FRBAgregate0";
            this.FRBAgregate0.Options = stCoreUI.FlatRadioButton._Options.Style1;
            this.FRBAgregate0.Size = new System.Drawing.Size(57, 22);
            this.FRBAgregate0.TabIndex = 32;
            this.FRBAgregate0.Tag = "0";
            this.FRBAgregate0.Text = "нет";
            // 
            // FNTftpRetryMinutes
            // 
            this.FNTftpRetryMinutes.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FNTftpRetryMinutes.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(47)))), ((int)(((byte)(49)))));
            this.FNTftpRetryMinutes.ButtonColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FNTftpRetryMinutes.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::RevizorUI.Properties.Settings.Default, "setTftpRetryMinutes", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FNTftpRetryMinutes.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FNTftpRetryMinutes.ForeColor = System.Drawing.Color.White;
            this.FNTftpRetryMinutes.Location = new System.Drawing.Point(413, 140);
            this.FNTftpRetryMinutes.Maximum = ((long)(60));
            this.FNTftpRetryMinutes.Minimum = ((long)(1));
            this.FNTftpRetryMinutes.Name = "FNTftpRetryMinutes";
            this.FNTftpRetryMinutes.Size = new System.Drawing.Size(76, 30);
            this.FNTftpRetryMinutes.TabIndex = 36;
            this.FNTftpRetryMinutes.Value = global::RevizorUI.Properties.Settings.Default.setTftpRetryMinutes;
            // 
            // flatGroupBox1
            // 
            this.flatGroupBox1.BackColor = System.Drawing.Color.Transparent;
            this.flatGroupBox1.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.flatGroupBox1.Controls.Add(this.flatLabel8);
            this.flatGroupBox1.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.flatGroupBox1.Location = new System.Drawing.Point(14, 160);
            this.flatGroupBox1.Name = "flatGroupBox1";
            this.flatGroupBox1.ShowText = true;
            this.flatGroupBox1.Size = new System.Drawing.Size(393, 21);
            this.flatGroupBox1.TabIndex = 35;
            // 
            // flatLabel8
            // 
            this.flatLabel8.AutoEllipsis = true;
            this.flatLabel8.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel8.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel8.ForeColor = System.Drawing.Color.White;
            this.flatLabel8.Location = new System.Drawing.Point(-3, 0);
            this.flatLabel8.Name = "flatLabel8";
            this.flatLabel8.Size = new System.Drawing.Size(379, 52);
            this.flatLabel8.TabIndex = 30;
            this.flatLabel8.Text = "агрегировать IP адреса в сети, привести к маске:";
            // 
            // flatLabel7
            // 
            this.flatLabel7.AutoEllipsis = true;
            this.flatLabel7.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel7.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel7.ForeColor = System.Drawing.Color.White;
            this.flatLabel7.Location = new System.Drawing.Point(495, 101);
            this.flatLabel7.Name = "flatLabel7";
            this.flatLabel7.Size = new System.Drawing.Size(187, 41);
            this.flatLabel7.TabIndex = 28;
            this.flatLabel7.Text = "удалять файл источника реестра";
            // 
            // flatLabel6
            // 
            this.flatLabel6.AutoEllipsis = true;
            this.flatLabel6.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel6.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel6.ForeColor = System.Drawing.Color.White;
            this.flatLabel6.Location = new System.Drawing.Point(495, 60);
            this.flatLabel6.Name = "flatLabel6";
            this.flatLabel6.Size = new System.Drawing.Size(187, 35);
            this.flatLabel6.TabIndex = 27;
            this.flatLabel6.Text = "сохранять имя файла по умолчанию";
            // 
            // flatLabel5
            // 
            this.flatLabel5.AutoEllipsis = true;
            this.flatLabel5.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel5.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel5.ForeColor = System.Drawing.Color.White;
            this.flatLabel5.Location = new System.Drawing.Point(495, 20);
            this.flatLabel5.Name = "flatLabel5";
            this.flatLabel5.Size = new System.Drawing.Size(187, 35);
            this.flatLabel5.TabIndex = 26;
            this.flatLabel5.Text = "использовать Layer7 для URL, -поддержка iptables";
            // 
            // FTDeleteSourceEnable
            // 
            this.FTDeleteSourceEnable.BackColor = System.Drawing.Color.Transparent;
            this.FTDeleteSourceEnable.Checked = global::RevizorUI.Properties.Settings.Default.setDeleteSourceEnable;
            this.FTDeleteSourceEnable.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FTDeleteSourceEnable.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setDeleteSourceEnable", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTDeleteSourceEnable.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FTDeleteSourceEnable.Location = new System.Drawing.Point(413, 101);
            this.FTDeleteSourceEnable.Name = "FTDeleteSourceEnable";
            this.FTDeleteSourceEnable.Options = stCoreUI.FlatToggle._Options.Style3;
            this.FTDeleteSourceEnable.Size = new System.Drawing.Size(76, 33);
            this.FTDeleteSourceEnable.TabIndex = 25;
            // 
            // FTSaveDefaultName
            // 
            this.FTSaveDefaultName.BackColor = System.Drawing.Color.Transparent;
            this.FTSaveDefaultName.Checked = global::RevizorUI.Properties.Settings.Default.setSaveDefaultName;
            this.FTSaveDefaultName.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FTSaveDefaultName.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setSaveDefaultName", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTSaveDefaultName.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FTSaveDefaultName.Location = new System.Drawing.Point(413, 60);
            this.FTSaveDefaultName.Name = "FTSaveDefaultName";
            this.FTSaveDefaultName.Options = stCoreUI.FlatToggle._Options.Style3;
            this.FTSaveDefaultName.Size = new System.Drawing.Size(76, 33);
            this.FTSaveDefaultName.TabIndex = 24;
            // 
            // FTUrlListEnable
            // 
            this.FTUrlListEnable.BackColor = System.Drawing.Color.Transparent;
            this.FTUrlListEnable.Checked = global::RevizorUI.Properties.Settings.Default.setUrlListEnable;
            this.FTUrlListEnable.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FTUrlListEnable.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::RevizorUI.Properties.Settings.Default, "setUrlListEnable", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTUrlListEnable.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.FTUrlListEnable.Location = new System.Drawing.Point(413, 20);
            this.FTUrlListEnable.Name = "FTUrlListEnable";
            this.FTUrlListEnable.Options = stCoreUI.FlatToggle._Options.Style3;
            this.FTUrlListEnable.Size = new System.Drawing.Size(76, 33);
            this.FTUrlListEnable.TabIndex = 23;
            // 
            // PBSetupFolderOutput
            // 
            this.PBSetupFolderOutput.Cursor = System.Windows.Forms.Cursors.Hand;
            this.PBSetupFolderOutput.Image = global::RevizorUI.Properties.Resources.imgFolderOpen;
            this.PBSetupFolderOutput.Location = new System.Drawing.Point(366, 125);
            this.PBSetupFolderOutput.Name = "PBSetupFolderOutput";
            this.PBSetupFolderOutput.Size = new System.Drawing.Size(29, 29);
            this.PBSetupFolderOutput.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.PBSetupFolderOutput.TabIndex = 21;
            this.PBSetupFolderOutput.TabStop = false;
            this.PBSetupFolderOutput.Click += new System.EventHandler(this.PBSetupFolderOutput_Click);
            // 
            // PBSetupFolderInput
            // 
            this.PBSetupFolderInput.Cursor = System.Windows.Forms.Cursors.Hand;
            this.PBSetupFolderInput.Image = global::RevizorUI.Properties.Resources.imgFolderOpen;
            this.PBSetupFolderInput.Location = new System.Drawing.Point(366, 90);
            this.PBSetupFolderInput.Name = "PBSetupFolderInput";
            this.PBSetupFolderInput.Size = new System.Drawing.Size(29, 29);
            this.PBSetupFolderInput.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.PBSetupFolderInput.TabIndex = 20;
            this.PBSetupFolderInput.TabStop = false;
            this.PBSetupFolderInput.Click += new System.EventHandler(this.PBSetupFolderInput_Click);
            // 
            // FBSetupRestore
            // 
            this.FBSetupRestore.BackColor = System.Drawing.Color.Transparent;
            this.FBSetupRestore.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FBSetupRestore.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FBSetupRestore.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.FBSetupRestore.Location = new System.Drawing.Point(223, 216);
            this.FBSetupRestore.Name = "FBSetupRestore";
            this.FBSetupRestore.Rounded = false;
            this.FBSetupRestore.Size = new System.Drawing.Size(266, 32);
            this.FBSetupRestore.TabIndex = 19;
            this.FBSetupRestore.Text = "Перечитать настройки";
            this.FBSetupRestore.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FBSetupRestore.Click += new System.EventHandler(this.FBSetupRestore_Click);
            // 
            // FBSetupSave
            // 
            this.FBSetupSave.BackColor = System.Drawing.Color.Transparent;
            this.FBSetupSave.BaseColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FBSetupSave.Cursor = System.Windows.Forms.Cursors.Hand;
            this.FBSetupSave.Font = new System.Drawing.Font("Segoe UI", 12F);
            this.FBSetupSave.Location = new System.Drawing.Point(498, 216);
            this.FBSetupSave.Name = "FBSetupSave";
            this.FBSetupSave.Rounded = false;
            this.FBSetupSave.Size = new System.Drawing.Size(192, 32);
            this.FBSetupSave.TabIndex = 18;
            this.FBSetupSave.Text = "Сохранить настройки";
            this.FBSetupSave.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(243)))), ((int)(((byte)(243)))), ((int)(((byte)(243)))));
            this.FBSetupSave.Click += new System.EventHandler(this.FBSetupSave_Click);
            // 
            // flatLabel4
            // 
            this.flatLabel4.AutoSize = true;
            this.flatLabel4.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel4.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel4.ForeColor = System.Drawing.Color.White;
            this.flatLabel4.Location = new System.Drawing.Point(11, 20);
            this.flatLabel4.Name = "flatLabel4";
            this.flatLabel4.Size = new System.Drawing.Size(168, 17);
            this.flatLabel4.TabIndex = 17;
            this.flatLabel4.Text = "IP/DNS адрес TFTP сервера:";
            // 
            // FTBSetupIpTftp
            // 
            this.FTBSetupIpTftp.BackColor = System.Drawing.Color.Transparent;
            this.FTBSetupIpTftp.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::RevizorUI.Properties.Settings.Default, "setIpTftpServer", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTBSetupIpTftp.FocusOnHover = false;
            this.EPMain.SetIconPadding(this.FTBSetupIpTftp, 4);
            this.FTBSetupIpTftp.Location = new System.Drawing.Point(188, 20);
            this.FTBSetupIpTftp.MaxLength = 32767;
            this.FTBSetupIpTftp.Multiline = false;
            this.FTBSetupIpTftp.Name = "FTBSetupIpTftp";
            this.FTBSetupIpTftp.ReadOnly = false;
            this.FTBSetupIpTftp.SelectedText = "";
            this.FTBSetupIpTftp.SelectionLength = 0;
            this.FTBSetupIpTftp.Size = new System.Drawing.Size(202, 29);
            this.FTBSetupIpTftp.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("FTBSetupIpTftp.SpellMark")));
            this.FTBSetupIpTftp.TabIndex = 16;
            this.FTBSetupIpTftp.Text = global::RevizorUI.Properties.Settings.Default.setIpTftpServer;
            this.FTBSetupIpTftp.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.FTBSetupIpTftp.TextCaption = null;
            this.FTBSetupIpTftp.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.FTBSetupIpTftp.UseSystemPasswordChar = false;
            // 
            // flatLabel3
            // 
            this.flatLabel3.AutoSize = true;
            this.flatLabel3.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel3.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel3.ForeColor = System.Drawing.Color.White;
            this.flatLabel3.Location = new System.Drawing.Point(11, 125);
            this.flatLabel3.Name = "flatLabel3";
            this.flatLabel3.Size = new System.Drawing.Size(171, 17);
            this.flatLabel3.TabIndex = 15;
            this.flatLabel3.Text = "путь к сохраняемым файлам:";
            // 
            // FTBSetupOutput
            // 
            this.FTBSetupOutput.BackColor = System.Drawing.Color.Transparent;
            this.FTBSetupOutput.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::RevizorUI.Properties.Settings.Default, "setSaveFile", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTBSetupOutput.FocusOnHover = false;
            this.FTBSetupOutput.Location = new System.Drawing.Point(188, 125);
            this.FTBSetupOutput.MaxLength = 32767;
            this.FTBSetupOutput.Multiline = false;
            this.FTBSetupOutput.Name = "FTBSetupOutput";
            this.FTBSetupOutput.ReadOnly = true;
            this.FTBSetupOutput.SelectedText = "";
            this.FTBSetupOutput.SelectionLength = 0;
            this.FTBSetupOutput.Size = new System.Drawing.Size(176, 29);
            this.FTBSetupOutput.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("FTBSetupOutput.SpellMark")));
            this.FTBSetupOutput.TabIndex = 14;
            this.FTBSetupOutput.Text = global::RevizorUI.Properties.Settings.Default.setSaveFile;
            this.FTBSetupOutput.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.FTBSetupOutput.TextCaption = null;
            this.FTBSetupOutput.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.FTBSetupOutput.UseSystemPasswordChar = false;
            // 
            // flatLabel2
            // 
            this.flatLabel2.AutoSize = true;
            this.flatLabel2.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel2.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel2.ForeColor = System.Drawing.Color.White;
            this.flatLabel2.Location = new System.Drawing.Point(11, 90);
            this.flatLabel2.Name = "flatLabel2";
            this.flatLabel2.Size = new System.Drawing.Size(132, 17);
            this.flatLabel2.TabIndex = 13;
            this.flatLabel2.Text = "путь к файлу реестра:";
            // 
            // FTBSetupInput
            // 
            this.FTBSetupInput.BackColor = System.Drawing.Color.Transparent;
            this.FTBSetupInput.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::RevizorUI.Properties.Settings.Default, "setInputFile", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTBSetupInput.FocusOnHover = false;
            this.FTBSetupInput.Location = new System.Drawing.Point(188, 90);
            this.FTBSetupInput.MaxLength = 32767;
            this.FTBSetupInput.Multiline = false;
            this.FTBSetupInput.Name = "FTBSetupInput";
            this.FTBSetupInput.ReadOnly = true;
            this.FTBSetupInput.SelectedText = "";
            this.FTBSetupInput.SelectionLength = 0;
            this.FTBSetupInput.Size = new System.Drawing.Size(176, 29);
            this.FTBSetupInput.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("FTBSetupInput.SpellMark")));
            this.FTBSetupInput.TabIndex = 12;
            this.FTBSetupInput.Text = global::RevizorUI.Properties.Settings.Default.setInputFile;
            this.FTBSetupInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.FTBSetupInput.TextCaption = null;
            this.FTBSetupInput.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.FTBSetupInput.UseSystemPasswordChar = false;
            // 
            // flatLabel1
            // 
            this.flatLabel1.AutoSize = true;
            this.flatLabel1.BackColor = System.Drawing.Color.Transparent;
            this.flatLabel1.Font = new System.Drawing.Font("Segoe UI Light", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.flatLabel1.ForeColor = System.Drawing.Color.White;
            this.flatLabel1.Location = new System.Drawing.Point(11, 55);
            this.flatLabel1.Name = "flatLabel1";
            this.flatLabel1.Size = new System.Drawing.Size(122, 17);
            this.flatLabel1.TabIndex = 11;
            this.flatLabel1.Text = "имя файла журнала:";
            // 
            // FTBSetupLog
            // 
            this.FTBSetupLog.BackColor = System.Drawing.Color.Transparent;
            this.FTBSetupLog.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::RevizorUI.Properties.Settings.Default, "setLogFile", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.FTBSetupLog.FocusOnHover = false;
            this.EPMain.SetIconPadding(this.FTBSetupLog, 4);
            this.FTBSetupLog.Location = new System.Drawing.Point(188, 55);
            this.FTBSetupLog.MaxLength = 32767;
            this.FTBSetupLog.Multiline = false;
            this.FTBSetupLog.Name = "FTBSetupLog";
            this.FTBSetupLog.ReadOnly = false;
            this.FTBSetupLog.SelectedText = "";
            this.FTBSetupLog.SelectionLength = 0;
            this.FTBSetupLog.Size = new System.Drawing.Size(202, 29);
            this.FTBSetupLog.SpellMark = ((System.Collections.Generic.List<stCore.TxtPosition>)(resources.GetObject("FTBSetupLog.SpellMark")));
            this.FTBSetupLog.TabIndex = 0;
            this.FTBSetupLog.Text = global::RevizorUI.Properties.Settings.Default.setLogFile;
            this.FTBSetupLog.TextAlign = System.Windows.Forms.HorizontalAlignment.Left;
            this.FTBSetupLog.TextCaption = null;
            this.FTBSetupLog.TextColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.FTBSetupLog.UseSystemPasswordChar = false;
            // 
            // FPBMain
            // 
            this.FPBMain.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(70)))), ((int)(((byte)(73)))));
            this.FPBMain.DarkerProgress = System.Drawing.Color.FromArgb(((int)(((byte)(23)))), ((int)(((byte)(148)))), ((int)(((byte)(92)))));
            this.FPBMain.Location = new System.Drawing.Point(0, 370);
            this.FPBMain.Maximum = 100;
            this.FPBMain.Name = "FPBMain";
            this.FPBMain.Pattern = true;
            this.FPBMain.PercentSign = false;
            this.FPBMain.ProgressColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(169)))), ((int)(((byte)(82)))));
            this.FPBMain.ShowBalloon = true;
            this.FPBMain.Size = new System.Drawing.Size(706, 42);
            this.FPBMain.TabIndex = 0;
            this.FPBMain.Value = 0;
            // 
            // BWMain
            // 
            this.BWMain.WorkerReportsProgress = true;
            this.BWMain.WorkerSupportsCancellation = true;
            // 
            // OFDMain
            // 
            this.OFDMain.Filter = "Архив Реестра (zip)|register.zip|Архив Реестра (xml)|dump.xml|Список IP/URL (fw)|" +
    "*.fw|Данные ZIP|*.zip|Данные XML|*.xml";
            this.OFDMain.RestoreDirectory = true;
            this.OFDMain.ShowReadOnly = true;
            // 
            // SFDMain
            // 
            this.SFDMain.DefaultExt = "fw";
            this.SFDMain.Filter = "Список(fw)|*.fw|Список (txt)|*.txt";
            this.SFDMain.RestoreDirectory = true;
            this.SFDMain.SupportMultiDottedExtensions = true;
            // 
            // EPMain
            // 
            this.EPMain.ContainerControl = this;
            // 
            // BGWMain
            // 
            this.BGWMain.WorkerReportsProgress = true;
            this.BGWMain.WorkerSupportsCancellation = true;
            this.BGWMain.DoWork += new System.ComponentModel.DoWorkEventHandler(this.BGWMain_DoWork);
            this.BGWMain.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.BGWMain_ProgressChanged);
            this.BGWMain.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.BGWMain_RunWorkerCompleted);
            // 
            // RevizorClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(709, 412);
            this.Controls.Add(this.formSkinMain);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "RevizorClient";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.TransparencyKey = System.Drawing.Color.Fuchsia;
            this.formSkinMain.ResumeLayout(false);
            this.flatTabControlMain.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PBInputClear)).EndInit();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.flatGroupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PBSetupFolderOutput)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PBSetupFolderInput)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EPMain)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private stCoreUI.FormSkin formSkinMain;
        private stCoreUI.FlatProgressBar FPBMain;
        private stCoreUI.FlatTabControl flatTabControlMain;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private stCoreUI.FlatMini flatMini1;
        private stCoreUI.FlatClose flatClose1;
        private stCoreUI.FlatTextBox FTBInput;
        private stCoreUI.FlatButton FBInput;
        private stCoreUI.FlatStickyButton FSBStart;
        private stCoreUI.FlatButton FBIpSave;
        private stCoreUI.FlatButton FBUrlSave;
        private System.ComponentModel.BackgroundWorker BWMain;
        private stCoreUI.FlatAlertBox FABMain;
        private System.Windows.Forms.OpenFileDialog OFDMain;
        private System.Windows.Forms.SaveFileDialog SFDMain;
        private System.Windows.Forms.ErrorProvider EPMain;
        private System.ComponentModel.BackgroundWorker BGWMain;
        private stCoreUI.FlatLabel FLBLDateTitle;
        private System.Windows.Forms.PictureBox PBInputClear;
        private stCoreUI.FlatLabel FLBLDateSet;
        private stCoreUI.FlatLabel FLBLUrlTitle;
        private stCoreUI.FlatLabel FLBLIpTitle;
        private stCoreUI.FlatLabel FLBLUrlSet;
        private stCoreUI.FlatLabel FLBLIpSet;
        private stCoreUI.FlatLabel flatLabel1;
        private stCoreUI.FlatTextBox FTBSetupLog;
        private stCoreUI.FlatLabel flatLabel4;
        private stCoreUI.FlatTextBox FTBSetupIpTftp;
        private stCoreUI.FlatLabel flatLabel3;
        private stCoreUI.FlatTextBox FTBSetupOutput;
        private stCoreUI.FlatLabel flatLabel2;
        private stCoreUI.FlatTextBox FTBSetupInput;
        private stCoreUI.FlatButton FBSetupSave;
        private stCoreUI.FlatButton FBSetupRestore;
        private System.Windows.Forms.PictureBox PBSetupFolderInput;
        private System.Windows.Forms.PictureBox PBSetupFolderOutput;
        private stCoreUI.FlatToggle FTUrlListEnable;
        private stCoreUI.FlatToggle FTDeleteSourceEnable;
        private stCoreUI.FlatToggle FTSaveDefaultName;
        private stCoreUI.FlatLabel flatLabel7;
        private stCoreUI.FlatLabel flatLabel6;
        private stCoreUI.FlatLabel flatLabel5;
        private stCoreUI.FlatLabel FLBLImportTypeTitle;
        private stCoreUI.FlatLabel FLBLImportFileTitle;
        private stCoreUI.FlatLabel FLBLImportTypeSet;
        private stCoreUI.FlatLabel FLBLImportFileSet;
        private stCoreUI.FlatLabel flatLabel8;
        private stCoreUI.FlatRadioButton FRBAgregate0;
        private stCoreUI.FlatRadioButton FRBAgregate16;
        private stCoreUI.FlatRadioButton FRBAgregate24;
        private stCoreUI.FlatGroupBox flatGroupBox1;
        private stCoreUI.FlatNumeric FNTftpRetryMinutes;
        private stCoreUI.FlatNumeric FNTftpRetryMaxCount;
        private stCoreUI.FlatLabel flatLabel10;
        private stCoreUI.FlatLabel flatLabel9;
        private stCoreUI.FlatRadioButton FRBAgregate8;
    }
}

