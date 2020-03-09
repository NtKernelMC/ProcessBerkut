#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#include <stdio.h>
#include <string>
#include <Psapi.h>
#include <TlHelp32.h>
#include "ProcessBerkut.h"
using namespace std;
int __cdecl main()
{
	SafeLaunch::ProcessGate procGate(CreateProcessW);
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo; 
	if (procGate.SafeProcess<const wchar_t*, LPSTARTUPINFOW>
	(L"TestClient.exe", L"", NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
	{
		printf("Process Created!\n");
	}
	else printf("%d\n", GetLastError());
	system("pause");
	return EXIT_SUCCESS;
}