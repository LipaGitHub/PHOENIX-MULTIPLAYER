#pragma once
#include "..\..\DLL\DLL\Estruturas.h"
#define PIPE_NAME TEXT("\\\\.\\pipe\\gateway")
#define PIPE_NAMEWRITE TEXT("\\\\.\\pipe\\gatewayEscrita")
ZonaMsg *mPartilhadaZonaMsg;
DadosJogo *mPartilhadaZonaDadosJogo;

HANDLE PodeEscrever, PodeLer, hMutex, PodeAtenderCliente, hMutexLer, Evento;
TCHAR NomeMemoria[] = TEXT("Memoria Partilhada MSG");
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever"), NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");
TCHAR NomeSemaforoPodeAtenderCliente[] = TEXT("Semáforo Pode Atender Cliente");

char init = 1;
TCHAR cmd[BufferSize];
void WINAPI RecebeTeclaCliente(HANDLE hPipe);
DWORD WINAPI ThreadVerificaMemoria(LPVOID param);

void readTChars(TCHAR * p, int maxchars) {
	int len;
	_fgetts(p, maxchars, stdin);
	len = _tcslen(p);
	if (p[len - 1] == TEXT('\n'))
		p[len - 1] = TEXT('\0');
}

DWORD WINAPI lerMemoria() {
	HANDLE hPipe;
	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] LER MEMORIA!\n"));
		Sleep(1000);
	}
	HANDLE GuardarJogadorBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadVerificaMemoria, hPipe, 0, NULL);
	return 1;
}

void WINAPI PossivelJogar(HANDLE hPipe) {
	TCHAR buf[256];
	DWORD n;
	while (1) {
		/*
		if (mPartilhadaZonaDadosJogo->jogoIniciado == true) {
		wcscpy_s(buf, JOGOJAINICIADO);
		WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
		}
		else*/

		if (mPartilhadaZonaDadosJogo->nJogadoresAtivos >= mPartilhadaZonaDadosJogo->nMaxJogadores) {
			wcscpy_s(buf, MAXJOGADORESEXCEDIDO);
			WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
		}
		else {
			wcscpy_s(buf, POSSIVELJOGAR);
			HANDLE threadLerMemoria = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lerMemoria, NULL, 0, NULL);
			WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
			break;
		}
	}
}


int IniciarMemoriaMutexSemaforo() {
	HANDLE hZonaMsg, hDadosJogo;
	LARGE_INTEGER d, t;

	int pos;
	char init = 1;

	//Criar um MUTEX para acesso ao in(nome= "MUTEX in")
	hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex in"));
	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);
	PodeAtenderCliente = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeAtenderCliente);
	hMutexLer = CreateMutex(NULL, FALSE, TEXT("Mutex Ler"));
	Evento = CreateEvent(NULL, TRUE, FALSE, NULL);

	d.QuadPart = sizeof(ZonaMsg);
	t.QuadPart = sizeof(DadosJogo);
	hZonaMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, d.HighPart, d.LowPart, NomeMemoria);
	hDadosJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, t.HighPart, t.LowPart, TEXT("Memoria Partilhada Dado Jogo"));
	if (PodeEscrever == NULL || PodeLer == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	mPartilhadaZonaMsg = (ZonaMsg*)MapViewOfFile(hZonaMsg, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)d.QuadPart);
	mPartilhadaZonaDadosJogo = (DadosJogo*)MapViewOfFile(hDadosJogo, FILE_MAP_READ, 0, 0, (SIZE_T)t.QuadPart);

	if (mPartilhadaZonaMsg == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		return -1;
	}

	if (init) {
		//Colocar in a zero
		mPartilhadaZonaMsg->in = 0;
		mPartilhadaZonaMsg->out = 0;
		//Restaurar o valor do semaforo PODEESCREVER(+10)
		ReleaseSemaphore(PodeEscrever, 10, NULL);
	}
	return 0;
}

void WINAPI GuardarJogador(HANDLE hPipe) {
	TCHAR buf[25];
	DWORD n;
	int pos;
	if (mPartilhadaZonaDadosJogo->jogoIniciado || !(mPartilhadaZonaDadosJogo->nJogadoresAtivos >= mPartilhadaZonaDadosJogo->nMaxJogadores)) {
		BOOL ret = ReadFile(hPipe, buf, sizeof(buf), &n, NULL);
		buf[n / sizeof(TCHAR)] = '\0';
		if (!ret || !n) {
			_tprintf(TEXT("Ocorreu algum erro ao receber o username do jogador!\n"), n, buf);
		}

		//Jogo ainda não foi lançado pelo Servidor
		WaitForSingleObject(PodeEscrever, INFINITE);
		//Esperar pelo mutex
		WaitForSingleObject(hMutex, INFINITE);
		// Copiar o valor de in para pos = i % Buffers;

		pos = mPartilhadaZonaDadosJogo->nJogadoresAtivos;
		//atualizar valor de in( na memoria partilhada->mPartilhadaZonaMsg)
		mPartilhadaZonaMsg->in = (pos + 1) % Buffers;
		mPartilhadaZonaMsg->mexer = false;
		//Recebe o nome através de NamedPipe do Cliente


		wcscpy_s(mPartilhadaZonaMsg->nave.nome, buf);
		mPartilhadaZonaMsg->nave.tHandle = hPipe;
		_stprintf_s(mPartilhadaZonaMsg->buf[pos], BufferSize, ADICIONARCLIENTE);
		//_tprintf(TEXT("[GW] Recebi %d bytes do cliente '%s'... (ReadFile)\n"), n, buf);


		// libertar o mutex
		ReleaseMutex(hMutex);

		_tprintf(TEXT("[ENVIAR P/ SERVIDOR] %s '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
		Sleep(1000);
		ReleaseSemaphore(PodeLer, 1, NULL);
	}
	else {
		//Iniciado o jogo
		_tprintf(LIMITEADICIONARCLIENTE);
	}
}

TCHAR * getNomeJogador(HANDLE hPipe) {
	for (int i = 0; i < mPartilhadaZonaDadosJogo->nJogadoresAtivos; i++) {
		if (mPartilhadaZonaDadosJogo->nDefensoras[i].tHandle == hPipe) {
			return mPartilhadaZonaDadosJogo->nDefensoras[i].nome;
		}
	}
}

DWORD WINAPI ThreadAtendeCliente(LPVOID param) {
	HANDLE hPipe = (HANDLE)param;
	HANDLE possivelJogar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PossivelJogar, hPipe, 0, NULL);
	WaitForSingleObject(possivelJogar, INFINITE);
	
	HANDLE GuardarJogadorBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GuardarJogador, hPipe, 0, NULL);
	WaitForSingleObject(GuardarJogadorBuffer, INFINITE);
	
	HANDLE TrataPedidosCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeTeclaCliente, hPipe, 0, NULL);

	return 0;
}
DWORD WINAPI ThreadVerificaMemoria(LPVOID param) {
	HANDLE hPipeEscrita = (HANDLE)param;
	DWORD cbToWrite = sizeof(mPartilhadaZonaMsg);
	WaitForSingleObject(PodeLer, INFINITE);
	WaitForSingleObject(hMutex, INFINITE);
	_tprintf(TEXT("\nPassei pelo Evento"));
	for (int i = 0; i < nJogadores; i++) {
		mPartilhadaZonaMsg->dadosCliente.nDefensoras[i] = mPartilhadaZonaDadosJogo->nDefensoras[i];
		mPartilhadaZonaMsg->dadosCliente.nInimigas[i] = mPartilhadaZonaDadosJogo->nInimigas[i];
		mPartilhadaZonaMsg->dadosCliente.nPowerups[i] = mPartilhadaZonaDadosJogo->nPowerups[i];
	}
	
	if (!WriteFile(hPipeEscrita, &mPartilhadaZonaMsg, cbToWrite, &cbToWrite, NULL)) {
		_tprintf(TEXT("[ERRO] Escrever no pipe Escrita... (WriteFile)\n"));
		exit(-1);
	}
	
	ReleaseMutex(hMutex);
	ReleaseSemaphore(PodeEscrever, 1, NULL);
	return 0;
}
DWORD WINAPI EscreverMsg(HANDLE hPipe) {
	TCHAR buf[256];
	DWORD n;
	TCHAR *nome;
	int pos;
	while (1) {
		if (!mPartilhadaZonaDadosJogo->jogoIniciado) {
			//Jogo ainda nao foi lançado pelo Servidor

			BOOL ret = ReadFile(hPipe, buf, sizeof(buf), &n, NULL);
			buf[n / sizeof(TCHAR)] = '\0';
			if (!ret || !n)
				break;

			WaitForSingleObject(PodeEscrever, INFINITE);

			WaitForSingleObject(hMutex, INFINITE);

			/**/
			// Copiar o valor de in para pos = i % Buffers;
			nome = getNomeJogador(hPipe);
			wcscpy_s(mPartilhadaZonaMsg->nave.nome, nome);
			for (int i = 0; i < mPartilhadaZonaDadosJogo->nMaxJogadores; i++) {
				if ((_tcscmp(mPartilhadaZonaDadosJogo->nDefensoras[i].nome, mPartilhadaZonaMsg->nave.nome)) == 0) {
					pos = i;
					break;
				}
			}
			//pos = mPartilhadaZonaDadosJogo->nJogadoresAtivos;

			//atualizar valor de in( na memoria partilhada->mPartilhadaZonaMsg)
			mPartilhadaZonaMsg->in = (pos + 1) % Buffers;

			/*_tprintf(TEXT("CMD>"));
			readTChars(cmd, BufferSize);*/


			mPartilhadaZonaMsg->mexer = true;
			_tprintf(TEXT("[%s] Recebi %d bytes: '%s'... (ReadFile)\n"), nome, n, buf);
			wcscpy_s(mPartilhadaZonaMsg->buf[pos], buf);
			//_stprintf_s(mPartilhadaZonaMsg->buf[pos], BufferSize, buf);

			// libertar o mutex
			ReleaseMutex(hMutex);

			_tprintf(TEXT("Escrever para buffer %d o valor '%s'\n"), pos, buf);
			ReleaseSemaphore(PodeLer, 1, NULL);
		}
		else {
			//Iniciado o jogo
			_tprintf(TEXT("Jogo Não Iniciado"));
			//_tprintf(LIMITEADICIONARCLIENTE);
		}
	}
	return 0;
}

void WINAPI RecebeTeclaCliente(HANDLE hPipe) {
	//HANDLE hPipe;
	TCHAR buf[256];
	DWORD n;
	TCHAR *nome;


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