#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\..\DLL\DLL\Estruturas.h"
#include "HeaderServidor.h"

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (IniciarMemoriaMutexSemaforo() == (-1)) {
		return -1;
	}

	_tprintf(TEXT("-------------------------------------\n"));
	_tprintf(TEXT("----BEM-VINDO PHOENIX MULTIPLAYER----\n"));
	_tprintf(TEXT("-------------------------------------\n"));

	//1.Criação do jogo
	mPartilhadaDadosJogo->nJogadoresAtivos = 0;
	mPartilhadaDadosJogo->nMaxJogadores = nJogadores;
	mPartilhadaDadosJogo->jogoIniciado = false;
	dadosIniciais();


	//2.Associação ao jogo
	//Thread que está associada ao ler da memória partilhada, 
	//ou seja verifica se recebeu atualizações do gateway, nomeadamente jogadores que se tenham
	//'registado' no jogo
	HANDLE hThreadLeZonaMsg = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)leMsg, NULL, 0, NULL);

	//3.Início do jogo
	int op;
	do {
		_tprintf(TEXT("\n\nDeseja iniciar jogo?\n"));
		_tprintf(TEXT("\n1- Sim\n"));
		_tprintf(TEXT("2- Não\n"));
		_tprintf(TEXT(">>"));
		_tscanf_s(TEXT("%d"), &(op));
		if (op == 1) {

			
			if (!mPartilhadaDadosJogo->jogoCriado) {
				criarMapa();
				criaNavesInimigas(mPartilhadaDadosJogo->dConfiguraveis.nInimigas);
				mPartilhadaDadosJogo->jogoCriado = true;
				HANDLE imprimeMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imprimeMapa, mPartilhadaDadosJogo, 0, NULL);
				ReleaseSemaphore(PodeLer, 1, NULL);
			}
			else {
				_tprintf(TEXT("Ainda não Foi criado um Jogo\n"));
			}
			
			if (mPartilhadaDadosJogo->nJogadoresAtivos >= 1) {
				//Faz com que nenhum cliente/jogador seja mais aceite no jogo
				//WaitForSingleObject(hThreadLeZonaMsg, INFINITE);
				mPartilhadaDadosJogo->jogoIniciado = true;
				HANDLE hThreadEscreveZonaMsg = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escreveMsg, NULL, 0, NULL);
			}
			else {
				_tprintf(TEXT("Ainda não houve jogadores conectados ao jogo.\n"));
			}
			
		}
	} while (mPartilhadaDadosJogo->nJogadoresAtivos == 0 || op == 2);
	/*
	do {
		if (mPartilhadaDadosJogo->nJogadoresAtivos >= 1) {
			//Faz com que nenhum cliente/jogador seja mais aceite no jogo
			//WaitForSingleObject(hThreadLeZonaMsg, INFINITE);
			mPartilhadaDadosJogo->jogoIniciado = true;
			HANDLE hThreadEscreveZonaMsg = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escreveMsg, NULL, 0, NULL);
		}
		else {
			_tprintf(TEXT("Ainda não houve jogadores conectados ao jogo.\n"));
		}
		ReleaseSemaphore(PodeLer, 1, NULL);

	} while ();
	*/
	//4.Decorrer do jogo
	while (!mPartilhadaDadosJogo->estadoJogo) {
		//HANDLE imprimeMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imprimeMapa, mPartilhadaDadosJogo, 0, NULL);
		//HANDLE lerBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)leMsg, NULL, 0, NULL);
	}

	//WaitForSingleObject(PodeLer, INFINITE);
	//criarMapa();
	//criaNavesInimigas(mPartilhadaDadosJogo->dConfiguraveis.nInimigas);

	//ReleaseSemaphore(PodeLer, 1, NULL);

	//HANDLE imprimeMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imprimeMapa, mPartilhadaDadosJogo, 0, NULL);
	//HANDLE lerBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)leMsg, NULL, 0, NULL);

	//WaitForSingleObject(lerBuffer, INFINITE);

	//gerarInimigas();

	system("pause");
}



