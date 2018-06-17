#pragma once

#include "Resource.h"
#define PIPE_NAME TEXT("\\\\.\\pipe\\gateway")
HANDLE hPipe;

HANDLE createPipe() {

	WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER);

	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Ainda não foi lançado o gateway!\n"));
		Sleep(1000);
	}
	return hPipe;
}