//
// Program.cs
//
// Author:
//       Alexander Bothe <info@alexanderbothe.com>
//
// Copyright (c) 2014 Alexander Bothe
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#define P2
using System.Threading.Tasks;
using System;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace CunoTTY
{
	public class Program
	{
		public static void Main(string[] args)
		{
			var port1 = new SerialPort ("/dev/ttyUSB0", 38400);
			#if P2
			var port2 = new SerialPort ("/dev/ttyUSB1", 38400);
			#endif
			var enc = Encoding.ASCII;

			port1.Encoding = enc;
			#if P2
			port2.Encoding = enc;
			port2.Open ();
			#endif

			port1.Open ();

			var sr1 = new StreamReader (port1.BaseStream, enc);
			var sw1 = new StreamWriter(port1.BaseStream, enc) { AutoFlush = false };

			// Init reception processes
			sw1.Write("\"t1\r\n");
			sw1.Flush ();

			#if P2
			var sr2 = new StreamReader (port2.BaseStream, enc);
			var sw2 = new StreamWriter (port2.BaseStream, enc) { AutoFlush = false };
			sw2.Write("\"t1\r\n");
			sw2.Flush ();
			#endif

			// Send message
			string msgToSend = "DerpDerp";



			var task = Task.Factory.StartNew (() => {
				char c;
				while(port1.IsOpen)
				{
					Console.Write("sr1: ");
					c = (char)sr1.Read();
					switch(c)
					{
						case '\r':
						case '\n':
							Console.WriteLine();
							break;
						default:
							Console.Write(c);
							break;
					}
					Console.WriteLine();
				}
				Console.WriteLine();
				Console.WriteLine("sr1 finished");
			});

			#if P2
			var task2 = Task.Factory.StartNew (() => {
				char c;
				while(port2.IsOpen)
				{
					Console.Write("sr2: ");
					c = (char)sr2.Read();
					switch(c)
					{
						case '\r':
						case '\n':
							Console.WriteLine();
							break;
						default:
							Console.Write(c);
							break;
					}
				}
				Console.WriteLine();
				Console.WriteLine("sr2 finished");
			});
			#endif


			// Wait for its reception
			Console.WriteLine ("Press <Return> to exit. Press S to send string.");
			for (;;) {
				var uinp = Console.ReadLine ();
				if (string.IsNullOrWhiteSpace (uinp))
					break;
				if (uinp.StartsWith ("s")) {
					sw1.Write ("\"s");
					sw1.Write ((byte)msgToSend.Length);
					sw1.Write ('Q');
					sw1.Write ("\r\n");

					sw1.Flush ();
				}
			}

			port1.Close ();
			#if P2
			port2.Close ();
			#endif
		}
	}
}

