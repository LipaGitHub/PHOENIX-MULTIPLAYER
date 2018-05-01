#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#define Ljanela 1000
#define Cjanela 1000
#define L 20
#define C 20

DWORD WINAPI gerarInimigas(int nThreads);
void navesInimigas();
void criarMapa();
void imprimeMapa();
void dadosIniciais();
void criaNavesInimigas(int nCriar);

typedef struct {
	int x, y;
	char caracter;
}Celula;

typedef struct {
	char nome[20];
	int duracao, ocorrencia;
	//Duração 0-Temporario, 1-Permanente
	//Ocorrencia 0-Vulgar, 1-Invulgar,2-Raro
}Powerups;

typedef struct {
	Celula Posicao;
	int tipo;// 0-basica, 1-Esquiva
	//Colocar um HANDLE da Thread que gere esta nava inimiga
}NaveInvasora;

typedef struct {
	Celula Posicao;
}NaveDefensora;


Celula *Tiros;
Celula Mapa[L][C];


int _tmain(int argc, LPTSTR argv[]) {
	
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	_tprintf(TEXT("----BEM-VINDO PHOENIX MULTIPLAYER----\n"));
	
	//dadosIniciais();

	criarMapa();
	imprimeMapa();
	//gerarInimigas(nInimigas);


	system("pause");
}


void dadosIniciais() {
	int nInimigas, nPowerups, nDuracao, nProb, nVidasIni;

	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Dados Configuráveis:\n"));
	_tprintf(TEXT("\n-------------------------------------\n"));
	_tprintf(TEXT("Quantas Naves Inimigas?\n"));
	_tscanf_s(TEXT("%d"), &nInimigas);
	_tprintf(TEXT("Quantos Powerups?\n"));
	_tscanf_s(TEXT("%d"), &nPowerups);
	_tprintf(TEXT("Tempo de Duração?\n"));
	_tscanf_s(TEXT("%d"), &nDuracao);
	_tprintf(TEXT("Probabilidade de Ocorrência de Powerups?\n"));
	_tscanf_s(TEXT("%d"), &nProb);
	_tprintf(TEXT("Quantas vidas iniciais?\n"));
	_tscanf_s(TEXT("%d"), &nVidasIni);
}
/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread de Naves Inimigas */
/* ----------------------------------------------------- */
DWORD WINAPI gerarInimigas(int nThreads) {

	HANDLE *threadInimigas;
	DWORD *threadId;
	threadInimigas = (HANDLE *)malloc(nThreads * sizeof(HANDLE));
	threadId = (DWORD *)malloc(nThreads * sizeof(DWORD));

	for (int i = 0; i < nThreads; i++) {
		threadInimigas[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)navesInimigas, NULL, 0, &threadId[i]);
	}

	WaitForMultipleObjects(nThreads, threadInimigas, TRUE, INFINITE);
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}

void criaNavesInimigas(int nCriar) {
	

}

void navesInimigas() {

	_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());

	
	_tprintf(TEXT("[Thread Nave inimiga %d Vou desligar]\n"), GetCurrentThreadId());
}


void criarMapa() {
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			if (i == 0 || i == (L - 1)) {
				Mapa[j][i].caracter = '1';
			}
			else if (j == 0 || j == (C - 1)) {
				Mapa[j][i].caracter = '0';
			}

		}
	}
}

void imprimeMapa() {
	system("cls");
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			if (Mapa[j][i].caracter == '1') {
				_tprintf(TEXT("-"));
			}
			else if (Mapa[j][i].caracter == '0') {
				_tprintf(TEXT("|"));
			}
			else {
				_tprintf(TEXT(" "));
			}
		}
		_tprintf(TEXT("\n"));
	}
}