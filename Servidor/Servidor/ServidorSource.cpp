#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#define L 1000
#define C 1000

DWORD WINAPI gerarInimigas(int nThreads);
void navesInimigas();

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

typedef struct{
	Celula Posicao;
	int tipo;// 0-basica, 1-Esquiva
}NaveInvasora;

typedef struct {
	Celula Posicao;
}NaveDefensora;


Celula *Tiros;
Celula Mapa[L][C];


int _tmain(int argc, LPTSTR argv[]) {
	int nInimigas, nPowerups, nDuracao, nProb, nVidasIni;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	_tprintf(TEXT("----BEM-VINDO PHOENIX MULTIPLAYER----\n"));
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

	gerarInimigas(nInimigas);
	
	
	system("pause");
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

void navesInimigas(){
	
	_tprintf(TEXT("[Thread Nave inimiga %d]\n"), GetCurrentThreadId());
	
	_tprintf(TEXT("[Thread Nave inimiga %d Vou desligar]\n"), GetCurrentThreadId());
}
