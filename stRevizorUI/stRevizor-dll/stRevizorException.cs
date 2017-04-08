using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace stRevizor
{
    [Serializable]
    public class RevEmptySourceException : Exception
    {
        public RevEmptySourceException() { }
        public RevEmptySourceException(string message) : base(message) { }
        public RevEmptySourceException(string message, Exception inner) : base(message, inner) { }
        protected RevEmptySourceException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
