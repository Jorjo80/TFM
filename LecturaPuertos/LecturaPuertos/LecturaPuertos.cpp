#include <Windows.h>
#include <iostream>


void apertura_puerto()
{
	HANDLE m_hComm;

	m_hComm = CreateFile((LPCWSTR)"COM11", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	                     FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hComm == (HANDLE)-1)
	{
		printf("Error abriendo puerto serie 11");
	}
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
