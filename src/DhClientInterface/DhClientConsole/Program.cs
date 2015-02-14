using System;
using DhClientInterface;

namespace DhClientConsole
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			SerialPortManager manager = new SerialPortManager(new SerialSettings
				{
					BaudRate = 57600,
					DataBits = 8,
					Parity = System.IO.Ports.Parity.None,
					PortName = "/dev/cu.usbserial-A6008cJO",//"/dev/cu.usbmodem1411",
					StopBits = System.IO.Ports.StopBits.One
				});

			manager.StartListening ();

			char key = '9';

			do
			{
				Console.WriteLine("ESCOLHA UMA OPCAO:");
				Console.WriteLine("1 - Configurar LED");
				Console.WriteLine("2 - Finalizar Configuração");
				Console.WriteLine("3 - Abrir Contagem");
				Console.WriteLine("4 - Cancelar Contagem");

				key = Console.ReadKey().KeyChar;
				switch (key) {
				case '1':
					manager.SendData(1);
					break;
				case '2':
					manager.SendData(2);
					break;
				case '3':
					manager.SendData(3);
					break;
				case '4':
					manager.SendData(4);
					break;
				default:
					break;
				}

				Console.Clear();
			} while (key != '9');
		}
	}
}
