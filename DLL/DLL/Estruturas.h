#pragma once


#include <windows.h>
#include <tchar.h>


#define Ljanela 1000
#define Cjanela 1000
#define L 20
#define C 20
#define nJogadores 20
#define nObjetos 10
#define BufferSize 100
#define Buffers 10


#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif



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
}NaveInvasora;

typedef struct {
	Celula Posicao;
}NaveDefensora;

typedef struct {
	int nInimigas, nPowerups, nDuracao, nProb, nVidasIni;
}DadosConfiguraveis;


typedef struct {
	Celula Mapa[L][C];
	NaveDefensora nDefensoras[nJogadores];
	NaveInvasora nInimigas[nJogadores];
	Powerups nPowerups[nObjetos];
	DadosConfiguraveis dConfiguraveis;
}DadosJogo;

typedef struct {
	TCHAR buf[Buffers][BufferSize];
	int in, out;
}ZonaMsg;


DLL_IMP_API void imprimeMapa(DadosJogo *mPartilhada);
