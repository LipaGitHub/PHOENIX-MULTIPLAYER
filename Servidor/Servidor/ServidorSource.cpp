#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\..\DLL\DLL\Estruturas.h"



DWORD WINAPI gerarInimigas();
void navesInimigas(NaveInvasora *naveInvasora);
void criarMapa();
void imprimeMapa();
void dadosIniciais();
void criaNavesInimigas(int nCriar);

Celula *Tiros;


DWORD *threadId;



int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	DadosJogo dJogo;

	HANDLE hMap;
	LARGE_INTEGER t;

	//DWORD *mapa;

	t.QuadPart = sizeof(DadosJogo);
	hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, t.HighPart, t.LowPart, TEXT("shm"));

	mPartilhada = (DadosJogo*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)t.QuadPart);

	_tprintf(TEXT("-------------------------------------\n"));
	_tprintf(TEXT("----BEM-VINDO PHOENIX MULTIPLAYER----\n"));
	_tprintf(TEXT("-------------------------------------\n"));

	dadosIniciais();

	criarMapa();
	criaNavesInimigas(mPartilhada->dConfiguraveis.nInimigas);
	
	HANDLE imprimeMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imprimeMapa, NULL, 0, NULL);
	gerarInimigas();
	
	system("pause");
}


void dadosIniciais() {
	_tprintf(TEXT("\n\nDados Configuráveis:\n"));
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Quantas Naves Inimigas? "));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nInimigas));
	_tprintf(TEXT("Quantos Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nPowerups));
	_tprintf(TEXT("Tempo de Duração? "));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nDuracao));
	_tprintf(TEXT("Probabilidade de Ocorrência de Powerups? "));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nProb));
	_tprintf(TEXT("Quantas vidas iniciais? "));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nVidasIni));
}


/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread de Naves Inimigas */
/* ----------------------------------------------------- */

DWORD WINAPI gerarInimigas() {
	HANDLE *tIni;
	HANDLE hMutex;

	tIni = (HANDLE*)malloc(mPartilhada->dConfiguraveis.nInimigas * sizeof(HANDLE));		//alocação de memória para o n° de naves invasoras
	threadId = (DWORD *)malloc(mPartilhada->dConfiguraveis.nInimigas * sizeof(DWORD));	//alocação de memória para os ID's de cada thread de nave invasora
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL) {
		_tprintf(TEXT("Deu erro no mutex..."));
	}

	for (int i = 0; i < mPartilhada->dConfiguraveis.nInimigas; i++) {
		WaitForSingleObject(hMutex, INFINITE);
		tIni[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)navesInimigas, &(mPartilhada->nInimigas[i]), 0, &threadId[i]);
		ReleaseMutex(hMutex);
	}

	WaitForMultipleObjects(mPartilhada->dConfiguraveis.nInimigas, tIni, TRUE, INFINITE);
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}

void criaNavesInimigas(int nCriar) {
	for (int i = 0; i < nCriar; i++) {
		mPartilhada->nInimigas[i].Posicao.caracter = 'I';
		mPartilhada->nInimigas[i].Posicao.x = 8 + i;
		mPartilhada->nInimigas[i].Posicao.y = 1;
	}
	_tprintf(TEXT("\n--- Naves invasoras criadas ---\n"));
	system("pause");
}


void navesInimigas(NaveInvasora *naveInvasora) {

	//_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());

	while (1) {
		mPartilhada->Mapa[naveInvasora->Posicao.x][naveInvasora->Posicao.y].caracter = ' ';

		if (((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x < 18) {
			naveInvasora->Posicao.x++;
		}
		else if(((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x == 18) {
			naveInvasora->Posicao.y++;
			naveInvasora->Posicao.x--;
		}
		else if (((naveInvasora->Posicao.y % 2) == 0) && naveInvasora->Posicao.x >= 2) {
			naveInvasora->Posicao.x--;
			if (naveInvasora->Posicao.x == 1) {
				naveInvasora->Posicao.y++;
			}
		}
		else if(((naveInvasora->Posicao.y % 2) != 0) && naveInvasora->Posicao.x == 1){
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
				mPartilhada->Mapa[j][i].caracter = '-';
			}
			else if (j == 0 || j == (C - 1)) {
				mPartilhada->Mapa[j][i].caracter = '|';
			}
			else {
				mPartilhada->Mapa[j][i].caracter = ' ';
			}
		}
	}
}

void imprimeMapa() {
	while (1) {
		system("cls");
		for (int i = 0; i < mPartilhada->dConfiguraveis.nInimigas; i++) {
			mPartilhada->Mapa[mPartilhada->nInimigas[i].Posicao.x][mPartilhada->nInimigas[i].Posicao.y].caracter = mPartilhada->nInimigas->Posicao.caracter;
			//_tprintf(TEXT("%c"), mPartilhada->Mapa[mPartilhada->nInimigas[i].Posicao.x][mPartilhada->nInimigas[i].Posicao.y].caracter);
		}

		for (int i = 0; i < L; i++) {
			for (int j = 0; j < C; j++) {
				if (mPartilhada->Mapa[j][i].caracter != ' ') {
					_tprintf(TEXT("%c"), mPartilhada->Mapa[j][i].caracter);
				}
				else {
					_tprintf(TEXT(" "));
				}
			}
			_tprintf(TEXT("\n"));
		}
		Sleep(1000);
	}
}
