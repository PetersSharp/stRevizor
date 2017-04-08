
#region COPYRIGHT (c) metastruct, 27 Mar 2013
//  Simple C# FTP Class
//  https://metastruct.wordpress.com/2012/08/20/easy-c-ftp-class/
//  Easy to use class for downloading, uploading, and other FTP commands.
//  https://www.codeproject.com/Tips/443588/Simple-Csharp-FTP-Class
#endregion

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;

namespace stRevizor
{
    public class FTPClient
    {
        private string _srvName = null;
        private string _srvLogin = null;
        private string _srvPwd = null;
        private FtpWebRequest ftpRequest = null;
        private FtpWebResponse ftpResponse = null;
        private Stream ftpStream = null;
        private int bufferSize = 2048;

        /* Construct Object */
        public FTPClient(string srvName, string srvLogin, string srvPwd)
        {
            IPAddress ipa;
            if ((ipa = stXFTPUtil.GetIPAddress(srvName)) == null)
            {
                throw new XFTPException(502, Properties.Resources.txtXFTPBadIpAddress);
            }
            _srvName = string.Format("ftp://{0}/", srvName);
            _srvLogin = srvLogin;
            _srvPwd = srvPwd;
        }

        /* Download File */
        public void Get(string remoteFile, string localFile)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + remoteFile);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.DownloadFile;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Get the FTP Server's Response Stream */
                ftpStream = ftpResponse.GetResponseStream();
                /* Open a File Stream to Write the Downloaded File */
                FileStream localFileStream = new FileStream(localFile, FileMode.Create);
                /* Buffer for the Downloaded Data */
                byte[] byteBuffer = new byte[bufferSize];
                int bytesRead = ftpStream.Read(byteBuffer, 0, bufferSize);
                /* Download the File by Writing the Buffered Data Until the Transfer is Complete */
                try
                {
                    while (bytesRead > 0)
                    {
                        localFileStream.Write(byteBuffer, 0, bytesRead);
                        bytesRead = ftpStream.Read(byteBuffer, 0, bufferSize);
                    }
                }
                catch (Exception e)
                {
                    throw new XFTPException(500, e.Message);
                }
                /* Resource Cleanup */
                localFileStream.Close();
                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
            }
            catch (System.Net.WebException e)
            {
                
                if (e.Response != null)
                {
                    System.Net.FtpWebResponse fwr = e.Response as System.Net.FtpWebResponse;
                    if ((fwr != null) && (fwr.StatusCode == System.Net.FtpStatusCode.ActionNotTakenFileUnavailable))
                    {
                        throw new XFTPException(1, e.Message);
                    }
                }
                throw new XFTPException(550, e.Message);
            }
            catch (Exception e)
            {
                throw new XFTPException(501, e.Message);
            }
            return;
        }

        /* Upload File */
        public void Put(string remoteFile, string localFile)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + remoteFile);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.UploadFile;
                /* Establish Return Communication with the FTP Server */
                ftpStream = ftpRequest.GetRequestStream();
                /* Open a File Stream to Read the File for Upload */
                FileStream localFileStream = new FileStream(localFile, FileMode.Open);
                /* Buffer for the Downloaded Data */
                byte[] byteBuffer = new byte[bufferSize];
                int bytesSent = localFileStream.Read(byteBuffer, 0, bufferSize);
                /* Upload the File by Sending the Buffered Data Until the Transfer is Complete */
                try
                {
                    while (bytesSent != 0)
                    {
                        ftpStream.Write(byteBuffer, 0, bytesSent);
                        bytesSent = localFileStream.Read(byteBuffer, 0, bufferSize);
                    }
                }
                catch (Exception e)
                {
                    throw new XFTPException(502, e.Message);
                }
                /* Resource Cleanup */
                localFileStream.Close();
                ftpStream.Close();
                ftpRequest = null;
            }
            catch (Exception e)
            {
                throw new XFTPException(503, e.Message);
            }
            return;
        }

        /* Delete File */
        public void Delete(string deleteFile)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)WebRequest.Create(_srvName + deleteFile);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.DeleteFile;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Resource Cleanup */
                ftpResponse.Close();
                ftpRequest = null;
            }
            catch (Exception e)
            {
                throw new XFTPException(504, e.Message);
            }
            return;
        }

        /* Rename File */
        public void Rename(string currentFileNameAndPath, string newFileName)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)WebRequest.Create(_srvName + currentFileNameAndPath);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.Rename;
                /* Rename the File */
                ftpRequest.RenameTo = newFileName;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Resource Cleanup */
                ftpResponse.Close();
                ftpRequest = null;
            }
            catch (Exception e)
            {
                throw new XFTPException(505, e.Message);
            }
            return;
        }

        /* Create a New Directory on the FTP Server */
        public void CreateDirectory(string newDirectory)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)WebRequest.Create(_srvName + newDirectory);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.MakeDirectory;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Resource Cleanup */
                ftpResponse.Close();
                ftpRequest = null;
            }
            catch (Exception e)
            {
                throw new XFTPException(506, e.Message);
            }
            return;
        }

        /* Get the Date/Time a File was Created */
        public string GetFileDateTime(string fileName)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + fileName);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.GetDateTimestamp;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Establish Return Communication with the FTP Server */
                ftpStream = ftpResponse.GetResponseStream();
                /* Get the FTP Server's Response Stream */
                StreamReader ftpReader = new StreamReader(ftpStream);
                /* Store the Raw Response */
                string fileInfo = null;
                /* Read the Full Response Stream */
                try
                {
                    fileInfo = ftpReader.ReadToEnd();
                }
                catch (Exception e)
                {
                    throw new XFTPException(507, e.Message);
                }
                /* Resource Cleanup */
                ftpReader.Close();
                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
                /* Return File Created Date Time */
                return fileInfo;
            }
            catch (Exception e)
            {
                throw new XFTPException(508, e.Message);
            }
            /* Return an Empty string Array if an Exception Occurs */
            //return "";
        }

        /* Get the Size of a File */
        public string GetFileSize(string fileName)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + fileName);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.GetFileSize;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Establish Return Communication with the FTP Server */
                ftpStream = ftpResponse.GetResponseStream();
                /* Get the FTP Server's Response Stream */
                StreamReader ftpReader = new StreamReader(ftpStream);
                /* Store the Raw Response */
                string fileInfo = null;
                /* Read the Full Response Stream */
                try
                {
                    while (ftpReader.Peek() != -1) { fileInfo = ftpReader.ReadToEnd(); }
                }
                catch (Exception e)
                {
                    throw new XFTPException(509, e.Message);
                }
                /* Resource Cleanup */
                ftpReader.Close();
                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
                /* Return File Size */
                return fileInfo;
            }
            catch (Exception e)
            {
                throw new XFTPException(510, e.Message);
            }
            /* Return an Empty string Array if an Exception Occurs */
            //return "";
        }

        /* Get the Size of a File, format output */
        public string GetFileSizeFormat(string fileName)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + fileName);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.GetFileSize;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Establish Return Communication with the FTP Server */
                long fileInfo = ftpResponse.ContentLength;
                /* Resource Cleanup */

                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
                /* Get and return file size */
                return stXFTPUtil.FormatDataSize((double)fileInfo);
            }
            catch (Exception e)
            {
                throw new XFTPException(510, e.Message);
            }
            /* Return an Empty string Array if an Exception Occurs */
            // return "";
        }

        /* List Directory Contents File/Folder Name Only */
        public string[] DirectoryList(string directory)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + directory);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.ListDirectory;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Establish Return Communication with the FTP Server */
                ftpStream = ftpResponse.GetResponseStream();
                /* Get the FTP Server's Response Stream */
                StreamReader ftpReader = new StreamReader(ftpStream);
                /* Store the Raw Response */
                string directoryRaw = null;
                /* Read Each Line of the Response and Append a Pipe to Each Line for Easy Parsing */
                try
                {
                    while (ftpReader.Peek() != -1) { directoryRaw += ftpReader.ReadLine() + "|"; }
                }
                catch (Exception e)
                {
                    throw new XFTPException(511, e.Message);
                }
                /* Resource Cleanup */
                ftpReader.Close();
                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
                /* Return the Directory Listing as a string Array by Parsing 'directoryRaw' with the Delimiter you Append (I use | in This Example) */
                try
                {
                    string[] directoryList = directoryRaw.Split("|".ToCharArray());
                    return directoryList;
                }
                catch (Exception e)
                {
                    throw new XFTPException(512, e.Message);
                }
            }
            catch (Exception e)
            {
                throw new XFTPException(513, e.Message);
            }
            /* Return an Empty string Array if an Exception Occurs */
            // return new string[] { "" };
        }

        /* List Directory Contents in Detail (Name, Size, Created, etc.) */
        public string[] DirectoryListDetailed(string directory)
        {
            try
            {
                /* Create an FTP Request */
                ftpRequest = (FtpWebRequest)FtpWebRequest.Create(_srvName + directory);
                /* Log in to the FTP Server with the User Name and Password Provided */
                ftpRequest.Credentials = new NetworkCredential(_srvLogin, _srvPwd);
                /* When in doubt, use these options */
                ftpRequest.UseBinary = true;
                ftpRequest.UsePassive = true;
                ftpRequest.KeepAlive = true;
                /* Specify the Type of FTP Request */
                ftpRequest.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                /* Establish Return Communication with the FTP Server */
                ftpResponse = (FtpWebResponse)ftpRequest.GetResponse();
                /* Establish Return Communication with the FTP Server */
                ftpStream = ftpResponse.GetResponseStream();
                /* Get the FTP Server's Response Stream */
                StreamReader ftpReader = new StreamReader(ftpStream);
                /* Store the Raw Response */
                string directoryRaw = null;
                /* Read Each Line of the Response and Append a Pipe to Each Line for Easy Parsing */
                try
                {
                    while (ftpReader.Peek() != -1) { directoryRaw += ftpReader.ReadLine() + "|"; }
                }
                catch (Exception e)
                {
                    throw new XFTPException(514, e.Message);
                }
                /* Resource Cleanup */
                ftpReader.Close();
                ftpStream.Close();
                ftpResponse.Close();
                ftpRequest = null;
                /* Return the Directory Listing as a string Array by Parsing 'directoryRaw' with the Delimiter you Append (I use | in This Example) */
                try
                {
                    string[] directoryList = directoryRaw.Split("|".ToCharArray());
                    return directoryList;
                }
                catch (Exception e)
                {
                    throw new XFTPException(515, e.Message);
                }
            }
            catch (Exception e)
            {
                throw new XFTPException(516, e.Message);
            }
            /* Return an Empty string Array if an Exception Occurs */
            //return new string[] { "" };
        }
    } 
}
