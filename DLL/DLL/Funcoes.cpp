#include <windows.h>
#include "Estruturas.h"


void imprimeMapa(DadosJogo *mPartilhada) {
	while (1) {
		system("cls");
		for (int i = 0; i < mPartilhada->dConfiguraveis.nInimigas; i++) {
			mPartilhada->Mapa[mPartilhada->nInimigas[i].Posicao.x][mPartilhada->nInimigas[i].Posicao.y].caracter
				= mPartilhada->nInimigas->Posicao.caracter;
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
