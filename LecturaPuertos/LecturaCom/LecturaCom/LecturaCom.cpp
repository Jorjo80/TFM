// LecturaCom.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include "H:\Jorge\UPM\master\TFM\serialport\SerialPort.h"
#include "H:\Jorge\UPM\master\TFM\serialport\stdafx.h"


VOID WINAPI CompletionRoutine(_In_ DWORD dwErrorCode, _In_ DWORD dwNumberOfBytesTransfered, _Inout_ LPOVERLAPPED lpOverlapped) noexcept
{
	UNREFERENCED_PARAMETER(dwErrorCode);
	UNREFERENCED_PARAMETER(dwNumberOfBytesTransfered);
	UNREFERENCED_PARAMETER(lpOverlapped);
}

int main()
{
	std::vector<BYTE> buf;
	buf.resize(10000);
	for (auto& b : buf)
		b = '%';
	HANDLE hEvent = nullptr;
	constexpr const int nPortToUse = 11;

	try
	{
		COMMCONFIG config;
		CSerialPort::GetDefaultConfig(nPortToUse, config);

		CSerialPort port;
		port.Open(nPortToUse, 115200, CSerialPort::Parity::NoParity, 8, CSerialPort::StopBits::OneStopBit, CSerialPort::FlowControl::NoFlowControl);
		HANDLE hPort = port.Detach();
		port.Attach(hPort);

		DWORD dwModemStatus;
		port.GetModemStatus(dwModemStatus);

		DCB dcb;
		port.GetState(dcb);
	}
	catch(CSerialException& e)
	{

	}

    std::cout << "Hello World!\n";

	_mm_pause();
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
