
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

// #define SHOW_PROGRESSBAR
#define  NET_DEBUG_TO_LOG

using System;
using System.Collections.Generic;
using System.Xml;
using System.Linq;
using System.IO;
using System.Net;
using System.ComponentModel;
using System.Reflection;
using stCore;
using System.Xml.Serialization;
using stRevizor.Data;
using stRevizor.TFTP;
using System.Threading;
using System.Threading.Tasks;

namespace stRevizor
{
    public class Engine
    {
        #region Variables

        public enum State : int
        {
            ListSuccess,
            ListEquals,
            ListNoInit,
            ListNotFound,
            ListFileAccessTimeOut,
            ListErrorZip,
            ListError,
            ServerBusy,
            ServerError
        };

        private const int _fileAccessTimeout = 120;

        private const string _tftpBusyProcFileName = "blocklist.proccess";
        private const string _tftpEndUploadFileName = "blocklist.complete";
        private const string _tftpBlockTimerFileName = "blocklist.timer";
        private const string _tftpAutoconfigFileName = "autoconfig.xml";

        private dynamic _xftp = null;
        private bool _isInit = false;

        private List<InputSorage> _finfo = null;
        /// <summary>
        /// 
        /// </summary>
        public List<InputSorage> FileSorage
        {
            get
            {
                return _finfo.OrderBy(o => ((InputSorage)o).fInfo.CreationTime).DefaultIfEmpty().ToList();
            }
        }

        public InputSorage LastFileSorage
        {
            get
            {
                InputSorage isc = null;
                List<InputSorage> lis = this.FileSorage;

                if ((lis != default(List<InputSorage>)) && (lis.Count == 0))
                {
                    return isc;
                }
                lis.ForEach(o =>
                {
                    if (File.Exists(o.fPath))
                    {
                        isc = o;
                        return;
                    }
                    else
                    {
                        _finfo.Remove(o);
                    }
                });
                return isc;
            }
            set
            {
                if (value == null)
                {
                    _finfo.Clear();
                }
                else
                {
                    _finfo.Add(value);
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="fPath"></param>
        /// <param name="fName"></param>
        public void AddToFileSorage(string fPath, string fName)
        {
            InputSorage fs = new InputSorage(fPath, fName);
            LastFileSorage = fs;
        }

        private ServerInfo.SiRoot _siroot = null;
        /// <summary>
        /// 
        /// </summary>
        public ServerInfo.SiRoot ServerInfo
        {
            get { return _siroot; }
        }

        private bool _srvType = false;
        /// <summary>
        /// 
        /// </summary>
        public bool ServerType
        {
            get { return _srvType; }
        }

        private string _srvName = String.Empty;
        /// <summary>
        /// 
        /// </summary>
        public string ServerName
        {
            get { return _srvName; }
        }

        private string _srvLogin = String.Empty;
        /// <summary>
        /// 
        /// </summary>
        public string ServerLogin
        {
            get { return _srvLogin; }
        }

        private string _srvPwd = String.Empty;
        /// <summary>
        /// 
        /// </summary>
        public string ServerPassword
        {
            get { return _srvPwd; }
        }

        private List<string> _watchList = new List<string>()
        {
            "register.zip",
            "dump.xml"
        };
        /// <summary>
        /// 
        /// </summary>
        public List<string> WatchFileList
        {
            get { return _watchList; }
        }

        private string _rootPath = String.Empty;
        /// <summary>
        /// 
        /// </summary>
        public string RootPath
        {
            get { return this._rootPath; }
            set { this._rootPath = value; }
        }

        private string _updateTime = String.Empty;
        /// <summary>
        /// 
        /// </summary>
        public string UpdateTime
        {
            get { return this._updateTime; }
            set { this._updateTime = value; }
        }

#if SHOW_PROGRESSBAR
        private int _pbCounter = 0;
        private Action<int> _progressBar = (x) => { };
        /// <summary>
        /// 
        /// </summary>
        public Action<int> ProgressBar
        {
            get { return this._progressBar; }
            set { this._progressBar = value; }
        }
#endif

        #endregion

        #region Events
        /// <summary>
        /// Event Error to log
        /// </summary>
        public event EventHandler<RevErrorEventArgs> OnProcessError = delegate { };
        public event EventHandler<RevErrorEventArgs> OnProcessLog = delegate { };

        private AsyncOperation op;

        private void Fire_ProcessError(RevErrorEventArgs o)
        {
            op.Post(x => OnProcessError(this, (RevErrorEventArgs)x), o);
        }
        private void Fire_ProcessLog(RevErrorEventArgs o)
        {
            op.Post(x => OnProcessLog(this, (RevErrorEventArgs)x), o);
        }
        #endregion

        #region Engine Constructor

        /// <summary>
        /// Constructor Engine
        /// </summary>
        /// <param name="rootPath"></param>
        /// <param name="srvType"></param>
        /// <param name="srvIp"></param>
        /// <param name="srvLogin"></param>
        /// <param name="srvPwd"></param>
        public Engine(string rootPath, bool srvType, string srvName, string srvLogin, string srvPwd)
        {
            this.op = AsyncOperationManager.CreateOperation(null);
            _finfo = new List<InputSorage>();
            _rootPath = rootPath;
            _isInit = __InitEngine(srvType, srvName, srvLogin, srvPwd);
        }

        /// <summary>
        /// Set Engine
        /// </summary>
        /// <param name="srvType"></param>
        /// <param name="srvName"></param>
        /// <param name="srvLogin"></param>
        /// <param name="srvPwd"></param>
        public void EngineSet(bool srvType, string srvName, string srvLogin, string srvPwd)
        {
            _isInit = __InitEngine(srvType, srvName, srvLogin, srvPwd);
        }

        private bool __InitEngine(bool srvType, string srvName, string srvLogin, string srvPwd)
        {
            if (string.IsNullOrEmpty(srvName))
            {
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(Properties.Resources.txtServNameEmpty),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
                return false;
            }
            if ((srvType) && (string.IsNullOrEmpty(srvLogin)))
            {
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(Properties.Resources.txtSrvLoginEmpty),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
                return false;
            }
            if ((srvType) && (string.IsNullOrEmpty(srvPwd)))
            {
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(Properties.Resources.txtSrvPwdEmpty),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
                return false;
            }

            string srvIp = String.Empty;
            _srvType = srvType;
            _srvLogin = srvLogin;
            _srvPwd = srvPwd;

            try
            {
                IPAddress[] addrlst = Dns.GetHostAddresses(srvName);
                foreach (IPAddress addr in addrlst)
                {
                    srvIp = addr.ToString();
                    break;
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
            if (!string.IsNullOrWhiteSpace(srvIp))
            {
                _srvName = srvIp;
            }
            else
            {
                _srvName = srvName;
            }
            this.Fire_ProcessLog(
                new RevErrorEventArgs(
                    new Exception(
                        string.Format(
                            Properties.Resources.fmtTestConnection,
                            ((_srvType) ? "FTP" : "TFTP"),
                            _srvName
                        )
                    ),
                    MethodBase.GetCurrentMethod().Name
                )
            );
            try
            {
                if (_srvType)
                {
                    _xftp = new FTPClient(_srvName, _srvLogin, _srvPwd);
                }
                else
                {
                    _xftp = new TFTPClient(_srvName);
                }
                this._siroot = new ServerInfo.SiRoot();
            }
            catch (Exception e)
            {
#if NET_DEBUG_TO_LOG
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
#endif
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
            }
            return true;
        }

        #endregion

        #region Public Task method

        public Task<ServerInfo.SiRoot> ReceiveAutoconfigT()
        {
            return Task<ServerInfo.SiRoot>.Factory.StartNew(() =>
            {
                return ReceiveAutoconfig();
            });
        }
        public Task<State> BeginBlockTimerT(long min)
        {
            return Task<Engine.State>.Factory.StartNew(() =>
            {
                return BeginBlockTimer(min);
            });
        }
        public Task<State> BeginSourceT(string fPath, string fName)
        {
            return Task<Engine.State>.Factory.StartNew(() =>
            {
                return BeginSource(fPath, fName);
            });
        }

        #endregion


        #region Public method

        public ServerInfo.SiRoot ReceiveAutoconfig()
        {
            try
            {
                if (!_isInit)
                {
                    throw new RevEmptySourceException();
                }
                try
                {
                    string outpath = this.__outFileName(_tftpAutoconfigFileName, null);
                    __delTmpFileName(outpath);
                    try
                    {
                        _xftp.Get(_tftpAutoconfigFileName, outpath);
                    }
                    catch (XFTPException e)
                    {
#if NET_DEBUG_TO_LOG
                        this.Fire_ProcessLog(
                            new RevErrorEventArgs(
                                new Exception(
                                    string.Format(
                                        Properties.Resources.fmtXFTPReciveError,
                                        e.ErrorCode,
                                        e.ErrorMessage
                                    )
                                ),
                                MethodBase.GetCurrentMethod().Name
                            )
                        );
#endif
                        throw new RevEmptySourceException();
                    }
#if NET_DEBUG_TO_LOG
                    catch (Exception e)
                    {
                        this.Fire_ProcessLog(
                            new RevErrorEventArgs(
                                new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                                MethodBase.GetCurrentMethod().Name
                            )
                        );
#else
                    catch (Exception)
                    {
#endif
                        this.Fire_ProcessLog(
                            new RevErrorEventArgs(
                                new Exception(Properties.Resources.txtAutoconfigError),
                                MethodBase.GetCurrentMethod().Name
                            )
                        );
                        throw new RevEmptySourceException();
                    }
                    if (File.Exists(outpath))
                    {
                        ServerInfo.SiRoot sr = this.__DeserializeXml<ServerInfo.SiRoot>(outpath);
                        if (sr == null)
                        {
                            throw new RevEmptySourceException();
                        }
                        _siroot = sr;
                        __delTmpFileName(outpath);
                        {
                            _siroot.Ipfw = ((_siroot.Ipfw.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                            _siroot.Ips = ((_siroot.Ips.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                            _siroot.Ipt = ((_siroot.Ipt.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                            _siroot.Dnsfilter.Enable = ((_siroot.Dnsfilter.Enable.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                            _siroot.Urlfilter.Enable = ((_siroot.Urlfilter.Enable.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                            _siroot.Xmlparser.Enable = ((_siroot.Xmlparser.Enable.Equals("1")) ? Properties.Resources.txtSupport : Properties.Resources.txtNotSupport);
                        }
                    }
                }
                catch (RevEmptySourceException e)
                {
                    throw e;
                }
                catch (Exception e)
                {
                    this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                    throw new RevEmptySourceException();
                }
            }
            catch (RevEmptySourceException)
            {
                _siroot = new ServerInfo.SiRoot();
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                _siroot = new ServerInfo.SiRoot();
            }
            return _siroot;
        }

        public State BeginBlockTimer(long min)
        {
            string outpath = String.Empty;

            try
            {
                if (!_isInit)
                {
                    return State.ListNoInit;
                }
                if (!__TestRemoteDataBusy())
                {
                    return State.ServerBusy;
                }

                outpath = this.__outFileName(_tftpBlockTimerFileName, null);
                __delTmpFileName(outpath);
                File.WriteAllText(outpath, min.ToString());

                try
                {
                    _xftp.Put(_tftpBlockTimerFileName, outpath);
                }
#if NET_DEBUG_TO_LOG
                catch (Exception e)
                {
                    this.Fire_ProcessLog(
                       new RevErrorEventArgs(
                           new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                           MethodBase.GetCurrentMethod().Name
                       )
                    );
#else
                catch (Exception)
                {
#endif
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(
                                string.Format(
                                    Properties.Resources.fmtSendToError,
                                    ((_srvType) ? "FTP" : "TFTP")
                                )
                            ),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return State.ServerError;
                }
                __SendToUploadComplete();
                return State.ListSuccess;
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return State.ListError;
            }
            finally
            {
                __delTmpFileName(outpath);
            }
        }

        public State BeginSource(string fPath, string fName)
        {
            int chk = 0;
            bool isZipFile = false;
            string xmlPath = String.Empty;

            try
            {
                if (!_isInit)
                {
                    return State.ListNoInit;
                }
                while (chk < (_fileAccessTimeout + 1))
                {
                    try
                    {
                        if (!File.Exists(fPath))
                        {
                            this.Fire_ProcessLog(
                               new RevErrorEventArgs(
                                   new Exception(
                                       string.Format(
                                            Properties.Resources.fmtFileError,
                                            fName
                                        )
                                   ),
                                   MethodBase.GetCurrentMethod().Name
                               )
                            );
                            return State.ListNotFound;
                        }
                        using (FileStream fs = File.OpenRead(fPath))
                        {
                            this.Fire_ProcessLog(
                               new RevErrorEventArgs(
                                   new Exception(
                                       string.Format(
                                            Properties.Resources.fmtFileOk,
                                            fName
                                        )
                                   ),
                                   MethodBase.GetCurrentMethod().Name
                               )
                            );
                            break;
                        }
                    }
                    catch (Exception e)
                    {
                        if ((chk % 10) == 0)
                        {
                            this.Fire_ProcessLog(
                               new RevErrorEventArgs(
                                   new Exception(
                                       string.Format(
                                            Properties.Resources.fmtFileCheck,
                                            chk,
                                            e.Message
                                        )
                                   ),
                                   MethodBase.GetCurrentMethod().Name
                               )
                            );
                        }
                    }
                    if (chk == _fileAccessTimeout)
                    {
                        this.Fire_ProcessLog(
                           new RevErrorEventArgs(
                               new Exception(
                                   string.Format(
                                        Properties.Resources.fmtFileBusy,
                                        _fileAccessTimeout,
                                        fName
                                    )
                               ),
                               MethodBase.GetCurrentMethod().Name
                           )
                        );
                        return State.ListFileAccessTimeOut;
                    }
                    chk++;
                    Thread.Sleep(1000);
                }

                isZipFile = Path.GetExtension(fPath).Equals(".zip");

                if (isZipFile)
                {
                    try
                    {
                        xmlPath = __ParseZip(fPath);
                        if (string.IsNullOrWhiteSpace(xmlPath))
                        {
                            return State.ListErrorZip;
                        }
                    }
                    catch (Exception e)
                    {
                        this.Fire_ProcessLog(
                           new RevErrorEventArgs(
                               new Exception(
                                   string.Format(
                                        Properties.Resources.fmtZipError,
                                        e.Message
                                    )
                               ),
                               MethodBase.GetCurrentMethod().Name
                           )
                        );
                        return State.ListErrorZip;
                    }
                    if (!File.Exists(xmlPath))
                    {
                        this.Fire_ProcessLog(
                            new RevErrorEventArgs(
                                new Exception(
                                    string.Format(Properties.Resources.fmtZipFileNotFound, xmlPath)
                                ),
                                MethodBase.GetCurrentMethod().Name
                            )
                        );
                        return State.ListNotFound;
                    }
                }
                else
                {
                    xmlPath = fPath;
                }

#if SHOW_PROGRESSBAR
                this._progressBar(_pbCounter++);
#endif

                if (!__CheckXmlDate(xmlPath))
                {
                    return State.ListEquals;
                }

#if SHOW_PROGRESSBAR
                this._progressBar(++_pbCounter);
#endif
                if (!__TestRemoteDataBusy())
                {
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(Properties.Resources.txtServerIsBusy),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return State.ServerBusy;
                }

                try
                {

#if SHOW_PROGRESSBAR
                    this._progressBar(++_pbCounter);
#endif
                    _xftp.Put(fName, fPath);
#if SHOW_PROGRESSBAR
                    this._progressBar((_pbCounter += 30));
#endif
                }
#if NET_DEBUG_TO_LOG
                catch (Exception e)
                {
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
#else
                catch (Exception)
                {
#endif
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(
                                string.Format(
                                    Properties.Resources.fmtSendToError,
                                    ((_srvType) ? "FTP" : "TFTP")
                                )
                            ),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return State.ServerError;
                }
                __SendToUploadComplete();
                return State.ListSuccess;
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return State.ListError;
            }
            finally
            {
                if (isZipFile)
                {
                    __delTmpFileName(xmlPath);
                }

#if SHOW_PROGRESSBAR
                _pbCounter = 0;
                this._progressBar(0);
#endif
            }
        }

        #endregion

        #region Private method

        #region Function communicate tftp/ftp..
        /// <summary>
        /// Test remote flag data busy
        /// get file 'blocklist.proccess'
        /// </summary>
        /// <returns></returns>
        private bool __TestRemoteDataBusy()
        {
            string tmpFname = String.Empty;

            if (!_isInit)
            {
                return false;
            }
            try
            {
                tmpFname = this.__outFileName(_tftpBusyProcFileName, null);
                _xftp.Get(_tftpBusyProcFileName, tmpFname);
                return false;
            }
            catch (XFTPException e)
            {
                if (e.ErrorCode == 1)
                {
                    return true;
                }

#if NET_DEBUG_TO_LOG
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(
                            string.Format(
                                Properties.Resources.fmtXFTPReciveError,
                                e.ErrorCode,
                                e.ErrorMessage
                            )
                        ),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
#endif
                return false;
            }
            catch (Exception e)
            {

#if NET_DEBUG_TO_LOG
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
#endif
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
            }
            finally
            {
                this.__delTmpFileName(tmpFname);
            }
        }

        /// <summary>
        /// Set remote flag upload complete
        /// send file 'blocklist.complete'
        /// </summary>
        private void __SendToUploadComplete()
        {
            if (!_isInit)
            {
                return;
            }
            string tmpFname = String.Empty;
            try
            {
                tmpFname = this.__outFileName(_tftpBusyProcFileName, null);
                File.WriteAllText(tmpFname, DateTime.Now.ToString());
                _xftp.Put(_tftpEndUploadFileName, tmpFname);
            }
            catch (Exception e)
            {
#if NET_DEBUG_TO_LOG
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(string.Format(Properties.Resources.fmtXFTPAllError, e.Message)),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
#endif
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
            finally
            {
                this.__delTmpFileName(tmpFname);
            }
        }

        #endregion

        private string __ParseZip(string fPath)
        {
            stCore.stZipStorer zip = null;

            try
            {
                if (!File.Exists(fPath))
                {
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(Properties.Resources.txtZipNotFound),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return String.Empty;
                }
                zip = stZipStorer.Open(fPath, System.IO.FileAccess.Read);
                foreach (stZipStorer.ZipFileEntry entry in (List<stZipStorer.ZipFileEntry>)zip.ReadCentralDir())
                {

#if SHOW_PROGRESSBAR
                    this._progressBar(++_pbCounter);
#endif

                    if (Path.GetFileName(entry.FilenameInZip).Equals(_watchList[1]))
                    {
                        string opath = Path.Combine(
                            _rootPath,
                            _watchList[1]
                        );
                        if (!Directory.Exists(Path.GetDirectoryName(opath)))
                        {
                            Directory.CreateDirectory(Path.GetDirectoryName(opath));
                        }
                        __delTmpFileName(opath);
                        zip.ExtractFile(entry, opath);
                        return opath;
                    }
                }
                Exception e = new Exception(Properties.Resources.txtZipEntryNotFound);
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return String.Empty;
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return String.Empty;
            }
            finally
            {
                if (zip != null)
                {
                    zip.Close();
                    zip.Dispose();
                }
            }
        }

        private bool __CheckXmlDate(string fPath)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(_siroot.Datelist))
                {
                    return true;
                }

                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.Load(fPath);
                string Datelist = xmlDoc.GetElementsByTagName("reg:register")[0].Attributes.GetNamedItem("updateTimeUrgently").InnerText;

                xmlDoc = null;
                GC.Collect();
                GC.WaitForPendingFinalizers();

                if (string.IsNullOrWhiteSpace(Datelist))
                {
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(Properties.Resources.txtDatelistEmpty),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return false;
                }
                if (_siroot.Datelist.Equals(Datelist))
                {
                    this.Fire_ProcessLog(
                        new RevErrorEventArgs(
                            new Exception(Properties.Resources.txtDatelistEquals),
                            MethodBase.GetCurrentMethod().Name
                        )
                    );
                    return false;
                }
                this.Fire_ProcessLog(
                    new RevErrorEventArgs(
                        new Exception(
                            string.Format(Properties.Resources.fmtDatelistNew, Datelist)
                        ),
                        MethodBase.GetCurrentMethod().Name
                    )
                );
                _updateTime = Datelist;
                return true;
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
            }
        }

        private string __outFileName(string tftpname, string outpath)
        {
            return ((string.IsNullOrWhiteSpace(outpath)) ?
                Path.ChangeExtension(Path.GetTempFileName(), Guid.NewGuid().ToString() + Path.GetFileNameWithoutExtension(tftpname)) :
                outpath
            );
        }

        private void __delTmpFileName(string tmpFname)
        {
            try
            {
                if (
                    (!string.IsNullOrWhiteSpace(tmpFname)) &&
                    (File.Exists(tmpFname))
                   )
                {
                    File.Delete(tmpFname);
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
        }

        private T __DeserializeXml<T>(string xmlpath)
        {
            try
            {
                using (StreamReader stm = new StreamReader(xmlpath))
                {
                    XmlSerializer sr = new XmlSerializer(typeof(ServerInfo.SiRoot));
                    return (T)sr.Deserialize(stm);
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessLog(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return default(T);
            }
        }

        #endregion
    }
}
