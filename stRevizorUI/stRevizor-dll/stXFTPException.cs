using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace stRevizor
{
    /// <summary>
    /// A TFTP Exception
    /// </summary>
    public class XFTPException : Exception
    {

        public string ErrorMessage = "";
        public int ErrorCode = -1;

        /// <summary>
        /// Initializes a new instance of the <see cref="XFTPException"/> class.
        /// </summary>
        /// <param name="errCode">The err code.</param>
        /// <param name="errMsg">The err message.</param>
        public XFTPException(int errCode, string errMsg)
        {
            ErrorCode = errCode;
            ErrorMessage = errMsg;
        }

        /// <summary>
        /// Creates and returns a string representation of the current exception.
        /// </summary>
        /// <returns>
        /// A string representation of the current exception.
        /// </returns>
        /// <filterPriority>1</filterPriority>
        /// <permissionSet class="System.Security.permissionSet" version="1">
        /// 	<IPermission class="System.Security.Permissions.FileIOPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" PathDiscovery="*AllFiles*"/>
        /// </permissionSet>
        public override string ToString()
        {
            return String.Format("xFTPException: ErrorCode: {0} Message: {1}", ErrorCode, ErrorMessage);
        }
    }
}
