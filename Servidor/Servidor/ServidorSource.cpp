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
void navesInimigas();
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

	t.QuadPart = sizeof(DadosJogo);
	hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, t.HighPart, t.LowPart, TEXT("shm"));

	mPartilhada = (DadosJogo*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)t.QuadPart);

	_tprintf(TEXT("----BEM-VINDO PHOENIX MULTIPLAYER----\n"));

	dadosIniciais();

	criarMapa();
	criaNavesInimigas(mPartilhada->dConfiguraveis.nInimigas);
	imprimeMapa();
	gerarInimigas();


	system("pause");
}


void dadosIniciais() {
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Dados Configuráveis:\n"));
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Quantas Naves Inimigas?\n"));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nInimigas));
	_tprintf(TEXT("Quantos Powerups?\n"));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nPowerups));
	_tprintf(TEXT("Tempo de Duração?\n"));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nDuracao));
	_tprintf(TEXT("Probabilidade de Ocorrência de Powerups?\n"));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nProb));
	_tprintf(TEXT("Quantas vidas iniciais?\n"));
	_tscanf_s(TEXT("%d"), &(mPartilhada->dConfiguraveis.nVidasIni));
}


/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread de Naves Inimigas */
/* ----------------------------------------------------- */

DWORD WINAPI gerarInimigas() {


	HANDLE *tIni;
	tIni = (HANDLE*)malloc(mPartilhada->dConfiguraveis.nInimigas * sizeof(HANDLE));
	//... alocar memoria ( mPartilhada->dConfiguraveis.nInimigas * sizeof(HANDLE) )

	//Necessidade de ao criar a Threada para a nave também a colocar no mapa para depois poder imprimir

	for (int i = 0; i < mPartilhada->dConfiguraveis.nInimigas; i++) {
		tIni[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)navesInimigas, NULL, 0, &threadId[i]);
	}

	WaitForMultipleObjects(mPartilhada->dConfiguraveis.nInimigas, tIni, TRUE, INFINITE);
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}



/*

void criaNavesInimigas(int nCriar) {

	NaveInvasora *threadInimigas;

	threadInimigas = (NaveInvasora *)malloc(nCriar * sizeof(NaveInvasora));
	threadId = (DWORD *)malloc(nCriar * sizeof(DWORD));
	for (int i = 0; i < mPartilhada->dConfiguraveis.nInimigas; i++) {
		//threadInimigas[i]->tInvasora = (HANDLE *) malloc(sizeof(HANDLE));
		_tprintf(TEXT("Criei Nave\n"));
	}
	system("pause");
}
*/

void navesInimigas() {

	_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());

	while (1) {

	}

	_tprintf(TEXT("[Thread Nave inimiga %d Vou desligar]\n"), GetCurrentThreadId());
}


void criarMapa() {
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			if (i == 0 || i == (L - 1)) {
				mPartilhada->Mapa[j][i].caracter = '1';
			}
			else if (j == 0 || j == (C - 1)) {
				mPartilhada->Mapa[j][i].caracter = '0';
			}

		}
	}
}

void imprimeMapa() {
	system("cls");
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			if (mPartilhada->Mapa[j][i].caracter == '1') {
				_tprintf(TEXT("-"));
			}
			else if (mPartilhada->Mapa[j][i].caracter == '0') {
				_tprintf(TEXT("|"));
			}
			else {
				_tprintf(TEXT(" "));
			}
		}
		_tprintf(TEXT("\n"));
	}
}
