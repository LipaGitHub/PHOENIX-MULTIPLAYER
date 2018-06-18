#pragma once

DadosJogo *mPartilhadaDadosJogo;
ZonaMsg *mPartilhadaZonaMsg;
Celula *Tiros;

HANDLE PodeEscrever, PodeLer, hMutex, hMutexMP, hMutexLer,Evento;
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever"), NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");

DWORD *threadId;

void criaNavesInimigas(int nCriar) {
	WaitForSingleObject(hMutexMP, INFINITE);
	for (int i = 0; i < nCriar; i++) {
		mPartilhadaDadosJogo->nInimigas[i].Posicao.caracter = 'I';
		mPartilhadaDadosJogo->nInimigas[i].Posicao.x = 8 + i;
		mPartilhadaDadosJogo->nInimigas[i].Posicao.y = 1;
	}
	ReleaseMutex(hMutexMP);
	_tprintf(TEXT("\n--- Naves invasoras criadas ---\n"));
	//system("pause");
}


void navesInimigas(NaveInvasora *naveInvasora) {

	//_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());

	while (1) {
		//		[ATENÇAO]
		//QUANDO ESCREVE OU LE PROTEJER Memoria partilhada
		WaitForSingleObject(hMutexMP, INFINITE);
		mPartilhadaDadosJogo->Mapa[naveInvasora->Posicao.x][naveInvasora->Posicao.y].caracter = ' ';
		ReleaseMutex(hMutexMP);
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
	WaitForSingleObject(hMutexMP, INFINITE);
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
	ReleaseMutex(hMutexMP);
}

int IniciarMemoriaMutexSemaforo() {
	HANDLE hDadosJogo, hZonaMsg;
	LARGE_INTEGER t, d;

	hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex Out"));
	hMutexMP = CreateMutex(NULL, FALSE, TEXT("Mutex MP"));
	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);
	hMutexLer = CreateMutex(NULL, FALSE, TEXT("Mutex Ler"));
	Evento = CreateEvent(NULL, TRUE, FALSE, NULL);

	t.QuadPart = sizeof(DadosJogo);
	d.QuadPart = sizeof(ZonaMsg);
	hDadosJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, t.HighPart, t.LowPart, TEXT("Memoria Partilhada Dado Jogo"));
	hZonaMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, d.HighPart, d.LowPart, TEXT("Memoria Partilhada MSG"));

	if (PodeEscrever == NULL || PodeLer == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	mPartilhadaDadosJogo = (DadosJogo*)MapViewOfFile(hDadosJogo, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)t.QuadPart);
	mPartilhadaZonaMsg = (ZonaMsg*)MapViewOfFile(hZonaMsg, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)d.QuadPart);

	if (mPartilhadaDadosJogo == NULL || mPartilhadaZonaMsg == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		return -1;
	}
	_tprintf(TEXT("[Sucesso]Criação da Memória Partilhada e Mutexes\n"));
	return 0;
}


void dadosIniciais() {
	_tprintf(TEXT("\n\nDados Configuráveis:\n"));
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Qual o número máximo de jogadores humanos no jogo? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->nMaxJogadores));
	_tprintf(TEXT("Quantas Naves Inimigas? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nInimigas));
	_tprintf(TEXT("Quantos Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nPowerups));
	_tprintf(TEXT("Tempo de Duração? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nDuracao));
	_tprintf(TEXT("Probabilidade de Ocorrência de Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nProb));
	_tprintf(TEXT("Quantas vidas iniciais? "));
	_tscanf_s(TEXT("%d"), &(mPartilhadaDadosJogo->dConfiguraveis.nVidasIni));
}


/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread de Naves Inimigas */
/* ----------------------------------------------------- */

DWORD WINAPI gerarInimigas() {
	HANDLE *tIni;
	HANDLE hMutexMP;

	tIni = (HANDLE*)malloc(mPartilhadaDadosJogo->dConfiguraveis.nInimigas * sizeof(HANDLE));		//alocação de memória para o n° de naves invasoras
	threadId = (DWORD *)malloc(mPartilhadaDadosJogo->dConfiguraveis.nInimigas * sizeof(DWORD));	//alocação de memória para os ID's de cada thread de nave invasora
	hMutexMP = CreateMutex(NULL, FALSE, NULL);
	if (hMutexMP == NULL) {
		_tprintf(TEXT("Deu erro no mutex..."));
	}

	for (int i = 0; i < mPartilhadaDadosJogo->dConfiguraveis.nInimigas; i++) {
		WaitForSingleObject(hMutexMP, INFINITE);
		tIni[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)navesInimigas, &(mPartilhadaDadosJogo->nInimigas[i]), 0, &threadId[i]);
		ReleaseMutex(hMutexMP);
	}

	WaitForMultipleObjects(mPartilhadaDadosJogo->dConfiguraveis.nInimigas, tIni, TRUE, INFINITE);
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}

void registaNave() {
	int pos;
	WaitForSingleObject(hMutex, INFINITE);
	mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].processID = mPartilhadaZonaMsg->nave.processID;
	wcscpy_s(mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].nome, mPartilhadaZonaMsg->nave.nome);
	mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].tHandle = mPartilhadaZonaMsg->nave.tHandle;
	if (mPartilhadaDadosJogo->nJogadoresAtivos == 0) {
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.x = 10;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.y = 10;
		mPartilhadaDadosJogo->Mapa[10][10].caracter = 'D';
	}
	else if (mPartilhadaDadosJogo->nJogadoresAtivos % 2 == 0) {
		pos = 10 + mPartilhadaDadosJogo->nJogadoresAtivos;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.x = pos;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.y = 10;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.caracter = 'D';
		mPartilhadaDadosJogo->Mapa[pos][10].caracter = 'D';
	}
	else {
		pos = 10 - mPartilhadaDadosJogo->nJogadoresAtivos;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.x = pos;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.y = 10;
		mPartilhadaDadosJogo->nDefensoras[mPartilhadaDadosJogo->nJogadoresAtivos].Posicao.caracter = 'D';
		mPartilhadaDadosJogo->Mapa[pos][10].caracter = 'D';
	}

	mPartilhadaDadosJogo->nJogadoresAtivos++;

	ReleaseMutex(hMutex);
	ReleaseSemaphore(PodeLer, 1, NULL);
}

void teclaCima(int w) {
	//TEM QUE SER RETIFICADO,
	//VERIFICAR SE NÂO ESTA NINGUEM NA POSIÇÃO
	int x = mPartilhadaDadosJogo->nDefensoras[w].Posicao.x,
		y = mPartilhadaDadosJogo->nDefensoras[w].Posicao.y;
	mPartilhadaDadosJogo->Mapa[x][y].caracter = ' ';
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.x = x;
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.y = y - 1;
	//[->linha][coluna]
	mPartilhadaDadosJogo->Mapa[x][y-1].caracter = 'D';
}
void teclaBaixo(int w) {
	_tprintf(BAIXO);
	int x = mPartilhadaDadosJogo->nDefensoras[w].Posicao.x,
		y = mPartilhadaDadosJogo->nDefensoras[w].Posicao.y;
	mPartilhadaDadosJogo->Mapa[x][y].caracter = ' ';
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.x = x;
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.y = y + 1;
	//[->linha][coluna]
	mPartilhadaDadosJogo->Mapa[x][y+1].caracter = 'D';
}

void teclaEsquerda(int w) {
	_tprintf(ESQUERDA);
	
	int x = mPartilhadaDadosJogo->nDefensoras[w].Posicao.x,
		y = mPartilhadaDadosJogo->nDefensoras[w].Posicao.y;
	mPartilhadaDadosJogo->Mapa[x][y].caracter = ' ';
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.x = x-1;
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.y = y;
	//[->linha][coluna]
	mPartilhadaDadosJogo->Mapa[x-1][y].caracter = 'D';
}

void teclaDireita(int w) {
	_tprintf(DIREITA);
	
	int x = mPartilhadaDadosJogo->nDefensoras[w].Posicao.x,
		y = mPartilhadaDadosJogo->nDefensoras[w].Posicao.y;
	mPartilhadaDadosJogo->Mapa[x][y].caracter = ' ';
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.x = x+1;
	mPartilhadaDadosJogo->nDefensoras[w].Posicao.y = y;
	//[->linha][coluna]
	mPartilhadaDadosJogo->Mapa[x+1][y].caracter = 'D';
}


DWORD WINAPI leMsg() {
	int pos, w;
	while (1)
	{
		WaitForSingleObject(PodeLer, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		pos = mPartilhadaDadosJogo->nJogadoresAtivos;
		mPartilhadaZonaMsg->out = (mPartilhadaZonaMsg->out + 1) % Buffers;
		ReleaseMutex(hMutex);
		if (!mPartilhadaDadosJogo->jogoIniciado) {
			if (!mPartilhadaZonaMsg->mexer) {
				if ((_tcscmp(mPartilhadaZonaMsg->buf[pos], ADICIONARCLIENTE)) == 0) {
					registaNave();
					_tprintf(TEXT("%s: '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
				}
			}
			else {
				for (int i = 0; i < mPartilhadaDadosJogo->nMaxJogadores; i++) {
					if ((_tcscmp(mPartilhadaDadosJogo->nDefensoras[i].nome, mPartilhadaZonaMsg->nave.nome)) == 0) {
						pos = i;
						break;
					}
				}
				if ((_tcscmp(mPartilhadaZonaMsg->buf[pos], CIMA)) == 0) {
					_tprintf(TEXT("%s: '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
					teclaCima(pos);
				}
				else if ((_tcscmp(mPartilhadaZonaMsg->buf[pos], BAIXO)) == 0) {
					_tprintf(TEXT("%s: '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
					teclaBaixo(pos);
				}
				else if ((_tcscmp(mPartilhadaZonaMsg->buf[pos], ESQUERDA)) == 0) {
					_tprintf(TEXT("%s: '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
					teclaEsquerda(pos);
				}
				else if ((_tcscmp(mPartilhadaZonaMsg->buf[pos], DIREITA)) == 0) {
					_tprintf(TEXT("%s: '%s'\n"), mPartilhadaZonaMsg->nave.nome, mPartilhadaZonaMsg->buf[pos]);
					teclaDireita(pos);
				}
			}

		}
		else {
			return 0;
		}
		ReleaseSemaphore(PodeEscrever, 1, NULL);
		//Dispulta evento de atualização
		
	}

}

DWORD WINAPI escreveMsg() {
	int pos;
	while (1)
	{
		WaitForSingleObject(PodeLer, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		pos = mPartilhadaZonaMsg->out;
		mPartilhadaZonaMsg->out = (mPartilhadaZonaMsg->out + 1) % Buffers;
		ReleaseMutex(hMutex);
		_tprintf(TEXT("Ler do Buffer %d o valor '%s'\n"), pos, mPartilhadaZonaMsg->buf[pos]); // Reader reads data
		ReleaseSemaphore(PodeEscrever, mPartilhadaDadosJogo->nMaxJogadores, NULL);
	}
}