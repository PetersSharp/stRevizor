using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace stRevizor
{
    public class InputSorage
    {
        public FileInfo fInfo;
        public string fName;
        public string fPath;

        public InputSorage()
        {
            this.fName = String.Empty;
            this.fPath = String.Empty;
            this.fInfo = null;
        }
        public InputSorage(string fpath, string fname)
        {
            this.fName = fname;
            this.fPath = fpath;
            try
            {
                this.fInfo = new FileInfo(fpath);
            }
            catch { }
        }
    }
}
