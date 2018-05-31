#pragma once

DadosJogo *mPartilhadaDadosJogo;
ZonaMsg *mPartilhadaZonaMsg;
Celula *Tiros;

HANDLE PodeEscrever, PodeLer, hMutex;
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Sem�foro Pode Escrever"), NomeSemaforoPodeLer[] = TEXT("Sem�foro Pode Ler");

DWORD *threadId;

void criaNavesInimigas(int nCriar) {
	for (int i = 0; i < nCriar; i++) {
		mPartilhadaDadosJogo->nInimigas[i].Posicao.caracter = 'I';
		mPartilhadaDadosJogo->nInimigas[i].Posicao.x = 8 + i;
		mPartilhadaDadosJogo->nInimigas[i].Posicao.y = 1;
	}
	_tprintf(TEXT("\n--- Naves invasoras criadas ---\n"));
	system("pause");
}


void navesInimigas(NaveInvasora *naveInvasora) {

	//_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());

	while (1) {
		//		[ATEN�AO]
		//QUANDO ESCREVE OU LE PROTEJER Memoria partilhada
		mPartilhadaDadosJogo->Mapa[naveInvasora->Posicao.x][naveInvasora->Posicao.y].caracter = ' ';

		if (((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x < 18) {
			naveInvasora->Posicao.x++;
		}
		else if (((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x == 18) {
			naveInvasora->Posicao.y++;
			naveInvasora->Posicao.x--;
		}
		else if (((naveInvasora->Posicao.y % 2) == 0) && naveInvasora->Posicao.x >= 2) {
			naveInvasora->Posicao.x--;
			if (naveInvasora->Posicao.x == 1) {
				naveInvasora->Posicao.y++;
			}
		}
		else if (((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x == 1) {
			naveInvasora->Posicao.y++;
			naveInvasora->Posicao.x--;
		}
		Sleep(1000);
	}

	_tprintf(TEXT("[Thread Nave inimiga %d Vou desligar]\n"), GetCurrentThreadId());
}


void criarMapa() {
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			if (i == 0 || i == (L - 1)) {
				mPartilhadaDadosJogo->Mapa[j][i].caracter = '-';
			}
			else if (j == 0 || j == (C - 1)) {
				mPartilhadaDadosJogo->Mapa[j][i].caracter = '|';
			}
			else {
				mPartilhadaDadosJogo->Mapa[j][i].caracter = ' ';
			}
		}
	}
}

int IniciarMemoriaMutexSemaforo() {
	HANDLE hDadosJogo, hZonaMsg;
	LARGE_INTEGER t, d;

	hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex Out"));
	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);

	t.QuadPart = sizeof(DadosJogo);
	d.QuadPart = sizeof(ZonaMsg);
	hDadosJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, t.HighPart, t.LowPart, TEXT("Memoria Partilhada Dado Jogo"));
	hZonaMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, d.HighPart, d.LowPart, TEXT("Memoria Partilhada MSG"));

	if (PodeEscrever == NULL || PodeLer == NULL) {
		_tprintf(TEXT("[Erro]Cria��o de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	mPartilhadaDadosJogo = (DadosJogo*)MapViewOfFile(hDadosJogo, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)t.QuadPart);
	mPartilhadaZonaMsg = (ZonaMsg*)MapViewOfFile(hZonaMsg, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)d.QuadPart);

	if (mPartilhadaDadosJogo == NULL && mPartilhadaZonaMsg == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da mem�ria partilhada(%d)\n"), GetLastError());
		return -1;
	}

	return 0;
}


void dadosIniciais() {
	_tprintf(TEXT("\n\nDados Configur�veis:\n"));
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Quantas Naves Inimigas? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nInimigas));
	_tprintf(TEXT("Quantos Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nPowerups));
	_tprintf(TEXT("Tempo de Dura��o? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nDuracao));
	_tprintf(TEXT("Probabilidade de Ocorr�ncia de Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nProb));
	_tprintf(TEXT("Quantas vidas iniciais? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nVidasIni));
}


/* ----------------------------------------------------- */
/* "Thread" - Funcao associada � Thread de Naves Inimigas */
/* ----------------------------------------------------- */

DWORD WINAPI gerarInimigas() {
	HANDLE *tIni;
	HANDLE hMutex;

	tIni = (HANDLE*)malloc(mPartilhadaDadosJogo->dConfiguraveis.nInimigas * sizeof(HANDLE));		//aloca��o de mem�ria para o n� de naves invasoras
	threadId = (DWORD *)malloc(mPartilhadaDadosJogo->dConfiguraveis.nInimigas * sizeof(DWORD));	//aloca��o de mem�ria para os ID's de cada thread de nave invasora
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL) {
		_tprintf(TEXT("Deu erro no mutex..."));
	}

	for (int i = 0; i < mPartilhadaDadosJogo->dConfiguraveis.nInimigas; i++) {
		WaitForSingleObject(hMutex, INFINITE);
		tIni[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)navesInimigas, &(mPartilhadaDadosJogo->nInimigas[i]), 0, &threadId[i]);
		ReleaseMutex(hMutex);
	}

	WaitForMultipleObjects(mPartilhadaDadosJogo->dConfiguraveis.nInimigas, tIni, TRUE, INFINITE);
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}




DWORD WINAPI leMsg() {
		int pos;
	while (1)
	{
		WaitForSingleObject(PodeLer, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		pos = mPartilhadaZonaMsg->out;
		mPartilhadaZonaMsg->out = (mPartilhadaZonaMsg->out + 1) % Buffers;
		ReleaseMutex(hMutex);
		_tprintf(TEXT("Ler do Buffer %d o valor '%s'\n"), pos, mPartilhadaZonaMsg->buf[pos]); // Reader reads data
		ReleaseSemaphore(PodeEscrever, 1, NULL);
	}

}