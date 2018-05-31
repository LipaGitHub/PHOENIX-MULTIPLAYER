#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\..\DLL\DLL\Estruturas.h"
#include "HeaderGateway.h"


#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
#define N 10
#define TAM 250

HANDLE hCli[10];
int continua = 1;

DWORD WINAPI RecebeCliente(LPVOID p) {
	int i;
	HANDLE hPipe;


	do {
		_tprintf(TEXT("[GATEWAY] Esperar ligação de um CLIENTE...(ConnectNamedPipe)\n"));
		_tprintf(TEXT("[GATEWAY] Criar uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME);

		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
			TAM * sizeof(TCHAR), TAM * sizeof(TCHAR), 0, NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}

		if (!ConnectNamedPipe(hPipe, NULL)) {
			_tprintf(TEXT("[ERRO] Ligação ao CLIENTE! (ConnectNamedPipe\n"));
			exit(-1);
		}
		//GUARDAR HANDLE NA TABELA CLIENTES...
		for (i = 0; i < N; i++) {
			if (hCli[i] == INVALID_HANDLE_VALUE)
				break;

		}
		if (i < 10) {
			hCli[i] = hPipe;
		}

		HANDLE EscreverBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EscreverMsg, NULL, 0, NULL);
		WaitForSingleObject(EscreverBuffer, INFINITE);

	} while (continua);
	return 0;
}

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (IniciarMemoriaMutexSemaforo() == (-1)) {
		return -1;
	}
	//limpar tabela HANDLES
	for (int i = 0; i < N; i++)
		hCli[i] = INVALID_HANDLE_VALUE;
	//CRIAR THREAD T1
	HANDLE hThreadCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, NULL, 0, NULL);

	

	WaitForSingleObject(hThreadCliente, INFINITE);
	
	return 0;

}

