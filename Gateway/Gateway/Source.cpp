#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#include "HeaderGateway.h"



#define N 10
#define TAM 250
BOOL fConnected = false;
HANDLE hCli[10];
HANDLE hThread;
DWORD dwThreadId = 0;
int continua = 1;



DWORD WINAPI RecebeCliente() {
	int i;
	HANDLE hPipe;
	HANDLE hFile;
	BOOL bFile;
	DWORD n;
	TCHAR buf[256];

	while (1) {
		//Criação do pipe entre o gatway e os clientes
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
			sizeof(buf), sizeof(buf), 0, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe!"));
			exit(-1);
		}
		_tprintf(TEXT("[GATEWAY] Handle do pipe criado\n"));

		fConnected = ConnectNamedPipe(hPipe, NULL);
		if (fConnected) {
			hThread = CreateThread(NULL, 0, ThreadAtendeCliente, (LPVOID)hPipe, 0, &dwThreadId);
		}


		Sleep(10000);
	}
	/*DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	Sleep(200);*/

	/*do {
	//GUARDAR HANDLE NA TABELA CLIENTES...
	/*for (i = 0; i < N; i++) {
	if (hCli[i] == INVALID_HANDLE_VALUE)
	break;

	}
	if (i < 10) {
	hCli[i] = hPipe;
	}

	CloseHandle(hFile);*/
	//HANDLE AtendeCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)guardaCliente, NULL, 0, NULL);
	//Avisa o servidor do sucedido
	/*HANDLE EscreverBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EscreverMsg, NULL, 0, NULL);
	WaitForSingleObject(EscreverBuffer, INFINITE);
	//system("pause");
	} while (!mPartilhadaZonaDadosJogo->jogoIniciado);*/
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

	//Comunicação entre o cliente e o gateway (NamedPipes)
	HANDLE comunicacaoCliGW = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, NULL, 0, NULL);

	

	while (!mPartilhadaZonaDadosJogo->jogoIniciado) {
		/*_tprintf(TEXT("."));
		Sleep(1000);*/
	}
	/*
	//Atende clientes enquanto o servidor não lançar o jogo
	while (!mPartilhadaZonaDadosJogo->jogoIniciado) {
	int jogAtivos = mPartilhadaZonaDadosJogo->nJogadoresAtivos;
	if (jogAtivos >= N) {
	_tprintf(LIMITEADICIONARCLIENTE);
	}
	hCli[jogAtivos] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, (LPVOID)id, 0, NULL);
	WaitForMultipleObjects(1, &hCli[0], FALSE, INFINITE);
	}
	*/

	//WaitForSingleObject(hThreadCliente, INFINITE);

	return 0;

}