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

	dadosIniciais();

	//WaitForSingleObject(PodeLer, INFINITE);
	criarMapa();
	criaNavesInimigas(mPartilhadaDadosJogo->dConfiguraveis.nInimigas);

	ReleaseSemaphore(PodeLer, 1, NULL);

	//HANDLE imprimeMap = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imprimeMapa, mPartilhadaDadosJogo, 0, NULL);
	HANDLE lerBuffer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)leMsg, NULL, 0, NULL);

	WaitForSingleObject(lerBuffer, INFINITE);

	//gerarInimigas();

	system("pause");
}



