using System;
using System.IO.Ports;

namespace DhClientInterface
{
	/// <summary>
	/// Manager for serial port data
	/// </summary>
	public class SerialPortManager : IDisposable
	{
		public SerialPortManager(SerialSettings settings)
		{
			_currentSerialSettings = settings;
		}

		~SerialPortManager()
		{
			Dispose(false);
		}

		#region Fields
		private SerialPort _serialPort;
		private SerialSettings _currentSerialSettings = new SerialSettings();
		public event EventHandler<SerialDataEventArgs> NewSerialDataRecieved;

		#endregion

		#region Properties
		/// <summary>
		/// Gets or sets the current serial port settings
		/// </summary>
		public SerialSettings CurrentSerialSettings
		{
			get { return _currentSerialSettings; }
			set { _currentSerialSettings = value; }
		}

		#endregion

		#region Event handlers

		void _serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			int dataLength = _serialPort.BytesToRead;
			byte[] data = new byte[dataLength];
			int nbrDataRead = _serialPort.Read(data, 0, dataLength);
			if (nbrDataRead == 0)
				return;

			// Send data to whom ever interested
			if (NewSerialDataRecieved != null)
				NewSerialDataRecieved(this, new SerialDataEventArgs(data));
		}

		#endregion

		#region Methods

		/// <summary>
		/// Connects to a serial port defined through the current settings
		/// </summary>
		public void StartListening()
		{
			// Closing serial port if it is open
			if (_serialPort != null && _serialPort.IsOpen)
				StopListening();

			// Setting serial port settings
			_serialPort = new SerialPort(
				_currentSerialSettings.PortName,
				_currentSerialSettings.BaudRate,
				_currentSerialSettings.Parity,
				_currentSerialSettings.DataBits,
				_currentSerialSettings.StopBits);

			// Subscribe to event and open serial port for data
			_serialPort.DataReceived += new SerialDataReceivedEventHandler(_serialPort_DataReceived);
			_serialPort.Open();
		}

		/// <summary>
		/// Sends the data to the arduino.
		/// </summary>
		/// <param name="data">Data.</param>
		public void SendData(byte data)
		{
			if (_serialPort == null || !_serialPort.IsOpen) {
				StartListening ();
			}
			byte[] input = new byte[1];
			input [0] = data;
			_serialPort.Write(input, 0, 1);
		}

		/// <summary>
		/// Closes the serial port
		/// </summary>
		public void StopListening()
		{
			_serialPort.Close();
		}

		// Call to release serial port
		public void Dispose()
		{
			Dispose(true);
		}

		// Part of basic design pattern for implementing Dispose
		protected virtual void Dispose(bool disposing)
		{
			if (disposing)
			{
				_serialPort.DataReceived -= new SerialDataReceivedEventHandler(_serialPort_DataReceived);
			}
			// Releasing serial port (and other unmanaged objects)
			if (_serialPort != null)
			{
				if (_serialPort.IsOpen)
					_serialPort.Close();

				_serialPort.Dispose();
			}
		}

		#endregion
	}

	/// <summary>
	/// EventArgs used to send bytes recieved on serial port
	/// </summary>
	public class SerialDataEventArgs : EventArgs
	{
		public SerialDataEventArgs(byte[] dataInByteArray)
		{
			Data = dataInByteArray;
		}

		/// <summary>
		/// Byte array containing data from serial port
		/// </summary>
		public byte[] Data;
	}
}

