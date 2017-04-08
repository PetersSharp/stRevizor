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
using stCoreUI;
using System.Net;
using stRevizor;
using stRevizor.Data;

namespace RevizorUI
{
    public partial class RevizorClient : Form
    {
        #region window Message/Error set

        private void _winMessageError(string msg)
        {
#if DEBUG
            if (!this.isMinimized)
            {
                string msgstack = "";
                StackFrame CallStack = null;
                for (int i = 1; i < 10; i++)
                {
                    CallStack = new StackFrame(i, true);
                    if ((CallStack != null) && (!string.IsNullOrWhiteSpace(CallStack.GetFileName())))
                    {
                        msgstack += string.Format(
                            "{0}[{1}:{2}]",
                            Environment.NewLine,
                            Path.GetFileName(CallStack.GetFileName()),
                            CallStack.GetFileLineNumber()
                        );
                    }
                    else
                    {
                        break;
                    }
                }
                try
                {
                    string ext = Path.GetExtension(this.logFile),
                           file = string.Concat(
                                this.logFile.Substring(0, (this.logFile.Length - ext.Length)),
                                "-",
                                DateTime.Now.ToShortDateString(),
                                ext
                           );
                    File.AppendAllText(
                        file,
                        DateTime.Now.ToShortTimeString() + Environment.NewLine + msg + Environment.NewLine + msgstack,
                        Encoding.UTF8
                    );
                }
                catch (Exception ex)
                {
                    this._winMessageError(ex.Message);
                }
                // Debug on screen
                // MessageBox.Show(msg + msgstack);
            }
#endif
            if (this.isMinimized)
            {
                // this.notifyIconMain_Balloon(msg, null, ToolTipIcon.Error, 10000);
            }
            else
            {
                this._winMessage(FlatAlertBox._Kind.Error, msg, 10000);
            }
        }
        private void _winMessageSuccess(string msg)
        {
            if (this.isMinimized)
            {
                // this.notifyIconMain_Balloon(msg, null, ToolTipIcon.Warning, 5000);
            }
            else
            {
                this._winMessage(FlatAlertBox._Kind.Success, msg, 5000);
            }
        }
        private void _winMessageInfo(string msg)
        {
            if (this.isMinimized)
            {
                //this.notifyIconMain_Balloon(msg, null, ToolTipIcon.Info, 5000);
            }
            else
            {
                this._winMessage(FlatAlertBox._Kind.Info, msg, 5000);
            }
        }
        private void _winMessage(FlatAlertBox._Kind type, string msg, int delay)
        {
            flatThreadSafe.Run(this, (Action)(() => this.FABMain.ShowControl(type, msg, delay)));
        }
        private void _winStatusBar(string msg)
        {
            if (this.isMinimized)
            {
                //this.notifyIconMain_Balloon(msg, null, ToolTipIcon.None, 10000);
            }
            else
            {
                // flatThreadSafe.Run(this, (Action)(() => this.flatSB.Text = msg));
            }
        }
        private void _winProgressBar(Int32 cnt)
        {
            flatThreadSafe.Run(this, (Action)(() => this.FPBMain.Value = cnt));
        }
        private void _winProgressBarHide(Int32 cnt)
        {
            if ((cnt == 0) || (cnt >= 100))
            {
                flatThreadSafe.Run(this, (Action)(() => this.FPBMain.Visible = false));
            }
            else if (!this.FPBMain.Visible)
            {
                flatThreadSafe.Run(this, (Action)(() => this.FPBMain.Visible = true));
            }
            this._winProgressBar(cnt);
        }
        private void SetSetupError(object ctrl, int pad, string txterr, ErrorIconAlignment align = ErrorIconAlignment.MiddleRight)
        {
            if (ctrl == null)
            {
                return;
            }
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetIconAlignment(((FlatTextBox)ctrl), align)));
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetIconPadding(((FlatTextBox)ctrl), pad)));
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetError(((FlatTextBox)ctrl), txterr)));
        }
        private void SetSearchError(string txterr)
        {
            flatThreadSafe.Run(this, (Action)(() => this.LBSearchIp.DataSource = null));
            this.SetSetupError(this.FTBSearchIp, 10, txterr, ErrorIconAlignment.MiddleLeft);
        }

        #endregion

        #region CheckInputFile method
        private ImportFileType _CheckInputFile(string fname)
        {
            try
            {
                if (!File.Exists(fname))
                {
                    throw new ArgumentException(
                        string.Format(
                            Properties.Resources.fmtImportNotFound,
                            fname
                        )
                    );
                }
                string fext = Path.GetExtension(fname);
                if (fext.Equals(".zip", StringComparison.InvariantCultureIgnoreCase))
                {
                    return ImportFileType.Zip;
                }
                else if (fext.Equals(".xml", StringComparison.InvariantCultureIgnoreCase))
                {
                    return ImportFileType.Xml;
                }
                else if (fext.Equals(".fw", StringComparison.InvariantCultureIgnoreCase))
                {
                    return ImportFileType.Fw;
                }
                throw new ArgumentException(
                    string.Format(
                        Properties.Resources.fmtImportTypeError,
                        fext,
                        fname
                    )
                );
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
            return ImportFileType.None;
        }
        #endregion

        #region CheckTypeFile method
        private ExportFileType _CheckTypeFile(List<string> importList)
        {
            
            IPAddress clientIpAddr;
            ExportFileType type = ExportFileType.None;

            for (int i = 2; i < 9; i += 2)
            {
                int idx = (importList.Count / i);
                if (idx > 0)
                {
                    if (importList[idx].StartsWith("http", StringComparison.InvariantCultureIgnoreCase))
                    {
                        if ((type != ExportFileType.None) && (type != ExportFileType.Url))
                        {
                            return ExportFileType.None;
                        }
                        type = ExportFileType.Url;
                    }
                    else if (IPAddress.TryParse(importList[idx], out clientIpAddr))
                    {
                        if ((type != ExportFileType.None) && (type != ExportFileType.Ip))
                        {
                            return ExportFileType.None;
                        }
                        type = ExportFileType.Ip;
                    }
                    else
                    {
                        return ExportFileType.None;
                    }
                }
            }
            return type;
        }
        #endregion

        #region CheckImportFile method
        private void _CheckImportFile(string path, bool isImport)
        {
            try
            {
                if (
                    (isImport) &&
                    (!string.IsNullOrWhiteSpace(path))
                   )
                {
                    flatThreadSafe.Run(this, (Action)(() => this.importTtype = this._CheckInputFile(path)));
                    if (this.importTtype != ImportFileType.None)
                    {
                        flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.setInputFile = path));
                        this._SetButton(null, true, true, false);
                        this._winMessage(
                            FlatAlertBox._Kind.Info,
                            string.Format(
                                Properties.Resources.fmtImportOnLoadPath,
                                this._PrnFileName(path)
                            ),
                            10000
                        );
                        this._SetFileInfo(path);
                    }
                    else
                    {
                        this._winMessageError(
                            string.Format(
                                Properties.Resources.fmtImportOnLoadNotSupport,
                                Path.GetExtension(path)
                            )
                        );
                    }
                }
                else if (isImport)
                {
                    this._winMessageError(Properties.Resources.txtImportOnLoadEmpty);
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }
        #endregion

        #region init Revizor method
        private bool _initRevizor()
        {
            if (this.rv == null)
            {
                try
                {
                    if (!string.IsNullOrWhiteSpace(Properties.Settings.Default.setIpTftpServer))
                    {
                        this.rv = new Revizor(
                            Properties.Settings.Default.setPortListEnable,
                            Properties.Settings.Default.setUrlListEnable,
                            Properties.Settings.Default.setIpTftpServer,
                            this.rootPath
                        );
                        rv.OnProcessError += (o, e) =>
                        {
                            this._winMessageError(e.ex.GetType().Name + ": " + e.ex.Message);
                        };
                        rv.ProgressBar = _winProgressBar;
                        rv.AggregateNet = Properties.Settings.Default.setAggregateNet;
                    }
                    else
                    {
                        throw new ArgumentException(Properties.Resources.txtSetupTftpServerNotComplette);
                    }
                }
                catch (Exception ex)
                {
                    this._winMessageError(ex.Message);
                }
            }
            return (this.rv != null);
        }
        #endregion

        #region CheckRevizor method
        private bool _CheckRevizor()
        {
            if (this.rv == null)
            {
                this._winMessageError(Properties.Resources.txtSetupTftpServerNotComplette);
                return false;
            }
            return true;
        }
        #endregion

        #region Print file name method
        private string _PrnFileName(string path)
        {
            string s, d = Path.GetDirectoryName(path),
                      f = Path.GetFileName(path);
            s = ((string.IsNullOrWhiteSpace(d)) ? "." : d.Substring(0, ((d.Length > 6) ? 6 : d.Length)));
            return string.Concat(
                s,
                ((d.Length > 6) ? ".." : ""),
                Path.DirectorySeparatorChar,
                f
            );
        }
        #endregion

        #region Set file info method
        private void _SetFileInfo(string path, ExportFileType etype = ExportFileType.None)
        {
            try
            {
                FileInfo fi = new FileInfo(path);
                flatThreadSafe.Run(this, (Action)(() => this.FLBLImportFileSet.Text = this._PrnFileName(path)));
                flatThreadSafe.Run(this, (Action)(() => this.FLBLDateSet.Text = fi.LastWriteTime.ToString()));
                if (etype != ExportFileType.None)
                {
                    flatThreadSafe.Run(this, (Action)(() => this.FLBLImportTypeSet.Text = etype.ToString()));
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }
        #endregion

        #region List method
        private void _ListSave(List<string> lst, ExportFileType type)
        {
            if (this.rv == null)
            {
                this._winMessageError(Properties.Resources.txtImportPathNotComplette);
                return;
            }
            if (this.SFDMain.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    string fileType;
                    switch (type)
                    {
                        case ExportFileType.Ip:
                            {
                                if (this.rv.IpCount == 0)
                                {
                                    this._winMessageError(Properties.Resources.txtProccessNotComplette);
                                    return;
                                }
                                fileType = this.rv.IpFileName;
                                break;
                            }
                        case ExportFileType.Url:
                            {
                                if (this.rv.UrlCount == 0)
                                {
                                    this._winMessageError(Properties.Resources.txtProccessNotComplette);
                                    return;
                                }
                                fileType = this.rv.UrlFileName;
                                break;
                            }
                        case ExportFileType.Port:
                            {
                                if (this.rv.PortIpCount == 0)
                                {
                                    this._winMessageError(Properties.Resources.txtProccessNotComplette);
                                    return;
                                }
                                fileType = this.rv.PortIpFileName;
                                break;
                            }
                        default:
                            {
                                this._winMessageError(
                                    string.Format(
                                        Properties.Resources.fmtImportTypeError,
                                        type.ToString(), ""
                                    )
                                );
                                return;
                            }
                    }
                    Properties.Settings.Default.setSaveFile = ((Properties.Settings.Default.setSaveDefaultName) ?
                        Path.Combine(
                            Path.GetDirectoryName(this.SFDMain.FileName),
                            fileType
                        ) :
                        this.SFDMain.FileName
                    );
                    this._winMessage(
                        FlatAlertBox._Kind.Info,
                        string.Format(
                            Properties.Resources.fmtImportPath,
                            this._PrnFileName(Properties.Settings.Default.setSaveFile)
                        ),
                        6000
                    );
                    switch (type)
                    {
                        case ExportFileType.Ip:
                            {
                                this.rv.SaveToFileIpList(Properties.Settings.Default.setSaveFile);
                                break;
                            }
                        case ExportFileType.Url:
                            {
                                this.rv.SaveToFileIpList(Properties.Settings.Default.setSaveFile);
                                break;
                            }
                        case ExportFileType.Port:
                            {
                                string path = Path.GetDirectoryName(Properties.Settings.Default.setSaveFile),
                                       file = Path.GetFileName(Properties.Settings.Default.setSaveFile);

                                this.rv.SaveToFilePortIpList(
                                    Path.Combine(path, string.Concat("port-", file)),
                                    Path.Combine(path, string.Concat("port-ip-", file))
                                );
                                break;
                            }
                    }
                }
                catch (Exception ex)
                {
                    this._winMessageError(ex.Message);
                }
            }
        }

        #endregion

        #region Button state

        private void _SetButton(string startText, bool inState, bool startState, bool saveState)
        {
            if (!string.IsNullOrWhiteSpace(startText))
            {
                flatThreadSafe.Run(this, (Action)(() => this.FSBStart.Text = startText));
            }
            flatThreadSafe.Run(this, (Action)(() => this.PBInputClear.Enabled = inState));
            flatThreadSafe.Run(this, (Action)(() => this.FBInput.Enabled      = inState));
            flatThreadSafe.Run(this, (Action)(() => this.FSBStart.Enabled     = startState));
            flatThreadSafe.Run(this, (Action)(() => this.FBIpSave.Enabled     = saveState));
            flatThreadSafe.Run(this, (Action)(() => this.FBUrlSave.Enabled    = saveState));
            flatThreadSafe.Run(this, (Action)(() => this.FBPortSave.Enabled   = saveState));
            flatThreadSafe.Run(this, (Action)(() => this.FTBSearchIp.Enabled  = saveState));
            flatThreadSafe.Run(this, (Action)(() => this.FBSearchIp.Enabled   = saveState));
            if (!saveState)
            {
                flatThreadSafe.Run(this, (Action)(() => this.LBSearchIp.DataSource = new List<string>()));
            }
        }

        private void _SetSearchResult(List<string> lstr)
        {
            if (lstr.Count > 0)
            {
                flatThreadSafe.Run(this, (Action)(() => this.FBSearchSave.Enabled = true));
                flatThreadSafe.Run(this, (Action)(() =>
                    this.FLSearchCount.Text =
                        string.Format(
                            Properties.Resources.fmtSearchCount,
                            lstr.Count
                        )
                ));
            }
            else
            {
                flatThreadSafe.Run(this, (Action)(() => this.FLSearchCount.Text = String.Empty));
            }

        }

        #endregion
    }
}
