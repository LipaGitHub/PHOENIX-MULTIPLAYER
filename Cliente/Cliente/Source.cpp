#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "..\..\DLL\DLL\Estruturas.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\gateway")
#define PIPE_NAMEWRITE TEXT("\\\\.\\pipe\\gatewayEscrita")
HANDLE PodeAtenderCliente;
ZonaMsg zMsg;
DWORD cbToWrite;
BOOL fConnected = false;
HANDLE hThreadRececao;
DWORD dwThreadId = 0;

void WINAPI atualizaDados(HANDLE hPipe) {
	ZonaMsg zMsgRec;
	DWORD n = sizeof(ZonaMsg);
	BOOL ret = ReadFile(hPipe, &zMsgRec, n, &n, NULL);
	if (!ret || !n) {
		_tprintf(TEXT("Ocorreu algum erro ao receber dados do Gateway!\n"));
		exit(-1);
	}
	_tprintf(TEXT("Li do pipe do Gateway, [NOME]%s"),zMsgRec.nave.nome);
	
}
DWORD WINAPI ThreadAtendeGateway(LPVOID param) {
	HANDLE hPipe = (HANDLE)param;
	HANDLE threadDados = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)atualizaDados, hPipe, 0, NULL);
	WaitForSingleObject(threadDados, INFINITE);
	return 0;
}

DWORD WINAPI RecebeGateway() {
	HANDLE hPipe;
	hPipe = CreateNamedPipe(PIPE_NAMEWRITE, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
		sizeof(ZonaMsg), sizeof(ZonaMsg), 0, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Criar Named Pipe!"));
		exit(-1);
	}
	_tprintf(TEXT("[GATEWAY] Handle do pipe Receção Gateway criado\n"));

	fConnected = ConnectNamedPipe(hPipe, NULL);
	if (fConnected) {
		hThreadRececao = CreateThread(NULL, 0, ThreadAtendeGateway, (LPVOID)hPipe, 0, &dwThreadId);
	}
	WaitForSingleObject(hThreadRececao, INFINITE);
	return 1;
}



void WINAPI VerificaJogo(HANDLE hPipe) {
	TCHAR buf[256];
	DWORD n;

	while (1) {
		//Recebe do gateway o aviso se é possível jogar (servidor ainda não lançou jogo e/ou numero max de jogadores nao alcançado)
		if (ReadFile(hPipe, buf, sizeof(buf), &n, NULL)) {
			buf[n / sizeof(TCHAR)] = '\0';
			if ((_tcscmp(buf, JOGOJAINICIADO)) == 0) {
				_tprintf(TEXT("[JOGO] Viestes tarde! O servidor já lançou o jogo.\n"));
			}
			else if ((_tcscmp(buf, MAXJOGADORESEXCEDIDO)) == 0) {
				_tprintf(TEXT("[JOGO] Número máximo de jogadores excedido.\n"));
			}
			else if ((_tcscmp(buf, POSSIVELJOGAR)) == 0) {
				_tprintf(TEXT("A preparar jogo...\n"));
				HANDLE verificaAtualizacao = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeGateway, NULL, 0, NULL);
				Sleep(3000);
				break;
			}
		}
	}
}

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	HANDLE hPipe;
	DWORD n;
	TCHAR buf[256];
	ZonaMsg cMsg;
	//CHAR *buff;
	TCHAR clienteMsg[] = TEXT("%d", GetCurrentProcessId());
	int op;

	//Verifica a existência do NamedPipe do Gateway
	do {
		hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Ainda não foi lançado o gateway!\n"));
			Sleep(1000);
		}
	} while (hPipe == INVALID_HANDLE_VALUE);

	

	_tprintf(TEXT("Ligação estabelecida com sucesso!\n"));
	
	HANDLE VerificaJogoIniciado = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)VerificaJogo, hPipe, 0, NULL);
	WaitForSingleObject(VerificaJogoIniciado, INFINITE);
	
	_tprintf(TEXT("Introduza um username: "));
	_fgetts(buf, 256, stdin);

	if (!WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL)) {
		_tprintf(TEXT("[ERRO] Escrever no pipe... (WriteFile)\n"));
		exit(-1);
	}
	_tprintf(TEXT("[Cliente] Enviei %d bytes ao gateway\n\n"), n);
	

	while (1)
	{
		do {
			_tprintf(TEXT("1: CIMA\n"));
			_tprintf(TEXT("2: BAIXO\n"));
			_tprintf(TEXT("3: ESQUERDA\n"));
			_tprintf(TEXT("4: DIREITA\n"));
			_tprintf(TEXT("\nOpção>> "));
			//_fgetts(buf, 256, stdin);
			_tscanf_s(TEXT("%d"), &op);

			switch (op) {
			case 1: wcscpy_s(buf, CIMA); break;
			case 2: wcscpy_s(buf, BAIXO); break;
			case 3: wcscpy_s(buf, ESQUERDA); break;
			case 4: wcscpy_s(buf, DIREITA); break;
			}
		
			if (!WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe... (WriteFile)\n"));
				exit(-1);
			}
			_tprintf(TEXT("[Cliente] Enviei %d bytes ao gateway\n"), n);
		} while (_tcscmp(buf, TEXT("fim")));

		_tprintf(TEXT("[Cliente] Vou desligar o pipe(DisconnectNamedPipe/CloseHandle)\n"));
		if (!DisconnectNamedPipe(hPipe)) {
			_tprintf(TEXT("Erro ao desligar o pipe!"));
			exit(-1);
		}

		Sleep(2000);
		_tprintf(TEXT("[Cliente] Vou desligar o pipe... (CloseHandle)\n"));
		CloseHandle(hPipe);
	}
	/*_tprintf(TEXT("TECLA: "));
	//hPipe = CreateFile(PIPE_NAME, PIPE_ACCESS_DUPLEX, 0, NULL, OPEN_EXISTING, 0, NULL);
	/*if (hPipe != INVALID_HANDLE_VALUE) break;
	if (GetLastError() != ERROR_PIPE_BUSY) {
	_tprintf(TEXT("Could not open pipe %d\n"), GetLastError());
	return -1;
	}*/

	//LPTSTR lpvMessage = TEXT("Default message from client.");
	//WriteFile(hPipe, nave, sizeof(nave), , NULL);
	/*_tprintf(TEXT("[CLIENTE] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);
	/*if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
	_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
	exit(-1);
	}*/

	/*_tprintf(TEXT("[CLIENTE] Ligação ao pipe do GATEWAY... (CreateFile)\n"));

	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == NULL) {
	_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);

	exit(-1);
	}
	_tprintf(TEXT("[CLIENTE] Liguei-me...\n"));
	system("pause");*/
}