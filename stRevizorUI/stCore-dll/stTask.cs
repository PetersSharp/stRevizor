using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace stCore
{
    public static class stTask
    {
        public static void RunTask<T>(Func<T> _beginCb, Action _endCb, Action<string> _logCb)
        {
            Task<T> t1 = CreateTask<T>(_beginCb, _logCb);
            EndTask<T>(t1, _endCb, _logCb);
        }
        public static Task<T> CreateTask<T>(Func<T> _beginCb, Action<string> _logCb)
        {
            if ((_beginCb == null) && (_logCb != null))
            {
                _logCb(
                    __LogString("stCore.stTask.CreateTask(..) Fail", "Function beginCb is null, Abort")
                );
                return default(Task<T>);
            }
            return Task<T>.Factory.StartNew(() =>
            {
                return (T)_beginCb();
            });
        }
        public static void EndTask<T>(Task<T> _task, Action _endCb, Action<string> _logCb)
        {
            if (_task == null)
            {
                if (_logCb != null)
                {
                    _logCb(
                        __LogString("stCore.stTask.EndTask(..) Fail", "Task is null, Abort")
                    );
                }
                return;
            }
            _task.ContinueWith((o) =>
            {
                T aout = (T)o.Result;
                if ((_endCb != null) && (aout != null))
                {
                    _endCb();
                }
                o.Dispose();
            }, TaskContinuationOptions.NotOnFaulted | TaskContinuationOptions.NotOnCanceled);
            _task.ContinueWith((o) =>
            {
                if (_logCb != null)
                {
                    _logCb(
                        __LogString(
                            o.Status.ToString(),
                            ((o.Exception != null) ? o.Exception.Message : String.Empty)
                        )
                    );
                }
                o.Dispose();
            }, TaskContinuationOptions.OnlyOnFaulted);
        }

        private static string __LogString(string status, string exmsg)
        {
            return string.Format(
                "- {0}, {1}",
                status,
                ((exmsg == String.Empty) ? "-" : exmsg)
            );

        }
    }
}