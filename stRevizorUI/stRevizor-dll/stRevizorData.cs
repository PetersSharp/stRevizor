﻿
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
using System.Collections.Generic;

namespace stRevizor.Data
{
    [Serializable]
    public class RegisterDump : IDisposable
    {
        /// <summary>
        /// 
        /// </summary>
        public List<ItemRegisterDump> Items { get; set; }
        /// <summary>
        /// 
        /// </summary>
        public String UpdateTime { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public RegisterDump()
        {
            this.Items = new List<ItemRegisterDump>();
            this.UpdateTime = String.Empty;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="UpdateTime"></param>
        /// <param name="Items"></param>
        public RegisterDump(String UpdateTime, List<ItemRegisterDump> Items)
        {
            this.Items = Items;
            this.UpdateTime = UpdateTime;
        }
        ~RegisterDump()
        {
            this.Dispose();
        }
        public void Dispose()
        {
            if (this.Items != null)
            {
                this.Items.ForEach(o =>
                {
                    o.domain.Clear();
                    o.url.Clear();
                    o.ip.Clear();
                });
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public RegisterDump Clone()
        {
            return (RegisterDump)this.MemberwiseClone();
        }
    }
    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class ItemRegisterDump
    {
        public String id { get; set; }
        public String includeTime { get; set; }

        public String date { get; set; }
        public String number { get; set; }
        public String org { get; set; }

        public List<String> url { get; set; }
        public List<String> domain { get; set; }
        public List<String> ip { get; set; }

        public ItemRegisterDump()
        {
            id = String.Empty;
            includeTime = String.Empty;

            date = String.Empty;
            number = String.Empty;
            org = String.Empty;

            url = new List<string>();
            domain = new List<string>();
            ip = new List<string>();
        }
    }

    #region Event: public class ErrorEventArgs (EventArgs)
    /// <summary>
    /// Event Arguments Error
    /// </summary>
    [Serializable]
    public class RevErrorEventArgs : EventArgs
    {
        /// <summary>
        /// Exception
        /// </summary>
        public Exception ex { get; internal set; }
        /// <summary>
        /// string method Name (or another string)
        /// </summary>
        public string MethodName { get; internal set; }
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="ex">Exception <see cref="System.Exception"/></param>
        public RevErrorEventArgs(Exception ex, string mname = null)
        {
            this.ex = ex;
            this.MethodName = mname;
        }
    }
    #endregion


}
