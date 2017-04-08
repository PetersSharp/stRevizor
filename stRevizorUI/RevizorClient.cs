using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using stCore;
using stCoreUI;
using stRevizor;
using stRevizor.Data;
using System.Reflection;
using System.Net;
using System.Net.Sockets;

namespace RevizorUI
{
    /// <summary>
    /// RevizorClient
    /// </summary>
    public partial class RevizorClient : Form
    {
        private string rootPath;
        private string logFile;
        private bool isMinimized;
        private Revizor rv = null;
        private ImportFileType importTtype;

        private enum ImportFileType : int
        {
            None,
            Zip,
            Xml,
            Fw
        }
        private enum ExportFileType : int
        {
            None,
            Ip,
            Url,
            Port
        }

        /// <summary>
        /// Constructor RevizorClient
        /// </summary>
        /// <param name="importFilePath"></param>
        /// <param name="isImport"></param>
        public RevizorClient(string importFilePath, bool isImport)
        {
            this.rootPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            this.logFile = Path.Combine(
                this.rootPath,
                "data",
                Properties.Settings.Default.setLogFile
            );
            this.isMinimized = false;
            this.importTtype = ImportFileType.None;

            if (!Directory.Exists(Path.GetDirectoryName(this.logFile)))
            {
                Directory.CreateDirectory(Path.GetDirectoryName(this.logFile));
            }
            
            this.InitializeComponent();

            if (string.IsNullOrWhiteSpace(Properties.Settings.Default.setInputFile))
            {
                this._SetButton(null, true, false, false);
            }
            else
            {
                this._SetButton(null, true, true, false);
            }
            this._initRevizor();
            this._CheckImportFile(importFilePath, isImport);
        }

        private void FBInput_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.OFDMain.ShowDialog() == DialogResult.OK)
                {
                    this.importTtype = _CheckInputFile(this.OFDMain.FileName);
                    if (this.importTtype != ImportFileType.None)
                    {
                        flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setInputFile = this.OFDMain.FileName));
                        this._SetButton(null, true, true, false);
                        this._winMessage(
                            FlatAlertBox._Kind.Info,
                            string.Format(
                                Properties.Resources.fmtImportPath,
                                this._PrnFileName(this.OFDMain.FileName)
                            ),
                            10000
                        );
                        Properties.Settings.Default.Save();
                    }
                }
                else
                {
                    this.importTtype = ImportFileType.None;
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }

        private void FBIpSave_Click(object sender, EventArgs e)
        {
            if (_CheckRevizor())
            {
                this._ListSave(this.rv.IpList, ExportFileType.Ip);
            }
        }

        private void FBUrlSave_Click(object sender, EventArgs e)
        {
            if (_CheckRevizor())
            {
                this._ListSave(this.rv.UrlList, ExportFileType.Url);
            }
        }

        private void FBPortSave_Click(object sender, EventArgs e)
        {
            if (_CheckRevizor())
            {
                this._ListSave(this.rv.PortList, ExportFileType.Port);
            }
        }

        private void FSBStart_Click(object sender, EventArgs e)
        {
            try
            {
                if (_CheckRevizor())
                {
                    if (this.importTtype == ImportFileType.None)
                    {
                        this.importTtype = _CheckInputFile(Properties.Settings.Default.setInputFile);
                    }
                    switch (this.importTtype)
                    {
                        case ImportFileType.Zip:
                        case ImportFileType.Xml:
                        case ImportFileType.Fw:
                            {
                                this.FSBStart.Enabled = false;

                                if (this.BGWMain.IsBusy)
                                {
                                    this._SetButton(Properties.Resources.txtBTNWait, false, false, false);
                                    if (!this.BGWMain.CancellationPending)
                                    {
                                        if (this.BGWMain.WorkerSupportsCancellation)
                                        {
                                            this.BGWMain.CancelAsync();
                                        }
                                    }
                                    while (this.BGWMain.IsBusy)
                                    {
                                        Application.DoEvents();
                                    }
                                    this._SetButton(Properties.Resources.txtBTNStart, true, true, false);
                                }
                                else
                                {
                                    this._winMessage(
                                        FlatAlertBox._Kind.Info,
                                        Properties.Resources.txtBGWProccess,
                                        100000
                                    );
                                    this.BGWMain.RunWorkerAsync();
                                    this._SetButton(Properties.Resources.txtBTNStop, false, true, false);
                                }
                                break;
                            }
                        case ImportFileType.None:
                            {
                                throw new ArgumentException(Properties.Resources.txtImportPathNotComplette);
                            }
                    }
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }

        private void PBInputClear_Click(object sender, EventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => this.FTBInput.Text = Properties.Settings.Default.setInputFile = ""));
            this._SetButton(null, true, false, false);
            this._winMessage(
                FlatAlertBox._Kind.Info,
                Properties.Resources.txtBGWNotify,
                50000
            );
            Properties.Settings.Default.Save();
        }

        private void PBSetupFolderInput_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.OFDMain.ShowDialog() == DialogResult.OK)
                {
                    if (_CheckInputFile(this.OFDMain.FileName) != ImportFileType.None)
                    {
                        flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setInputFile = this.OFDMain.FileName));
                    }
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }

        private void PBSetupFolderOutput_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.SFDMain.ShowDialog() == DialogResult.OK)
                {
                    flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setSaveFile = this.SFDMain.FileName));
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }

        private void FBSetupRestore_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.Reload();
            this.Invalidate();
        }

        private void FBSetupSave_Click(object sender, EventArgs e)
        {
            this.EPMain.Clear();

            if (string.IsNullOrWhiteSpace(this.FTBSetupIpTftp.Text))
            {
                this.SetSetupError(this.FTBSetupIpTftp, 4, Properties.Resources.txtErrorProvSetupIpTftp);
                return;
            }
            if (string.IsNullOrWhiteSpace(this.FTBSetupLog.Text))
            {
                this.SetSetupError(this.FTBSetupLog, 4, Properties.Resources.txtErrorProvSetupLog);
                return;
            }
            long l1 = this.FNTftpRetryMinutes.Value,
                 l2 = this.FNTftpRetryMaxCount.Value;

            flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setTftpRetryMinutes = l1));
            flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setTftpRetryMaxCount = l2));
            flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setAggregateNet =
                    ((this.FRBAgregate0.Checked) ? 0 :
                        ((this.FRBAgregate24.Checked) ? 24 :
                            ((this.FRBAgregate16.Checked) ? 16 :
                                ((this.FRBAgregate8.Checked) ? 8 : 0)
                            )
                        )
                    )
            ));
            Properties.Settings.Default.Save();
            if (this.rv != null)
            {
                flatThreadSafe.Run(this, (Action)(() => rv.AggregateNet = Properties.Settings.Default.setAggregateNet));
                flatThreadSafe.Run(this, (Action)(() => rv.PortListEnable = Properties.Settings.Default.setPortListEnable));
                flatThreadSafe.Run(this, (Action)(() => rv.UrlListEnable = Properties.Settings.Default.setUrlListEnable));
            }
            this.Invalidate();
        }

        private void FTBSearchIp_TextChanged(object sender, EventArgs e)
        {
            this.EPMain.Clear();
        }

        private void FBSearchIp_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.rv == null)
                {
                    this.SetSearchError(Properties.Resources.txtSetupTftpServerNotComplette);
                    return;
                }
                if (!string.IsNullOrWhiteSpace(this.FTBSearchIp.Text))
                {
                    string ipa = this.FTBSearchIp.Text.Trim();
                    List<string> lsrc;

                    flatThreadSafe.Run(this, (Action)(() => this.FBSearchSave.Enabled = false));

                    if (this.FCBSearcInSearch.Checked)
                    {
                        lsrc = (List<string>)this.LBSearchIp.DataSource;
                        if ((lsrc == null) || (lsrc.Count == 0))
                        {
                            flatThreadSafe.Run(this, (Action)(() => this.FCBSearcInSearch.Checked = false));
                            flatThreadSafe.Run(this, (Action)(() => this.FLSearchCount.Text = String.Empty));
                            this.SetSearchError(Properties.Resources.txtSearchErrorQueryEmpty);
                        }
                        else
                        {
                            flatThreadSafe.Run(this, (Action)(() => this.FCBSearchStrong.Checked = false));
                        }
                    }
                    else if (this.rv.IpCount > 0)
                    {
                        lsrc = (List<string>)this.rv.IpList;
                    }
                    else
                    {
                        flatThreadSafe.Run(this, (Action)(() => this.FLSearchCount.Text = String.Empty));
                        this.SetSearchError(Properties.Resources.txtProccessNotComplette);
                        return;
                    }
                    if (this.FCBSearchStrong.Checked)
                    {
                        IPAddress addr;
                        if (IPAddress.TryParse(ipa, out addr))
                        {
                            switch (addr.AddressFamily)
                            {
                                case AddressFamily.InterNetwork:
                                    {
                                        ipa = this.rv.AggregateToNet(ipa, Properties.Settings.Default.setAggregateNet);
                                        if (!string.IsNullOrWhiteSpace(ipa))
                                        {
                                            flatThreadSafe.Run(this, (Action)(() => this.LBSearchIp.DataSource = lsrc.Where(o => o.Equals(ipa)).ToList<string>()));
                                            _SetSearchResult((List<string>)this.LBSearchIp.DataSource);
                                            return;
                                        }
                                        break;
                                    }
                                case AddressFamily.InterNetworkV6:
                                    {
                                        this.SetSearchError(Properties.Resources.txtSearchErrorIPV6);
                                        return;
                                    }
                                default:
                                    {
                                        break;
                                    }
                            }
                        }
                        this.SetSearchError(Properties.Resources.txtSearchErrorIPNotValid);
                        return;
                    }
                    else
                    {
                        if (ipa.Contains("."))
                        {
                            flatThreadSafe.Run(this, (Action)(() => this.LBSearchIp.DataSource = lsrc.Where(o => o.StartsWith(ipa)).ToList<string>()));
                        }
                        else
                        {
                            flatThreadSafe.Run(this, (Action)(() => this.LBSearchIp.DataSource = lsrc.Where(o => o.Contains(ipa)).ToList<string>()));
                        }
                        _SetSearchResult((List<string>)this.LBSearchIp.DataSource);
                    }
                }
                else
                {
                    this.SetSearchError(Properties.Resources.txtSearchErrorIPEmpty);
                    flatThreadSafe.Run(this, (Action)(() => this.FLSearchCount.Text = String.Empty));
                }
            }
            catch (Exception ex)
            {
                this.SetSearchError(ex.Message);
                flatThreadSafe.Run(this, (Action)(() => this.FLSearchCount.Text = String.Empty));
            }
        }

        private void FBSearchSave_Click(object sender, EventArgs e)
        {
            try
            {
                List<string> lsrc = (List<string>)this.LBSearchIp.DataSource;
                if ((lsrc != null) && (lsrc.Count > 0))
                {
                    if (this.SFDMain.ShowDialog() == DialogResult.OK)
                    {
                        flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setSaveFile = this.SFDMain.FileName));
                        File.WriteAllLines(
                            this.SFDMain.FileName,
                            lsrc.ToArray()
                        );
                    }
                }
                else
                {
                    throw new ArgumentException(Properties.Resources.txtSearchErrorSaveEmpty);
                }
            }
            catch (Exception ex)
            {
                this.SetSearchError(ex.Message);
            }
        }

        private void FTBSearchIp_KeyDown(object sender, KeyEventArgs e)
        {
            if (sender == (object)this.FTBSearchIp)
            {
                switch (e.KeyCode)
                {
                    case Keys.Escape:
                        {
                            this.FTBSearchIp.Text = String.Empty;
                            break;
                        }
                    case Keys.Return:
                        {
                            this.FBSearchIp_Click((object)this.FBSearchIp, e);
                            break;
                        }
                }
            }
        }

        private void FTPortListEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (sender == (object)this.FTPortListEnable)
            {
                FlatToggle ft = (FlatToggle)sender;
                if (ft.Checked)
                {
                    ft.CheckedChanged -= FTPortListEnable_CheckedChanged;
                    flatThreadSafe.Run(this, (Action)(() => this.FTUrlListEnable.Checked = false));
                    flatThreadSafe.Run(this, (Action)(() => this.FRBAgregate24.Checked = false));
                    flatThreadSafe.Run(this, (Action)(() => this.FRBAgregate16.Checked = false));
                    flatThreadSafe.Run(this, (Action)(() => this.FRBAgregate8.Checked = false));
                    flatThreadSafe.Run(this, (Action)(() => this.FRBAgregate0.Checked = true));
                    ft.CheckedChanged += FTPortListEnable_CheckedChanged;
                }
            }
        }

        private void FTUrlListEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (sender == (object)this.FTUrlListEnable)
            {
                FlatToggle ft = (FlatToggle)sender;
                if (ft.Checked)
                {
                    ft.CheckedChanged -= FTUrlListEnable_CheckedChanged;
                    flatThreadSafe.Run(this, (Action)(() => this.FTPortListEnable.Checked = false));
                    ft.CheckedChanged += FTUrlListEnable_CheckedChanged;
                }
            }
        }
    }
}
