using System;
using System.IO.Ports;
using System.Threading.Tasks;
using System.ComponentModel;

namespace DhClientInterface
{
	/// <summary>
	/// Class containing properties related to a serial port 
	/// </summary>
	public class SerialSettings
	{
		string _portName = "";
		int _baudRate = 4800;
		Parity _parity = Parity.None;
		int _dataBits = 8;
		StopBits _stopBits = StopBits.One;

		#region Properties
		/// <summary>
		/// The port to use (for example, COM1).
		/// </summary>
		public string PortName
		{
			get { return _portName; }
			set
			{
				if (!_portName.Equals(value))
				{
					_portName = value;
				}
			}
		}
		/// <summary>
		/// The baud rate.
		/// </summary>
		public int BaudRate
		{
			get { return _baudRate; }
			set
			{
				if (_baudRate != value)
				{
					_baudRate = value;
				}
			}
		}

		/// <summary>
		/// One of the Parity values.
		/// </summary>
		public Parity Parity
		{
			get { return _parity; }
			set
			{
				if (_parity != value)
				{
					_parity = value;
				}
			}
		}
		/// <summary>
		/// The data bits value.
		/// </summary>
		public int DataBits
		{
			get { return _dataBits; }
			set
			{
				if (_dataBits != value)
				{
					_dataBits = value;
				}
			}
		}
		/// <summary>
		/// One of the StopBits values.
		/// </summary>
		public StopBits StopBits
		{
			get { return _stopBits; }
			set
			{
				if (_stopBits != value)
				{
					_stopBits = value;
				}
			}
		}

		#endregion
	}
}

