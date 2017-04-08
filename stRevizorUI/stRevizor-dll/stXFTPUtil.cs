using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;

namespace stRevizor
{
    /// <summary>
    /// stXFTPUtil
    /// </summary>
    public static class stXFTPUtil
    {
        /// <summary>
        /// stXFTPUtil.GetIPAddress
        /// </summary>
        /// <param name="uri"></param>
        /// <returns></returns>
        public static IPAddress GetIPAddress(string uri)
        {
            try
            {
                return (IPAddress)Dns.GetHostEntry(uri).AddressList[0];
            }
            catch (Exception)
            {
                IPAddress ip = null;
                if (IPAddress.TryParse(uri, out ip))
                {
                    return ip;
                }
                return null;
            }
        }

        public static string GetLocalFileSize(string fpath)
        {
            try
            {
                FileInfo fInfo = new FileInfo(fpath);
                if (!fInfo.Exists)
                {
                    throw new Exception(
                        string.Format(Properties.Resources.fmtFileError, fpath)
                    );
                }
                return __FormatFileSize((double)fInfo.Length);
            }
            catch (Exception e)
            {
                throw e;
            }
        }

        public static string FormatDataSize(double bytes)
        {
            return __FormatFileSize(bytes);
        }

        private static string __FormatFileSize(double bytes)
        {
            const string fmtNum1 = "#,0.0";
            const string fmtNum2 = "#,0";

            if (bytes < 1024)
            {
                return bytes.ToString(fmtNum2);
            }
            bytes /= 1024;

            if (bytes < 1024)
            {
                return bytes.ToString(fmtNum1) + " KB";
            }
            bytes /= 1024;

            if (bytes < 1024)
            {
                return bytes.ToString(fmtNum1) + " MB";
            }
            bytes /= 1024;

            if (bytes < 1024)
            {
                return bytes.ToString(fmtNum1) + " GB";
            }
            bytes /= 1024;
            return bytes.ToString(fmtNum1) + " TB";
        }

    }
}
