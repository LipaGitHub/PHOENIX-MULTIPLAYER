#pragma once

#include "Resource.h"
#define PIPE_NAME TEXT("\\\\.\\pipe\\gateway")
HANDLE hPipe;
HWND hWndWindow = NULL;
ZonaMsg zMsg;
HANDLE createPipe() {

	WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER);

	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Ainda não foi lançado o gateway!\n"));
		Sleep(1000);
	}
	return hPipe;
}

//THREAD QUE LE O PIPE
DWORD WINAPI threadRead(LPVOID param) {
	HANDLE hWrite = (HANDLE)param;
	DWORD n;
	BOOL ret;

	while (true)
	{
		ret = ReadFile(hWrite, (LPVOID)&zMsg, sizeof(zMsg), &n, NULL);

		if (!ret || !n)
			break;

		;
		if (hWndWindow != NULL)

			InvalidateRect(hWndWindow, NULL, FALSE);
	}

	CloseHandle(hWrite);

	return NULL;
}

void createThread(HANDLE hPipeRead) {
	HANDLE th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadRead, (LPVOID)hPipeRead, 0, NULL);
}

