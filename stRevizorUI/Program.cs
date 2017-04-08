using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.IO;

namespace RevizorUI
{
    static class Client
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            bool isImport = false;
            string importFilePath = String.Empty;
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            if ((args != null) && (args.Length > 0))
            {
                isImport = true;
                importFilePath = args[0];
            }
            else if (AppDomain.CurrentDomain.SetupInformation.ActivationArguments != null)
            {
                if (
                    (AppDomain.CurrentDomain.SetupInformation.ActivationArguments.ActivationData != null) &&
                    (AppDomain.CurrentDomain.SetupInformation.ActivationArguments.ActivationData.Length > 0)
                   )
                {
#if DEBUG
                    AppDomain.CurrentDomain.SetupInformation.ActivationArguments.ActivationData.ToList().ForEach(o =>
                    {
                        File.AppendAllText("AppDomain.debug.txt", o);
                    });
#endif
                    isImport = true;
                    importFilePath = AppDomain.CurrentDomain.SetupInformation.ActivationArguments.ActivationData[0];
                }
            }
            RevizorClient cl = new RevizorClient(importFilePath, isImport);
            Application.Run(cl);
        }
    }
}
