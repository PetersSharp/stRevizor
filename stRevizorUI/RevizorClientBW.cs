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
using stRevizor;
using stRevizor.Data;
using System.Threading;

namespace RevizorUI
{
    public partial class RevizorClient : Form
    {
        #region BGWMain DoWork
        private void BGWMain_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                bool ret = false;
                List<string> importList = null;
                ExportFileType exportFileType = ExportFileType.None;

                if (this.rv == null)
                {
                    e.Cancel = true;
                    throw new ArgumentException(Properties.Resources.txtImportPathNotComplette);
                }
                if (this.rv.TestRemoteDataBusy())
                {
                    this._winMessageError(
                        string.Format(
                            Properties.Resources.fmtTftpServerBusy,
                            Properties.Settings.Default.setTftpRetryMinutes
                        )
                    );
                    this._SetButton(null, false, true, false);

                    DateTime dts = DateTime.Now.AddMinutes(Properties.Settings.Default.setTftpRetryMinutes);
                    Int32 cnt = 0, max = 0;

                    while (dts > DateTime.Now)
                    {
                        flatThreadSafe.Run(this, (Action)(() => 
                            this.FSBStart.Text = string.Format(
                                Properties.Resources.fmtBTNStartBusy,
                                new DateTime((dts - DateTime.Now).Ticks).ToString("mm:ss")
                            )
                        ));
                        flatThreadSafe.Run(this, (Action)(() => this.FSBStart.Invalidate()));
                        
                        if (max >= Properties.Settings.Default.setTftpRetryMaxCount)
                        {
                            this._winMessageError(
                                string.Format(
                                    Properties.Resources.fmtTftpServerSkipBusy,
                                    Properties.Settings.Default.setTftpRetryMaxCount
                                )
                            );
                            e.Cancel = true;
                            return;
                        }
                        if (cnt++ == 10)
                        {
                            if (this.rv.TestRemoteDataBusy())
                            {
                                dts = DateTime.Now.AddMinutes(Properties.Settings.Default.setTftpRetryMinutes);
                                this._winMessageError(Properties.Resources.fmtTftpServerBusy);
                            }
                            else
                            {
                                this._SetButton(Properties.Resources.txtBTNStop, false, true, false);
                                this._winMessageInfo(Properties.Resources.txtTftpServerEndBusy);
                                break;
                            }
                            cnt = 0;
                            max++;
                        }
                        if (this.BGWMain.CancellationPending)
                        {
                            e.Cancel = true;
                            return;
                        }
                        Application.DoEvents();
                        Thread.Sleep(6000);
                    }
                }
                //
                switch (this.importTtype)
                {
                    case ImportFileType.Zip:
                        {
                            ret = this.rv.ParseZip(Properties.Settings.Default.setInputFile);
                            break;
                        }
                    case ImportFileType.Xml:
                        {
                            ret = this.rv.ParseXml(Properties.Settings.Default.setInputFile);
                            break;
                        }
                    case ImportFileType.Fw:
                        {
                            importList = File.ReadAllLines(Properties.Settings.Default.setInputFile).ToList<string>();
                            if ((importList == null) || (importList.Count == 0))
                            {
                                e.Cancel = true;
                                throw new ArgumentException(Properties.Resources.txtImportOnLoadError);
                            }
                            exportFileType = this._CheckTypeFile(importList);
                            ret = (bool)(exportFileType != ExportFileType.None);
                            break;
                        }
                    case ImportFileType.None:
                        {
                            e.Cancel = true;
                            throw new ArgumentException(Properties.Resources.txtImportPathNotComplette);
                        }
                }
                if (!ret)
                {
                    e.Cancel = true;
                    throw new ArgumentException(Properties.Resources.txtImportProccessNotComplette);
                }
                switch (this.importTtype)
                {
                    case ImportFileType.Xml:
                    case ImportFileType.Zip:
                        {
                            if (rv.IpCount > 0)
                            {
                                rv.SendToTftpIpList(rv.IpList);
                            }
                            if (rv.UrlCount > 0)
                            {
                                rv.SendToTftpUrlList(rv.IpList);
                            }
                            break;
                        }
                    case ImportFileType.Fw:
                        {
                            if (importList == null)
                            {
                                break;
                            }
                            this._SetFileInfo(Properties.Settings.Default.setInputFile, exportFileType);
                            switch (exportFileType)
                            {
                                case ExportFileType.Ip:
                                    {
                                        flatThreadSafe.Run(this, (Action)(() => this.FLBLIpSet.Text = importList.Count.ToString()));
                                        rv.SendToTftpIpList(importList);
                                        break;
                                    }
                                case ExportFileType.Url:
                                    {
                                        flatThreadSafe.Run(this, (Action)(() => this.FLBLUrlSet.Text = importList.Count.ToString()));
                                        rv.SendToTftpUrlList(importList);
                                        break;
                                    }
                                default:
                                    {
                                        e.Cancel = true;
                                        this.importTtype = ImportFileType.None;
                                        throw new ArgumentException(
                                            string.Format(
                                                Properties.Resources.txtImportOnLoadNotExportType,
                                                Path.GetFileName(Properties.Settings.Default.setInputFile)
                                            )
                                        );
                                    }
                            }
                            break;
                        }
                }
                rv.SendToTftpWhiteList();
                rv.SendToTftpUploadComplete();
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }
        #endregion

        #region BGWMain ProgressChanged
        private void BGWMain_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            this._winProgressBar(e.ProgressPercentage);
        }
        #endregion

        #region BGWMain RunWorkerCompleted
        private void BGWMain_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            try
            {
                string status;
                FlatAlertBox._Kind type;

                if (e.Cancelled)
                {
                    status = Properties.Resources.txtBGWCancelled;
                    type = FlatAlertBox._Kind.Info;
                }
                else if (e.Error != null)
                {
#if DEBUG
                    status = e.Error.Message;
#else
                    status = Properties.Resources.txtBGWProcessAbort;
#endif
                    type = FlatAlertBox._Kind.Error;
                }
                else if (this.importTtype == ImportFileType.Fw)
                {
                    status = Properties.Resources.txtImportOnLoadProccessEnd;
                    type = FlatAlertBox._Kind.Success;
                }
                else
                {
                    status = Properties.Resources.txtBGWCompleted;
                    type = FlatAlertBox._Kind.Success;
                }
                status = string.Format(
                    "{0} : {1}",
                    status,
                    DateTime.Now.ToShortTimeString()
                );
                this._winProgressBar(0);
                this._winMessage(type, status, 25000);

                switch (this.importTtype)
                {
                    case ImportFileType.Zip:
                    case ImportFileType.Xml:
                        {
                            if (this.rv != null)
                            {
                                flatThreadSafe.Run(this, (Action)(() => this.FLBLDateSet.Text = this.rv.UpdateTime));
                                flatThreadSafe.Run(this, (Action)(() => this.FLBLIpSet.Text = this.rv.IpCount.ToString()));
                                flatThreadSafe.Run(this, (Action)(() => this.FLBLUrlSet.Text = this.rv.UrlCount.ToString()));
                            }
                            break;
                        }
                    case ImportFileType.Fw:
                        {
                            break;
                        }
                }
                if (Properties.Settings.Default.setDeleteSourceEnable)
                {
                    if (
                        (!string.IsNullOrWhiteSpace(Properties.Settings.Default.setInputFile)) &&
                        (File.Exists(Properties.Settings.Default.setInputFile))
                       )
                    {
                        File.Delete(Properties.Settings.Default.setInputFile);
                    }
                    flatThreadSafe.Run(this, (Action)(() => this.FTBInput.Text = Properties.Settings.Default.setInputFile = ""));
                    flatThreadSafe.Run(this, (Action)(() => this.importTtype = ImportFileType.None));
                    this._SetButton(Properties.Resources.txtBTNStart, true, false, true);
                }
                else
                {
                    this._SetButton(Properties.Resources.txtBTNStart, true, true, true);
                }
            }
            catch (Exception ex)
            {
                this._winMessageError(ex.Message);
            }
        }
        #endregion
    }
}
