
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

using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace stRevizor.ServerInfo
{
    [Serializable]
    [XmlRoot(ElementName = "dnsfilter")]
    public class SiDnsfilter
    {
        [XmlElement(ElementName = "enable")]
        public string Enable { get; set; }
        [XmlElement(ElementName = "host")]
        public string Host { get; set; }
        [XmlElement(ElementName = "wild")]
        public string Wild { get; set; }
        [XmlElement(ElementName = "filter")]
        public string Filter { get; set; }
    }

    [Serializable]
    [XmlRoot(ElementName = "urlfilter")]
    public class SiUrlfilter
    {
        [XmlElement(ElementName = "enable")]
        public string Enable { get; set; }
        [XmlElement(ElementName = "port")]
        public string Port { get; set; }
        [XmlElement(ElementName = "host")]
        public string Host { get; set; }
        [XmlElement(ElementName = "uri")]
        public string Uri { get; set; }
        [XmlElement(ElementName = "filter")]
        public string Filter { get; set; }
    }

    [Serializable]
    [XmlRoot(ElementName = "xmlparser")]
    public class SiXmlparser
    {
        [XmlElement(ElementName = "enable")]
        public string Enable { get; set; }
        [XmlElement(ElementName = "zip")]
        public string Zip { get; set; }
    }

    [Serializable]
    [XmlRoot(ElementName = "root")]
    public class SiRoot
    {
        [XmlElement(ElementName = "ipfw")]
        public string Ipfw { get; set; }
        [XmlElement(ElementName = "ipt")]
        public string Ipt { get; set; }
        [XmlElement(ElementName = "ips")]
        public string Ips { get; set; }
        [XmlElement(ElementName = "filter")]
        public string Filter { get; set; }
        [XmlElement(ElementName = "dnsfilter")]
        public SiDnsfilter Dnsfilter { get; set; }
        [XmlElement(ElementName = "urlfilter")]
        public SiUrlfilter Urlfilter { get; set; }
        [XmlElement(ElementName = "xmlparser")]
        public SiXmlparser Xmlparser { get; set; }
        [XmlElement(ElementName = "datelist")]
        public string Datelist { get; set; }
    }
}
