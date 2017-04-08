﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Net;
using System.Reflection;
using stRevizor;
using stRevizor.Data;

namespace stRevizorTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Revizor rv = new Revizor(
                true,
                "192.168.220.66",
                Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)
            );
            rv.OnProcessError += (o, e) =>
            {
                Console.WriteLine(
                    "Error: "
                    + Environment.NewLine + " Type:    " + e.ex.GetType().Name
                    + Environment.NewLine + " Method:  " + e.MethodName
                    + Environment.NewLine + " Message: " + e.ex.Message
                );
            };
            // iplist.proccess
            //if (rv.ParseXml("dump.xml"))

            if (rv.TestRemoteDataBusy())
            {
                Console.WriteLine("* TestRemoteDataBusy: Yes");
            }
            Console.WriteLine("* End: ");
            //Console.ReadLine();
            /*
            if (rv.ParseZip("register.zip"))
            {
                List<string> ipList = rv.IpList;
                List<string> urlList = rv.UrlList;
                //
                rv.SaveToFileIpList(ipList, "dump-ip.out");
                rv.SaveToFileUrlList(urlList, "dump-url.out");
                //
                rv.SendToTftpIpList(ipList);
                rv.SendToTftpUrlList(urlList);
                //
                Console.WriteLine("* End: " + ipList.Count + " : " + urlList.Count);
            }
             */
            Console.ReadLine();
        }


    }
}