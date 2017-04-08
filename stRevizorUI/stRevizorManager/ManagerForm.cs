
#region COPYRIGHT (c) 2017 by Peters Sharp (MIT License)
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
#endregion

// #define NOT_SHOW_VERSION

using System;
using System.Windows.Forms;
using System.IO;
using stCoreUI;
using System.Reflection;
using System.Diagnostics;
using System.Globalization;
using System.Threading.Tasks;

namespace stRevizor
{
    public partial class Manager : Form
    {
        bool __isRaisingEvents = false;
        long __TimerDropMin = 0;
        object __isLockStat = new object();
        object __isLockBallon = new object();
        object __isLockProccess = new object();
        stRevizor.Engine stRv = null;

        public bool __isMinimized
        {
            get { return this.NotifyMain.Visible; }
        }

        enum __ButtonState : int
        {
            Start,
            Stop,
            Setup,
            Desabled,
        }

        public Manager()
        {
            InitializeComponent();
            this.flatRadioButton_SetDefault();
            this.flatRadioButton_SetChange();
            this.flatButtonTimerDrop.Text = Properties.Resources.ButtonTimerDrop;
            this.labelVerion.Text = "v:" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
            
            // overwrite, stupid constructor (always set default value)
            this.flatContextMenuLog.ShowImageMargin = true;
            this.flatContextMenuSendServer.ShowImageMargin = true;
            this.fileSystemWatch.EnableRaisingEvents = false;
            this.NotifyMain.Visible = false;
            __Set_NotifyIcon(false);
            // end overwrite

#if NOT_SHOW_VERSION
            this.flatTabControlMain.TabPages.RemoveAt(3);
#endif

            if (!__SetupFolder_Check(true))
            {
                __SetupFolder_Text(__ButtonState.Setup);
            }
            else
            {
                __SetupFolder_Text(__ButtonState.Start);
            }

            if (Properties.Settings.Default.SetupServerType)
            {
                this.flatRadioButtonFTP.Checked = true;
                this.flatRadioButtonTFTP.Checked = false;
            }
            else
            {
                this.flatRadioButtonTFTP.Checked = true;
                this.flatRadioButtonFTP.Checked = false;
            }

            __Set_LogMsg(
                string.Format(
                    "{0} {1} - {2}",
                    Properties.Resources.ButtonStart,
                    this.formSkinMain.Text,
                    this.labelVerion.Text
                )
            );

            stRv = new Engine(
                stCore.IOBaseAssembly.BaseDataDir(),
                Properties.Settings.Default.SetupServerType,
                Properties.Settings.Default.SetupServer,
                Properties.Settings.Default.SetupServerFTPLogin,
                Properties.Settings.Default.SetupServerFTPPass
            );
            stRv.OnProcessError += (o, e) =>
            {
                string msg = string.Format("{0} : {1}", e.ex.GetType().Name, e.ex.Message);
                __Set_AlertMsg(FlatAlertBox._Kind.Error, msg);
                __Set_LogMsg(msg);

            };
            stRv.OnProcessLog += (o, e) =>
            {
                string msg = string.Format("{0} : {1}", e.ex.GetType().Name, e.ex.Message);
                __Set_LogMsg(msg);
            };

            /* disable in Engine dll
                  // stRv.ProgressBar = __Set_ProgressBar;
             */

            this.timerSrvInfo_Tick(this, null);
            this.timerSrvInfo.Enabled = true;
        }

        private void timerSrvInfo_Tick(object sender, EventArgs e)
        {
            lock (__isLockStat)
            {
                flatThreadSafe.Run(this, (Action)(() =>
                    this.ServerInfoBindingSource.Clear()
                ));

                Task<ServerInfo.SiRoot> t1 = stRv.ReceiveAutoconfigT();
                t1.ContinueWith((o) =>
                {
                    ServerInfo.SiRoot sio;
                    if ((sio = o.Result) != null)
                    {
                        flatThreadSafe.Run(this, (Action)(() =>
                            this.ServerInfoBindingSource.Add(sio)
                        ));
                    }
                    o.Dispose();
                }, TaskContinuationOptions.NotOnFaulted | TaskContinuationOptions.NotOnCanceled);
                t1.ContinueWith((o) =>
                {
                    string.Format(
                        Properties.Resources.LogTaskError,
                        o.Status.ToString(),
                        ((o.Exception != null) ? o.Exception.Message : "-")
                    );
                    o.Dispose();
                }, TaskContinuationOptions.OnlyOnFaulted);
            }
        }

        private void flatButtonReload_Click(object sender, EventArgs e)
        {
            this.timerSrvInfo_Tick(sender, e);
        }

        #region Private SET value

        private void __Set_StatusBar(string msg)
        {
            flatThreadSafe.Run(this, (Action)(() =>
                this.flatStatusBarInfo.Text = Properties.Resources.StatusString1 + msg
            ));
        }

        private void __Set_LogMsg(string msg)
        {
            if (Properties.Settings.Default.SetupLogEnable)
            {
                flatThreadSafe.Run(this, (Action)(() =>
                    this.ListBoxLog.Items.Add(
                        string.Format("{0:dd/MM/yy HH:mm:ss} - {1}", DateTime.Now, msg)
                    )
                ));
            }
            if (__isMinimized)
            {
                __Set_NotifyBallon(msg, null, ToolTipIcon.None, 10000);
            }
            else
            {
                __Set_StatusBar(msg);
            }
        }

        private void __Set_AlertMsg(FlatAlertBox._Kind type, string msg)
        {
            if (__isMinimized)
            {
                __Set_NotifyBallon(msg, null, ToolTipIcon.Info, 10000);
            }
            else
            {
                flatThreadSafe.Run(this, (Action)(() =>
                    this.flatAlertBoxInfo.ShowControl(
                        type,
                        msg,
                        ((type == FlatAlertBox._Kind.Error) ? 20000 : 10000)
                    )
                ));
            }
        }

        /* disable in Engine dll
        private void __Set_ProgressBar(Int32 cnt)
        {
            flatThreadSafe.Run(this, (Action)(() =>
                this.FPBMain.Value =
                    ((cnt >= 0) ? ((cnt > 100) ? 100 : cnt) : 0)
            ));
        }
        */

        private void __Set_ErrorProvider(object ctrl, int pad, string txterr, ErrorIconAlignment align = ErrorIconAlignment.MiddleRight)
        {
            if (ctrl == null)
            {
                return;
            }
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetIconAlignment(((FlatTextBox)ctrl), align)));
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetIconPadding(((FlatTextBox)ctrl), pad)));
            flatThreadSafe.Run(this, (Action)(() => this.EPMain.SetError(((FlatTextBox)ctrl), txterr)));
        }

        private void __Set_NotifyIcon(bool isRun)
        {
            flatThreadSafe.Run(this, (Action)(() =>
                this.NotifyMain.Icon =
                    ((isRun) ?
                        Properties.Resources.security_firewall_on : Properties.Resources.security_firewall_off
                    )
            ));
        }

        private void __Set_NotifyBallon(string msg, string title = null, ToolTipIcon ti = ToolTipIcon.None, int tm = 0)
        {
            if (__isMinimized)
            {
                lock (__isLockBallon)
                {
                    flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.BalloonTipIcon = ti));
                    flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.BalloonTipTitle = ((string.IsNullOrEmpty(title)) ? "" : title)));
                    flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.BalloonTipText = ((string.IsNullOrEmpty(msg)) ? "" : msg)));
                    flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.ShowBalloonTip((tm > 0) ? tm : 1000)));
                }
            }
        }

        #endregion

        #region SetupFolder SET/OPEN

        private void __SetupFolder_Text(__ButtonState state)
        {
            switch (state)
            {
                case __ButtonState.Start:
                    {
                        flatThreadSafe.Run(this, (Action)(() =>
                            this.flatButtonStartStop.Text = Properties.Resources.ButtonStart
                        ));
                        break;
                    }
                case __ButtonState.Stop:
                    {
                        flatThreadSafe.Run(this, (Action)(() =>
                            this.flatButtonStartStop.Text = Properties.Resources.ButtonStop
                        ));
                        break;
                    }
                case __ButtonState.Setup:
                    {
                        flatThreadSafe.Run(this, (Action)(() =>
                            this.flatButtonStartStop.Text = Properties.Resources.ButtonSetup
                        ));
                        break;
                    }
                case __ButtonState.Desabled:
                    {
                        flatThreadSafe.Run(this, (Action)(() =>
                            this.flatButtonStartStop.Text = Properties.Resources.ButtonDisabled
                        ));
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
            flatThreadSafe.Run(this, (Action)(() =>
                this.flatButtonStartStop.Refresh()
            ));
        }

        private bool __SetupFolder_Check(bool isCheck = false)
        {
            if ((!this.fileSystemWatch.EnableRaisingEvents) || (isCheck))
            {
                if (string.IsNullOrWhiteSpace(Properties.Settings.Default.SetupFolder))
                {
                    __Set_AlertMsg(FlatAlertBox._Kind.Error, Properties.Resources.AlertDirEmpty);
                    return false;
                }
                if (!Directory.Exists(Properties.Settings.Default.SetupFolder))
                {
                    __Set_AlertMsg(FlatAlertBox._Kind.Error, Properties.Resources.AlertDirNotFound);
                    return false;
                }
            }
            return true;
        }

        private void pictureBoxFolder_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.OFDMain.ShowDialog() == DialogResult.OK)
                {
                    flatThreadSafe.Run(this, (Action)(() =>
                        Properties.Settings.Default.SetupFolder = this.OFDMain.SelectedPath + Path.DirectorySeparatorChar
                    ));
                }
                __Set_StatusBar(
                    Properties.Resources.FolderPath + Properties.Settings.Default.SetupFolder
                );
            }
            catch (Exception ex)
            {
                __Set_LogMsg(ex.Message);
            }
        }

        #endregion

        #region File System Watch function

        private bool __FileWatcher_Run(bool isRun)
        {
            try
            {
                this.fileSystemWatch.EnableRaisingEvents = isRun;
                __Set_LogMsg(
                    string.Format(
                        "{0}: {1}",
                        ((isRun) ? Properties.Resources.LogManagerStart : Properties.Resources.LogManagerStop),
                        ((string.IsNullOrWhiteSpace(this.fileSystemWatch.Path)) ? "---" : this.fileSystemWatch.Path)
                    )
                );
                __Set_NotifyIcon(isRun);
                return true;
            }
            catch (Exception ex)
            {
                __Set_AlertMsg(FlatAlertBox._Kind.Error, ex.Message);
                __Set_NotifyIcon(false);
                return false;
            }
        }

        private void fileSystemWatch_Processed(object sender, System.IO.FileSystemEventArgs e)
        {
            Task t1 = Task.Factory.StartNew(() =>
            {
                stRv.WatchFileList.ForEach(o =>
                {
                    if (o.Equals(e.Name))
                    {
                        if (__SourceProcessed(e.FullPath, e.Name))
                        {
                            return;
                        }
                    }
                });
            });
            t1.ContinueWith((o) =>
            {
                o.Dispose();
            }, TaskContinuationOptions.NotOnFaulted | TaskContinuationOptions.NotOnCanceled);
            t1.ContinueWith((o) =>
            {
                string.Format(
                    Properties.Resources.LogTaskError,
                    o.Status.ToString(),
                    ((o.Exception != null) ? o.Exception.Message : "-")
                );
                o.Dispose();
            }, TaskContinuationOptions.OnlyOnFaulted);
        }

        private void pictureBoxLoadWait_Click(object sender, EventArgs e)
        {
            if (!this.flatContextMenuSendServer.Visible)
            {
                System.Drawing.Size msz = this.flatContextMenuSendServer.Size;
                System.Drawing.Point gpos = this.Location,
                                     wpos = this.pictureBoxLoadWait.Location;
                int x = ((gpos.X + wpos.X) - msz.Width),
                    y = ((gpos.Y + wpos.Y) + 54);

                this.flatContextMenuSendServer.Show(x, y);
            }
        }

        private void toolStripMenuItem3_Click(object sender, EventArgs e)
        {
            if (this.timerSendServer.Enabled)
            {
                this.timerSendServer.Enabled = false;
            }
            if (this.pictureBoxLoadWait.Image != null)
            {
                this.pictureBoxLoadWait.Image.Dispose();
                this.pictureBoxLoadWait.Image = null;
            }
            this.pictureBoxLoadWait.Click -= new System.EventHandler(this.pictureBoxLoadWait_Click);
            this.pictureBoxLoadWait.Cursor = Cursors.Default;
            stRv.LastFileSorage = null;
        }

        private void timerSendServer_Tick(object sender, EventArgs e)
        {
            __Set_LogMsg(
                Properties.Resources.LogResend
            );
            Task t1 = Task.Factory.StartNew(() =>
            {
                InputSorage fs = stRv.LastFileSorage;
                if ((fs == null) || (__SourceProcessed(fs.fPath, fs.fName)))
                {
                    flatThreadSafe.Run(this, (Action)(() =>
                        this.toolStripMenuItem3_Click(sender, e)
                    ));
                }
            });
            t1.ContinueWith((o) =>
            {
                o.Dispose();
            }, TaskContinuationOptions.NotOnFaulted | TaskContinuationOptions.NotOnCanceled);
            t1.ContinueWith((o) =>
            {
                __Set_LogMsg(
                    string.Format(
                        Properties.Resources.LogTaskError,
                        o.Status.ToString(),
                        ((o.Exception != null) ? o.Exception.Message : "-")
                    )
                );
                o.Dispose();
            }, TaskContinuationOptions.OnlyOnFaulted);
        }

        private bool __SourceProcessed(string fPath, string fName)
        {
            lock (__isLockProccess)
            {
                try
                {
                    string msg = stRevizor.stXFTPUtil.GetLocalFileSize(fPath);
                    msg = string.Format(
                        Properties.Resources.LogWatchFileFound,
                        fName,
                        ((string.IsNullOrWhiteSpace(msg)) ? "-" : msg)
                    );
                    __Set_LogMsg(msg);
                    __Set_AlertMsg(FlatAlertBox._Kind.Info, msg);
                }
                catch(Exception e)
                {
                    __Set_AlertMsg(
                        FlatAlertBox._Kind.Error,
                        string.Format(Properties.Resources.AlertFileStatError, fName, e.Message)
                    );
                    return false;
                }

                Engine.State st;

                switch ((st = stRv.BeginSource(fPath, fName)))
                {
                    case Engine.State.ListSuccess:
                        {
                            flatThreadSafe.Run(this, (Action)(() =>
                                Properties.Settings.Default.DateUpServer = DateTime.Now.ToString()
                            ));
                            if (!string.IsNullOrWhiteSpace(stRv.UpdateTime))
                            {
                                try
                                {
                                    DateTime dtUp = DateTime.Parse(stRv.UpdateTime);
                                    flatThreadSafe.Run(this, (Action)(() =>
                                        Properties.Settings.Default.DateUpList = dtUp.ToString()
                                    ));
                                }
                                catch
                                {
                                    flatThreadSafe.Run(this, (Action)(() =>
                                        Properties.Settings.Default.DateUpList = "???"
                                    ));
                                }
                            }

                            Properties.Settings.Default.Save();
                            string nPath = string.Format("{0}.old", fPath);

                            try
                            {
                                File.Delete(nPath);
                            }
                            catch { }
                            try
                            {
                                File.Move(fPath, nPath);
                            }
                            catch { }
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Success,
                                Properties.Resources.AlertSendSuccess
                            );
                            return true;
                        }
                    case Engine.State.ServerBusy:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertSendBusy
                            );
                            stRv.AddToFileSorage(fPath, fName);
                            if (!this.timerSendServer.Enabled)
                            {
                                this.timerSendServer.Enabled = true;
                                this.pictureBoxLoadWait.Cursor = Cursors.Hand;
                                this.pictureBoxLoadWait.Image = Properties.Resources.ajax_loader3;
                                this.pictureBoxLoadWait.Click += new System.EventHandler(this.pictureBoxLoadWait_Click);
                            }
                            break;
                        }
                    case Engine.State.ServerError:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertErrorServer
                            );
                            break;
                        }
                    case Engine.State.ListError:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertErrorLocal
                            );
                            break;
                        }
                    default:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                string.Format(
                                    Properties.Resources.AlertOtherError,
                                    st.ToString()
                                )
                            );
                            break;
                        }
                }
            }
            return false;
        }

        #endregion

        #region flatRadioButton SET value

        private void flatRadioButton_SetDefault()
        {
            if (Properties.Settings.Default.SetupServerType)
            {
                this.flatRadioButtonTFTP.Checked = false;
                this.flatRadioButtonFTP.Checked = true;
            }
            else
            {
                this.flatRadioButtonTFTP.Checked = true;
                this.flatRadioButtonFTP.Checked = false;
            }
            this.flatRadioButtonFTP.Refresh();
            this.flatRadioButtonTFTP.Refresh();
        }

        private void flatRadioButton_SetChange()
        {
            if (Properties.Settings.Default.SetupServerType)
            {
                this.flatTextBoxServerFTPLogin.Enabled = true;
                this.flatTextBoxServerFTPPass.Enabled = true;
            }
            else
            {
                this.flatTextBoxServerFTPLogin.Enabled = false;
                this.flatTextBoxServerFTPPass.Enabled = false;
            }
            this.flatTextBoxServerFTPLogin.Refresh();
            this.flatTextBoxServerFTPPass.Refresh();
        }

        private void flatRadioButtonTFTP_CheckedChanged(object sender, EventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.SetupServerType =
                ((this.flatRadioButtonTFTP.Checked) ? false : true)
            ));
            flatThreadSafe.Run(this, (Action)(() => this.flatStatusBarInfo.Text = "set: " + Properties.Settings.Default.SetupServerType));
            flatRadioButton_SetChange();
            __Set_StatusBar(
                ((this.flatRadioButtonTFTP.Checked) ? Properties.Resources.ServerTypeTFTP : Properties.Resources.ServerTypeFTP)
            );
        }
        
        private void flatRadioButtonFTP_CheckedChanged(object sender, EventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.SetupServerType =
                ((this.flatRadioButtonFTP.Checked) ? true : false)
            ));
            flatRadioButton_SetChange();
            __Set_StatusBar(
                ((this.flatRadioButtonFTP.Checked) ? Properties.Resources.ServerTypeFTP : Properties.Resources.ServerTypeTFTP)
            );
        }

        #endregion

        #region Log Context menu

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.SFDMain.ShowDialog() == DialogResult.OK)
                {
                    using (FileStream fp = File.Open(this.SFDMain.FileName, FileMode.CreateNew))
                    {
                        using (StreamWriter sw = new StreamWriter(fp))
                        {
                            foreach (var str in this.ListBoxLog.Items)
                            {
                                sw.WriteLine(str.ToString());
                            }
                        }
                    }
                    __Set_StatusBar(
                        string.Format(Properties.Resources.StatusSaveLog, this.SFDMain.FileName)
                    );
                }
            }
            catch (Exception ex)
            {
                __Set_LogMsg(ex.Message);
            }
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            this.ListBoxLog.Items.Clear();
        }

        #endregion

        private void flatButtonSave_Click(object sender, EventArgs e)
        {
            this.EPMain.Clear();

            if (string.IsNullOrWhiteSpace(this.flatTextBoxFolder.Text))
            {
                __Set_ErrorProvider(this.flatTextBoxFolder, 4, Properties.Resources.ErrPrvTextEmpty);
                return;
            }

            if (!string.IsNullOrWhiteSpace(this.flatTextBoxServer.Text))
            {
                flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.SetupServer =
                    this.flatTextBoxServer.Text
                ));
            }
            else
            {
                __Set_ErrorProvider(this.flatTextBoxServer, 4, Properties.Resources.ErrPrvTextEmpty);
                return;
            }

            if (Properties.Settings.Default.SetupServerType)
            {
                if (!string.IsNullOrWhiteSpace(this.flatTextBoxServerFTPLogin.Text))
                {
                    flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.SetupServerFTPLogin =
                        this.flatTextBoxServerFTPLogin.Text
                    ));
                }
                else
                {
                    __Set_ErrorProvider(this.flatTextBoxServerFTPLogin, 4, Properties.Resources.ErrPrvTextEmpty);
                    return;
                }

                if (!string.IsNullOrWhiteSpace(this.flatTextBoxServerFTPPass.Text))
                {
                    flatThreadSafe.Run(this, (Action)(() => Properties.Settings.Default.SetupServerFTPPass =
                        this.flatTextBoxServerFTPPass.Text
                    ));
                }
                else
                {
                    __Set_ErrorProvider(this.flatTextBoxServerFTPPass, 4, Properties.Resources.ErrPrvTextEmpty);
                    return;
                }
            }
            __Set_StatusBar(Properties.Resources.SetupSave);
            Properties.Settings.Default.Save();
            stRv.EngineSet(
                Properties.Settings.Default.SetupServerType,
                Properties.Settings.Default.SetupServer,
                Properties.Settings.Default.SetupServerFTPLogin,
                Properties.Settings.Default.SetupServerFTPPass
            );
            this.timerSrvInfo_Tick(sender, e);
        }

        private void flatButtonStartStop_Click(object sender, EventArgs e)
        {
            if (!__SetupFolder_Check())
            {
                __SetupFolder_Text(__ButtonState.Setup);
                this.flatTabControlMain.SelectTab(1);
                this.pictureBoxFolder.Focus();
                return;
            }
            if (this.fileSystemWatch.EnableRaisingEvents)
            {
                __Set_AlertMsg(FlatAlertBox._Kind.Success, Properties.Resources.LogManagerStop);
                __SetupFolder_Text(__ButtonState.Start);
                flatThreadSafe.Run(this, (Action)(() => __isRaisingEvents = false));
            }
            else
            {
                __Set_AlertMsg(FlatAlertBox._Kind.Success, Properties.Resources.LogManagerStart);
                __SetupFolder_Text(__ButtonState.Stop);
                flatThreadSafe.Run(this, (Action)(() =>
                    this.fileSystemWatch.Path = Path.GetDirectoryName(Properties.Settings.Default.SetupFolder)
                ));
                flatThreadSafe.Run(this, (Action)(() =>
                    this.fileSystemWatch.NotifyFilter = NotifyFilters.FileName
                ));
                __isRaisingEvents = true;
            }
            __FileWatcher_Run(__isRaisingEvents);
        }

        #region Timer Drop function

        private void flatButtonTimerDrop_Click(object sender, EventArgs e)
        {
            if (this.timerDrop.Enabled)
            {
                return;
            }
            if ((__TimerDropMin = this.flatNumericTimerDrop.Value) <= 0)
            {
                __Set_AlertMsg(FlatAlertBox._Kind.Error, Properties.Resources.AlertDropError1);
                return;
            }

            Task<Engine.State> t1 = stRv.BeginBlockTimerT(__TimerDropMin);
            t1.ContinueWith((o) =>
            {
                Engine.State st = o.Result;
                switch (st)
                {
                    case Engine.State.ListSuccess:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Success,
                                Properties.Resources.AlertDropError3
                            );
                            break;
                        }
                    case Engine.State.ServerBusy:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertDropError2
                            );
                            return;
                        }
                    case Engine.State.ServerError:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertErrorServer
                            );
                            return;
                        }
                    case Engine.State.ListError:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                Properties.Resources.AlertErrorLocal
                            );
                            return;
                        }
                    default:
                        {
                            __Set_AlertMsg(
                                FlatAlertBox._Kind.Error,
                                string.Format(
                                    Properties.Resources.AlertOtherError,
                                    st.ToString()
                                )
                            );
                            return;
                        }
                }
                __SetupFolder_Text(__ButtonState.Desabled);
                flatThreadSafe.Run(this, (Action)(() => __isRaisingEvents = this.fileSystemWatch.EnableRaisingEvents));
                __Set_LogMsg(string.Format(Properties.Resources.fmtStatusDropStart, __TimerDropMin));
                flatThreadSafe.Run(this, (Action)(() => __TimerDropMin = (__TimerDropMin * 60)));
                flatThreadSafe.Run(this, (Action)(() => this.timerDrop.Enabled = true));
                flatThreadSafe.Run(this, (Action)(() => this.flatButtonTimerDrop.Enabled = false));
                flatThreadSafe.Run(this, (Action)(() => this.flatButtonStartStop.Enabled = false));
                __FileWatcher_Run(false);
                o.Dispose();
            }, TaskContinuationOptions.NotOnFaulted | TaskContinuationOptions.NotOnCanceled);
            t1.ContinueWith((o) =>
            {
                __Set_LogMsg(
                    string.Format(
                        Properties.Resources.LogTaskError,
                        o.Status.ToString()
                    )
                );
                o.Dispose();
            }, TaskContinuationOptions.OnlyOnFaulted);
        }

        private void timerDrop_Tick(object sender, EventArgs e)
        {
            if (__TimerDropMin <= 0)
            {
                flatThreadSafe.Run(this, (Action)(() => this.timerDrop.Enabled = false));
                flatThreadSafe.Run(this, (Action)(() => this.flatButtonStartStop.Enabled = true));
                flatThreadSafe.Run(this, (Action)(() => this.flatButtonTimerDrop.Text = Properties.Resources.ButtonTimerDrop));
                flatThreadSafe.Run(this, (Action)(() => this.flatButtonTimerDrop.Enabled = true));
                this.flatButtonTimerDrop.Refresh();

                if (!__SetupFolder_Check())
                {
                    __SetupFolder_Text(__ButtonState.Setup);
                }
                else
                {
                    __SetupFolder_Text(
                        ((__isRaisingEvents) ? __ButtonState.Stop : __ButtonState.Start)
                    );
                    __FileWatcher_Run(__isRaisingEvents);
                }
                return;
            }
            TimeSpan t = TimeSpan.FromSeconds(__TimerDropMin);
            flatThreadSafe.Run(this, (Action)(() =>
                this.flatButtonTimerDrop.Text = string.Format("{0:D2}:{1:D2}:{2:D2}", t.Hours, t.Minutes, t.Seconds)
            ));
            this.flatButtonTimerDrop.Refresh();
            __TimerDropMin--;
        }

        #endregion

        #region NotifyIcon function

        private void flatMinimize_Click(object sender, EventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => this.Hide()));
            flatThreadSafe.Run(this, (Action)(() => this.ShowInTaskbar = false));
            flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.Visible = true));
            flatThreadSafe.Run(this, (Action)(() =>
                this.NotifyMain.Text = string.Format(
                    "{0} - {1}", this.formSkinMain.Text, DateTime.Now.ToShortTimeString()
                )
            ));
        }

        private void Manager_FormClosing(object sender, FormClosingEventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.Visible = false));
        }

        private void NotifyMain_Click(object sender, EventArgs e)
        {
            flatThreadSafe.Run(this, (Action)(() => this.WindowState = FormWindowState.Normal));
            flatThreadSafe.Run(this, (Action)(() => this.MaximizeBox = false));
            flatThreadSafe.Run(this, (Action)(() => this.ShowInTaskbar = true));
            flatThreadSafe.Run(this, (Action)(() => this.NotifyMain.Visible = false));
            flatThreadSafe.Run(this, (Action)(() => this.Show()));
            // this.Refresh();
        }

        #endregion

        #region Link Url launcher

        private void __LinkUrlClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            ProcessStartInfo pi;

            if (e.Link.LinkData != null)
            {
                pi = new ProcessStartInfo(e.Link.LinkData.ToString());
            }
            else
            {
                LinkLabel ll = sender as LinkLabel;
                pi = new ProcessStartInfo(ll.Text.Substring(e.Link.Start, e.Link.Length));
                ll.LinkVisited = false;
            }
            try
            {
                Process.Start(pi);
            }
            catch (Exception ex)
            {
                __Set_StatusBar(ex.Message);
            }
        }

        #endregion
    }
}
