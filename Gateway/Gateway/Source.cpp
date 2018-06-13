#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#include "HeaderGateway.h"


#define PIPE_NAME TEXT("\\\\.\\pipe\\gateway")
#define N 10
#define TAM 250

HANDLE hCli[10];
int continua = 1;

void WINAPI PossivelJogar(HANDLE hPipe) {
	TCHAR buf[256];
	DWORD n;
	while (1) {
		if (mPartilhadaZonaDadosJogo->jogoIniciado == true) {
			wcscpy_s(buf, JOGOJAINICIADO);
			WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
		}
		else if (mPartilhadaZonaDadosJogo->nJogadoresAtivos >= mPartilhadaZonaDadosJogo->nMaxJogadores) {
			wcscpy_s(buf, MAXJOGADORESEXCEDIDO);
			WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
		}
		else {
			wcscpy_s(buf, POSSIVELJOGAR);
			WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
			break;
		}
	}
}



void WINAPI RecebeTeclaCliente(HANDLE hPipe) {
	//HANDLE hPipe;
	TCHAR buf[256];
	DWORD n;
	TCHAR *nome;
	HANDLE GuardarJogadorBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GuardarJogador, hPipe, 0, NULL);
	WaitForSingleObject(GuardarJogadorBuffer, INFINITE);

	HANDLE EscreverMsgBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EscreverMsg, hPipe, 0, NULL);
	WaitForSingleObject(EscreverMsgBuffer, INFINITE);

	/*BOOL ret = ReadFile(hPipe, buf, sizeof(buf), &n, NULL);
	buf[n / sizeof(TCHAR)] = '\0';
	if (!ret || !n)
	break;
	nome = getNomeJogador(hPipe);

	_tprintf(TEXT("[%s] Recebi %d bytes: '%s'... (ReadFile)\n"), nome, n, buf);*/
	//aqui vai ter avisar o servidor com mensagens
}

DWORD WINAPI RecebeCliente() {
	int i;
	HANDLE hPipe;
	HANDLE hFile;
	BOOL bFile;
	DWORD n;
	TCHAR buf[256];

	while (1) {
		//Cria��o do pipe entre o gatway e os clientes
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
			sizeof(buf), sizeof(buf), 0, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe!"));
			exit(-1);
		}
		_tprintf(TEXT("[GATEWAY] Handle do pipe criado\n"));

		//Espera que um cliente se junte ao jogo
		if (ConnectNamedPipe(hPipe, NULL) != FALSE) {
			HANDLE possivelJogar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PossivelJogar, hPipe, 0, NULL);
			WaitForSingleObject(possivelJogar, INFINITE);
			HANDLE TrataPedidosCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeTeclaCliente, hPipe, 0, NULL);
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

	//Comunica��o entre o cliente e o gateway (NamedPipes)
	HANDLE comunicacaoCliGW = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, NULL, 0, NULL);

	while (!mPartilhadaZonaDadosJogo->jogoIniciado) {
		/*_tprintf(TEXT("."));
		Sleep(1000);*/
	}
	/*
	//Atende clientes enquanto o servidor n�o lan�ar o jogo
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