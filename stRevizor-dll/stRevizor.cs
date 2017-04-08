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
        private const string _tftpDnsFileName = "dnslist.fw";
        private const string _tftpPortFileName = "portlist.fw";
        private const string _tftpPortIpFileName = "portiplist.fw";
        private const string _tftpWhiteFileName   = "whitelist.fw";
        private const string _tftpBusyProcFileName = "iplist.proccess";
        private const string _tftpEndUploadFileName = "iplist.complete";
        private const string _localIpMatch = "127.0.0.";
        private const string _gwIpMatch = "0.0.0.0";
        private string _rootPath = String.Empty;
        private bool _dnsListEnable = true;
        private bool _urlListEnable = false;
        private bool _portListEnable = false;
        private long _aggregateNet = 0;
        private Int32 _pbCounter = 0;

        private RegisterDump _RegDump = null;
        private TFTPClient _tftp = null;
        private Action<int> _progressBar = (x) => {};
        private List<string> _urlList = null;
        private List<string> _ipList = null;
        private List<string> _whiteList = null;
        private List<string> _dnsList = null;
        private List<string> _portList = null;
        private List<string> _portIpList = null;

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
        public string PortFileName
        {
            get { return _tftpPortFileName; }
        }
        /// <summary>
        /// 
        /// </summary>
        public string PortIpFileName
        {
            get { return _tftpPortIpFileName; }
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
        /// 
        /// </summary>
        public Int32 DnsCount
        {
            get
            {
                return ((this._dnsList == null) ? 0 : this._dnsList.Count);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Int32 PortCount
        {
            get
            {
                return ((this._portIpList == null) ? 0 : this._portIpList.Count);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Int32 PortIpCount
        {
            get
            {
                return ((this._portIpList == null) ? 0 : this._portIpList.Count);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public Int32 WhiteCount
        {
            get
            {
                return ((this._whiteList == null) ? 0 : this._whiteList.Count);
            }
        }

        /// <summary>
        /// Enable Port based list to iptables layer 7 support
        /// iptables -A INPUT -p tcp -d {IP} --dport {PORT} -m state --state New -j DROP
        /// </summary>
        public bool DnsListEnable
        {
            get { return this._dnsListEnable; }
            set { this._dnsListEnable = value; }
        }
        /// <summary>
        /// Enable Port based list to iptables layer 7 support
        /// iptables -A INPUT -p tcp -d {IP} --dport {PORT} -m state --state New -j DROP
        /// </summary>
        public bool PortListEnable
        {
            get { return this._portListEnable; }
            set { this._portListEnable = value; }
        }
        /// <summary>
        /// Enable Url list to iptables layer 7 support
        /// iptables -A INPUT -p tcp --dport 80 -m string --string "{url pattern}" -j DROP
        /// </summary>
        public bool UrlListEnable
        {
            get { return this._urlListEnable; }
            set { this._urlListEnable = ((_portListEnable) ? false : value); }
        }
        /// <summary>
        /// Aggregate Ip to network /24 /16 /8
        /// valid number: 0, 24, 16, 8
        /// </summary>
        public long AggregateNet
        {
            get { return this._aggregateNet; }
            set { this._aggregateNet = ((_portListEnable) ? 0 : value); }
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
        public List<string> DnsList
        {
            get { return this._dnsList; }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<string> PortList
        {
            get { return this._portList; }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<string> PortIpList
        {
            get { return this._portIpList; }
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
        public Revizor(bool isUrlEnable, bool isPortListEnable, string tftpIp, string rootPath = null)
        {
            this.op = AsyncOperationManager.CreateOperation(null);
            this.RootPath = rootPath;
            this._tftp = new TFTPClient(tftpIp);
            this._portListEnable = isPortListEnable;
            this._urlListEnable = ((isPortListEnable) ? false : isUrlEnable);
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
                if (_portList == null)
                {
                    _portList = new List<string>();
                }
                if (_portIpList == null)
                {
                    _portIpList = new List<string>();
                }
                if (_dnsList == null)
                {
                    _dnsList = new List<string>();
                }
                if (_RegDump != null)
                {
                    _RegDump.Dispose();
                    _RegDump = null;
                }
                _ipList.Clear();
                _urlList.Clear();
                _dnsList.Clear();
                _portList.Clear();
                _portIpList.Clear();
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
                    // Add to DNS lists
                    if ((_dnsListEnable) && (dmncnt))
                    {
                        foreach (string domain in item.domain)
                        {
                            if (!_dnsList.Contains(domain))
                            {
                                _dnsList.Add(domain);
                            }
                        }
                    }
                    // Add to URL lists
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
                    // Add to Port and PortIp lists
                    else if ((_portListEnable) && (urlcnt))
                    {
                        foreach (string url in item.url)
                        {
                            Uri curl;
                            if (Uri.TryCreate(url, UriKind.Absolute, out curl))
                            {
                                if (ipcnt)
                                {
                                    foreach (string ip in item.ip)
                                    {
                                        this._setIpPortList(curl.Host, ip, curl.Port);
                                    }
                                }
                                else
                                {
                                    this._setIpPortList(curl.Host, null, curl.Port);
                                }
                            }
                        }
                    }
                    // Add to Ip lists
                    else if (ipcnt)
                    {
                        foreach (string ip in item.ip)
                        {
                            string ipsub = this.AggregateToNet(ip, this._aggregateNet);
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

        /// <summary>
        /// Aggregate Ip to Network
        /// valid mask: /24, /16, /8
        /// </summary>
        /// <param name="ip"></param>
        /// <param name="idx"></param>
        /// <returns></returns>
        public string AggregateToNet(string ip, long idx)
        {
            try
            {
                switch (idx)
                {
                    case 24:
                        {
                            return string.Concat(
                                this._aggregateToNet(ip),
                                ".0/24"
                            );
                        }
                    case 16:
                        {
                            return string.Concat(
                                this._aggregateToNet(
                                    this._aggregateToNet(ip)
                                ),
                                ".0.0/16"
                            );
                        }
                    case 8:
                        {
                            return string.Concat(
                                this._aggregateToNet(
                                    this._aggregateToNet(
                                        this._aggregateToNet(ip)
                                    )
                                ),
                                ".0.0.0/8"
                            );
                        }
                    default:
                        {
                            return ip;
                        }
                }
            }
            catch (Exception e)
            {
                this.Fire_ProcessError(new RevErrorEventArgs(e, MethodBase.GetCurrentMethod().Name));
                return String.Empty;
            }
        }

        #region Save to file..
        /// <summary>
        /// SaveToFile DnsList
        /// </summary>
        /// <param name="outpath"></param>
        public void SaveToFileDnsList(string outpath = null)
        {
            this._SaveToFile(_dnsList, _tftpDnsFileName, outpath);
        }
        /// <summary>
        /// SaveAsFile DnsList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveAsFileDnsList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpDnsFileName, outpath);
        }
        /// <summary>
        /// SaveToFile IpList
        /// </summary>
        /// <param name="outpath"></param>
        public void SaveToFileIpList(string outpath = null)
        {
            this._SaveToFile(_ipList, _tftpIpFileName, outpath);
        }
        /// <summary>
        /// SaveAsFile IpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveAsFileIpList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpIpFileName, outpath);
        }
        /// <summary>
        /// SaveToFile UrlList
        /// </summary>
        /// <param name="outpath"></param>
        public void SaveToFileUrlList(string outpath = null)
        {
            this._SaveToFile(_urlList, _tftpUrlFileName, outpath);
        }
        /// <summary>
        /// SaveAsFile UrlList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveAsFileUrlList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpUrlFileName, outpath);
        }
        /// <summary>
        /// SaveToFile PortIpList
        /// </summary>
        /// <param name="outpath0"></param>
        /// <param name="outpath1"></param>
        public void SaveToFilePortIpList(string outpath0 = null, string outpath1 = null)
        {
            if ((PortCount > 0) && (PortIpCount > 0))
            {
                this._SaveToFile(_portList, _tftpPortFileName, outpath0);
                this._SaveToFile(_portIpList, _tftpPortIpFileName, outpath1);
            }
        }
        /// <summary>
        /// SaveAsFile UrlList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SaveAsFileWhiteList(List<string> srcList, string outpath = null)
        {
            this._SaveToFile(srcList, _tftpWhiteFileName, outpath);
        }
        /// <summary>
        /// SaveToFile WhiteList
        /// </summary>
        /// <param name="outpath"></param>
        public void SaveToFileWhiteList(string outpath = null)
        {
            this._SaveToFile(this._whiteList, _tftpWhiteFileName, outpath);
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
        /// SendToTftp DnsList
        /// </summary>
        public void SendToTftpDnsList()
        {
            if (IpCount > 0)
            {
                this._SendToTftp(this._dnsList, _tftpDnsFileName, null);
            }
        }
        /// <summary>
        /// SendAsTftp DnsList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpDnsList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpDnsFileName, outpath);
        }
        /// <summary>
        /// SendToTftp IpList
        /// </summary>
        public void SendToTftpIpList()
        {
            if (IpCount > 0)
            {
                this._SendToTftp(this._ipList, _tftpIpFileName, null);
            }
        }
        /// <summary>
        /// SendAsTftp IpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpIpList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpIpFileName, outpath);
        }
        /// <summary>
        /// SendToTftp UrlList
        /// </summary>
        public void SendToTftpUrlList()
        {
            if (UrlCount > 0)
            {
                this._SendToTftp(this._urlList, _tftpUrlFileName, null);
            }
        }
        /// <summary>
        /// SendAsTftp UrlList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpUrlList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpUrlFileName, outpath);
        }
        /// <summary>
        /// SendToTftp PortIpList
        /// </summary>
        public void SendToTftpPortIpList()
        {
            if ((PortCount > 0) && (PortIpCount > 0))
            {
                this._SendToTftp(this._portList, _tftpPortFileName, null);
                this._SendToTftp(this._portIpList, _tftpPortIpFileName, null);
            }
        }
        /// <summary>
        /// SendAsTftp PortIpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpPortList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpPortFileName, outpath);
        }
        /// <summary>
        /// SendAsTftp PortIpList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpPortIpList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpPortIpFileName, outpath);
        }

        /// <summary>
        /// SendToTftp WhiteList
        /// </summary>
        public void SendToTftpWhiteList()
        {
            if ((WhiteCount > 0) && (WhiteCount > 0))
            {
                this._SendToTftp(this._whiteList, _tftpWhiteFileName, null);
            }
        }
        /// <summary>
        /// SendToTftp WhiteList
        /// </summary>
        /// <param name="srcList"></param>
        /// <param name="outpath"></param>
        public void SendAsTftpWhiteList(List<string> srcList, string outpath = null)
        {
            this._SendToTftp(srcList, _tftpWhiteFileName, outpath);
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
                File.AppendAllText(Path.Combine("data", "IpListError.log"), domain + " - " + e.Message + Environment.NewLine);
            }
            return (item.ip.Count > 0);
        }
        private void _setIpPortList(string domain, string ip, Int32 port)
        {
            try
            {
                string Host = String.Empty, Port = port.ToString();

                if (!_portList.Contains(Port))
                {
                    _portList.Add(Port);
                }
                if (string.IsNullOrWhiteSpace(ip))
                {
                    IPAddress[] addrlst = Dns.GetHostAddresses(domain);
                    foreach (IPAddress addr in addrlst)
                    {
                        if (!_portIpList.Contains((Host = string.Format(@"{0}|{1}", addr.ToString(), Port))))
                        {
                            _portIpList.Add(Host);
                        }
                    }
                }
                else
                {
                    if (!_portIpList.Contains((Host = string.Format(@"{0}|{1}", ip, Port))))
                    {
                        _portIpList.Add(Host);
                    }
                }
            }
            catch (Exception e)
            {
                Exception ex = new Exception(domain + " - " + e.Message);
                this.Fire_ProcessError(new RevErrorEventArgs(ex, MethodBase.GetCurrentMethod().Name));
                File.AppendAllText(Path.Combine("data","IpPortListError.log"), domain + " - " + e.Message + Environment.NewLine);
            }
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
            return ((src.Contains(".")) ? src.Substring(0, src.LastIndexOf('.')) : src);
        }
        #endregion
    }
}
