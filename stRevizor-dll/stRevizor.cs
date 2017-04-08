using System;
using System.Collections.Generic;
using System.Xml;
using stRevizor.Data;
using stRevizor.TFTP;
using System.IO;
using System.Net;
using System.ComponentModel;
using System.Reflection;
using stCore;

namespace stRevizor
{
    public class Revizor
    {
        #region Variables

        private const string _localDataDirectory = "data";
        private const string _inZipFileName = "dump.xml";
        private const string _tftpIpFileName = "iplist.fw";
        private const string _tftpUrlFileName = "urllist.fw";
        private const string _tftpWhiteFileName = "whitelist.fw";
        private const string _tftpBusyProcFileName = "iplist.proccess";
        private const string _tftpEndUploadFileName = "iplist.complete";
        private const string _localIpMatch = "127.0.0.";
        private const string _gwIpMatch = "0.0.0.0";
        private string _rootPath = String.Empty;
        private bool _urlListEnable = false;
        private long _aggregateNet = 0;
        private Int32 _pbCounter = 0;

        private RegisterDump _RegDump = null;
        private TFTPClient _tftp = null;
        private Action<int> _progressBar = (x) => {};
        private List<string> _urlList = null;
        private List<string> _ipList = null;
        private List<string> _whiteList = null;

        private List<string> _whiteSourceList = new List<string>()
        {
            "n01.rfc-revizor.ru",
            "n02.rfc-revizor.ru",
            "n03.rfc-revizor.ru",
            "ns.google.com",
            "google.com",
            "google.ru",
            "yandex.ru",
            "ya.ru"
        };

        /// <summary>
        /// 
        /// </summary>
        public string IpFileName
        {
            get { return _tftpIpFileName; }
        }
        /// <summary>
        /// 
        /// </summary>
        public string UrlFileName
        {
            get { return _tftpUrlFileName; }
        }
        /// <summary>
        /// 
        /// </summary>
        public string UpdateTime
        {
            get {
                return ((this._RegDump == null) ? "" : this._RegDump.UpdateTime);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Int32 IpCount
        {
            get
            {
                return ((this._ipList == null) ? 0 : this._ipList.Count);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Int32 UrlCount
        {
            get
            {
                return ((this._urlList == null) ? 0 : this._urlList.Count);
            }
        }

        /// <summary>
        /// Enable Url list to iptables layer 7 support
        /// iptables -A INPUT -p tcp --dport 80 -m string --string "{url pattern}" -j DROP
        /// </summary>
        public bool UrlListEnable
        {
            get { return this._urlListEnable; }
            set { this._urlListEnable = value; }
        }
        /// <summary>
        /// Aggregate Ip to network /24 /16 /8
        /// valid number: 0, 24, 16, 8
        /// </summary>
        public long AggregateNet
        {
            get { return this._aggregateNet; }
            set { this._aggregateNet = value; }
        }
        //_aggregateEnable
        /// <summary>
        /// 
        /// </summary>
        public string RootPath
        {
            get { return this._rootPath; }
            set { this._rootPath = value; }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<string> IpList
        {
            get { return this._ipList; }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<string> UrlList
        {
            get { return this._urlList; }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<string> WhiteList
        {
            get { return this._whiteList; }
        }
        /// <summary>
        /// 
        /// </summary>
        public RegisterDump RegDump
        {
            get { return this._RegDump; }
            set { this._RegDump = value; }
        }
        /// <summary>
        /// 
        /// </summary>
        public Action<int> ProgressBar
        {
            get { return this._progressBar; }
            set { this._progressBar = value; }
        }
        #endregion

        #region Events
        /// <summary>
        /// Event Wiki Error, return <see cref="WikiEngine.WikiErrorEventArgs"/>WikiEngine.WikiErrorEventArgs
        /// </summary>
        public event EventHandler<RevErrorEventArgs> OnProcessError = delegate { };

        private AsyncOperation op;

        private void Fire_ProcessError(RevErrorEventArgs o)
        {
            op.Post(x => OnProcessError(this, (RevErrorEventArgs)x), o);
        }
        #endregion

        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="tftpIp"></param>
        /// <param name="isurlListEnable"></param>
        /// <param name="rootPath"></param>
        public Revizor(bool isUrlEnable, string tftpIp, string rootPath = null)
        {
            this.op = AsyncOperationManager.CreateOperation(null);
            this.RootPath = rootPath;
            this._tftp = new TFTPClient(tftpIp);
            this._urlListEnable = isUrlEnable;
            this._whiteList = new List<string>();
            this._whiteSourceList.ForEach(o =>
            {
                try
                {
                    IPAddress[] addrlst = Dns.GetHostAddresses(o);
                    foreach (IPAddress addr in addrlst)
                    {
                        this._whiteList.Add(addr.ToString());
                    }
                }
                catch (Exception e)
                {
                    this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                }
            });
        }
        #endregion

        #region Public method
        public bool ParseZip(string path)
        {
            stCore.stZipStorer zip = null;
            this._pbCounter = 0;

            try
            {
                if (!File.Exists(path))
                {
                    Exception ex = new Exception(Properties.Resources.txtZipNotFound);
                    this.Fire_ProcessError(new RevErrorEventArgs(ex, MethodBase.GetCurrentMethod().Name));
                    return false;
                }
                zip = stZipStorer.Open(path, System.IO.FileAccess.Read);
                foreach (stZipStorer.ZipFileEntry entry in (List<stZipStorer.ZipFileEntry>)zip.ReadCentralDir())
                {
                    //this._progressBar(this._pbCounter++);
                    if (Path.GetFileName(entry.FilenameInZip).Equals(_inZipFileName))
                    {
                        string opath = Path.Combine(
                            _rootPath,
                            _localDataDirectory,
                            _inZipFileName
                        );
                        if (!Directory.Exists(Path.GetDirectoryName(opath)))
                        {
                            Directory.CreateDirectory(Path.GetDirectoryName(opath));
                        }
                        if (File.Exists(opath))
                        {
                            File.Delete(opath);
                        }
                        zip.ExtractFile(entry, opath);
                        return ParseXml(opath, this._pbCounter);
                    }
                }
                Exception e = new Exception(Properties.Resources.txtZipEntryNotFound);
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
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
        /// <summary>
        /// Parse
        /// </summary>
        /// <param name="xmlpath"></param>
        /// <returns></returns>
        public bool ParseXml(string xmlpath, int cnt = 0)
        {
            this._pbCounter = cnt;

            try
            {
                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.Load(xmlpath);

                if (_ipList == null)
                {
                    _ipList = new List<string>();
                }
                if (_urlList == null)
                {
                    _urlList = new List<string>();
                }
                if (_RegDump != null)
                {
                    _RegDump.Dispose();
                    _RegDump = null;
                }
                _ipList.Clear();
                _urlList.Clear();
                _RegDump = new RegisterDump();
                _RegDump.UpdateTime = xmlDoc.GetElementsByTagName("reg:register")[0].Attributes.GetNamedItem("updateTime").InnerText;
                XmlNodeList content = xmlDoc.GetElementsByTagName("content");

                Int32 i = 0, offset = this._progressBarOffset(content.Count, cnt);

                for (; i < content.Count; i++)
                {
                    if ((i % offset) == 0)
                    {
                        this._progressBar(this._pbCounter++);
                    }

                    ItemRegisterDump item = new ItemRegisterDump();

                    item.id = content[i].Attributes.GetNamedItem("id").InnerText;
                    item.includeTime = content[i].Attributes.GetNamedItem("includeTime").InnerText;

                    foreach (XmlNode node in content[i].ChildNodes)
                    {
                        switch (node.Name)
                        {
                            case "decision":
                                {
                                    item.date = node.Attributes.GetNamedItem("date").InnerText;
                                    item.number = node.Attributes.GetNamedItem("number").InnerText;
                                    item.org = node.Attributes.GetNamedItem("org").InnerText;
                                    break;
                                }
                            case "url":
                                {
                                    item.url.Add(node.InnerText);
                                    break;
                                }
                            case "domain":
                                {
                                    item.domain.Add(node.InnerText);
                                    break;
                                }
                            case "ip":
                                {
                                    item.ip.Add(node.InnerText);
                                    break;
                                }
                        }
                    }
                    _RegDump.Items.Add(item);
                }

                offset = this._progressBarOffset(_RegDump.Items.Count, cnt);

                foreach (ItemRegisterDump item in _RegDump.Items)
                {
                    if ((i % offset) == 0)
                    {
                        this._progressBar(this._pbCounter++);
                    }
                    i++;

                    bool ipcnt = (item.ip.Count > 0),
                        urlcnt = (item.url.Count > 0),
                        dmncnt = (item.domain.Count > 0);

                    if (!ipcnt)
                    {
                        if (
                            (dmncnt) &&
                            ((!_urlListEnable) || ((_urlListEnable) && (!urlcnt)))
                           )
                        {
                            foreach (string domain in item.domain)
                            {
                                ipcnt = this._getIpFromDomain(item, domain);
                            }
                        }
                        else if ((!_urlListEnable) && (urlcnt))
                        {
                            foreach (string url in item.url)
                            {
                                ipcnt = this._getIpFromDomain(item, new Uri(url).Host);
                            }
                        }
                    }
                    /// Add to lists
                    if ((_urlListEnable) && (urlcnt))
                    {
                        foreach (string url in item.url)
                        {
                            if (!_ipList.Contains(url))
                            {
                                _urlList.Add(url);
                            }
                        }
                    }
                    else if (ipcnt)
                    {
                        foreach (string ip in item.ip)
                        {
                            string ipsub;
                            switch (this._aggregateNet)
                            {
                                case 24:
                                    {
                                        ipsub = string.Concat(
                                            this._aggregateToNet(ip),
                                            ".0/24"
                                        );
                                        break;
                                    }
                                case 16:
                                    {
                                        ipsub = string.Concat(
                                            this._aggregateToNet(
                                                this._aggregateToNet(ip)
                                            ),
                                            ".0.0/16"
                                        );
                                        break;
                                    }
                                case 8:
                                    {
                                        ipsub = string.Concat(
                                            this._aggregateToNet(
                                                this._aggregateToNet(
                                                    this._aggregateToNet(ip)
                                                )
                                            ),
                                            ".0.0.0/8"
                                        );
                                        break;
                                    }
                                default:
                                    {
                                        ipsub = ip;
                                        break;
                                    }
                            }
                            if (
                                (!_ipList.Contains(ipsub)) &&
                                (!ip.StartsWith(_localIpMatch)) &&
                                (!ip.Equals(_gwIpMatch))
                               )
                            {
                                _ipList.Add(ipsub);
                            }
                        }
                    }
                }
                return true;
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return false;
            }
            finally
            {
                this._pbCounter = 0;
            }
        }

        #region Save to file..
        /// <summary>
        /// SaveToFile IpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveToFileIpList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpIpFileName, outpath);
        }
        /// <summary>
        /// SaveToFile UrlList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveToFileUrlList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpUrlFileName, outpath);
        }
        /// <summary>
        /// SaveToFile WhiteList
        /// </summary>
        public void SaveToFileWhiteList()
        {
            this._SaveToFile(this._whiteList, _tftpWhiteFileName, null);
        }
        #endregion

        #region Send to ftp/tftp..
        /// <summary>
        /// Test remote flag data busy
        /// get file 'iplist.proccess'
        /// </summary>
        /// <returns></returns>
        public bool TestRemoteDataBusy()
        {
            bool ret = true;
            string tmpFname = String.Empty;

            try
            {
                tmpFname = this._outFileName(_tftpBusyProcFileName, null);
                this._tftp.Get(_tftpBusyProcFileName, tmpFname);
            }
            catch (TFTP.TFTPClient.TFTPException e)
            {
                if (e.ErrorCode == 1)
                {
                    ret = false;
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
            this._delTmpFileName(tmpFname);
            return ret;
        }
        /// <summary>
        /// Set remote flag upload complete
        /// send file 'iplist.complete'
        /// </summary>
        public void SendToTftpUploadComplete()
        {
            string tmpFname = String.Empty;
            try
            {
                tmpFname = this._outFileName(_tftpBusyProcFileName, null);
                File.WriteAllText(tmpFname, DateTime.Now.ToString());
                this._tftp.Put(_tftpEndUploadFileName, tmpFname);
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
            this._delTmpFileName(tmpFname);
        }
        /// <summary>
        /// SendToTftp IpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendToTftpIpList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpIpFileName, outpath);
        }
        /// <summary>
        /// SendToTftp UrlList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendToTftpUrlList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpUrlFileName, outpath);
        }
        /// <summary>
        /// SendToTftp WhiteList
        /// </summary>
        public void SendToTftpWhiteList()
        {
            this._SendToTftp(this._whiteList, _tftpWhiteFileName, null);
        }
        #endregion

        #endregion

        #region Private method

        private void _SaveToFile(List<string> srcList, string tftpname, string outpath)
        {
            if ((srcList == null) || (srcList.Count == 0))
            {
                Exception e = new Exception(Properties.Resources.txtIpListEmpty);
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return;
            }
            string opath = ((string.IsNullOrWhiteSpace(outpath)) ?
                Path.Combine(
                    _rootPath,
                    _localDataDirectory,
                    tftpname
                ) :
                this._outFileName(tftpname, outpath)
            );
            using (StreamWriter wr = new StreamWriter(opath))
            {
                wr.NewLine = "\n";
                srcList.ForEach(o =>
                {
                    wr.WriteLine(o);
                });
            }
        }
        private void _SendToTftp(List<string> srcList, string tftpname, string outpath)
        {
            if ((srcList == null) || (srcList.Count == 0))
            {
                Exception e = new Exception(Properties.Resources.txtIpListEmpty);
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return;
            }
            try
            {
                bool isTmpFile = (outpath == null);

                if (isTmpFile)
                {
                    outpath = this._outFileName(tftpname, outpath);
                }
                if (!File.Exists(outpath))
                {
                    this._SaveToFile(srcList, tftpname, outpath);
                }
                this._tftp.Put(tftpname, outpath);
                if (isTmpFile)
                {
                    File.Delete(outpath);
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
        }
        private bool _getIpFromDomain(ItemRegisterDump item, string domain)
        {
            try
            {
                IPAddress[] addrlst = Dns.GetHostAddresses(domain);
                foreach (IPAddress addr in addrlst)
                {
                    item.ip.Add(addr.ToString());
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
            return (item.ip.Count > 0);
        }
        private string _outFileName(string tftpname, string outpath)
        {
            return ((string.IsNullOrWhiteSpace(outpath)) ?
                Path.ChangeExtension(Path.GetTempFileName(), Guid.NewGuid().ToString() + Path.GetFileNameWithoutExtension(tftpname)) :
                outpath
            );
        }
        private void _delTmpFileName(string tmpFname)
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
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
            }
        }
        private Int32 _progressBarOffset(Int32 count, Int32 oldOffset)
        {
            return (Int32)(count / (50 - oldOffset));
        }
        private string _aggregateToNet(string src)
        {
            return src.Substring(0, src.LastIndexOf('.'));
        }
        #endregion
    }
}
