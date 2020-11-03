#include <Windows.h>
#include <iostream>

HANDLE m_hComm;
unsigned long enviado = 0, enviando = 0, leidos = 0;
uint8_t *buffer; //Guardado de respuestas
uint8_t *cmd;



void apertura_puerto()
{
	m_hComm = CreateFile((LPCWSTR)"COM11", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	                     FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		printf("Error abriendo puerto serie 11");
	}
}

void configurar_puerto()
{
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm, &dcb);
	dcb.BaudRate = CBR_115200;
	dcb.ByteSize = 8;
	dcb.Parity = EVENPARITY;
	dcb.fParity = TRUE;
	dcb.StopBits = ONESTOPBIT;

	dcb.fBinary = TRUE;
	SetCommState(m_hComm, &dcb);
}

void enviodatos()
{
	do
	{
		leidos = sizeof(buffer);
		enviado = 0;
		enviando = 0;
		while(enviado != leidos)
		{
			WriteFile(m_hComm, &(buffer[enviado]), leidos - enviado, &enviando, NULL);
			enviado += enviando;
		}
		
	} while (leidos == 512);
	char eof = 0;
	WriteFile(m_hComm, &eof, 1, &enviando, NULL);
	CloseHandle(m_hComm);
	
}

int main()
{
	std::cout << "Hello World!\n";
	apertura_puerto();
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
