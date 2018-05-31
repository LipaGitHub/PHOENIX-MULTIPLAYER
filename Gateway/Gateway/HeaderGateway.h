#pragma once

ZonaMsg *mPartilhadaZonaMsg;

HANDLE PodeEscrever, PodeLer, hMutex;
TCHAR NomeMemoria[] = TEXT("Memoria Partilhada MSG");
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever"), NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");

char init = 1;
TCHAR cmd[BufferSize];


void readTChars(TCHAR * p, int maxchars) {
	int len;
	_fgetts(p, maxchars, stdin);
	len = _tcslen(p);
	if (p[len - 1] == TEXT('\n'))
		p[len - 1] = TEXT('\0');
}

int IniciarMemoriaMutexSemaforo() {
	HANDLE hZonaMsg;
	LARGE_INTEGER d;

	int pos;
	char init = 1;

	//Criar um MUTEX para acesso ao in(nome= "MUTEX in")
	hMutex = CreateMutex(NULL, FALSE, TEXT("Mutex in"));
	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);

	d.QuadPart = sizeof(ZonaMsg);

	hZonaMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, d.HighPart, d.LowPart, NomeMemoria);

	if (PodeEscrever == NULL || PodeLer == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	mPartilhadaZonaMsg = (ZonaMsg*)MapViewOfFile(hZonaMsg, FILE_MAP_ALL_ACCESS, 0, 0, (SIZE_T)d.QuadPart);

	if (mPartilhadaZonaMsg == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		return -1;
	}

	if (init) {
		//Colocar in a zero
		mPartilhadaZonaMsg->in = 0;
		mPartilhadaZonaMsg->out = 0;
		//Restaurar o valor do semaforo PODEESCREVER(+10)
		ReleaseSemaphore(PodeEscrever, 10, NULL);
	}
	return 0;
}              

DWORD WINAPI EscreverMsg() {
	int pos;

	while (1) {
		WaitForSingleObject(PodeEscrever, INFINITE);
		//Esperar pelo mutex
		WaitForSingleObject(hMutex, INFINITE);
		// Copiar o valor de in para pos = i % Buffers;
		pos = mPartilhadaZonaMsg->in;
		//atualizar valor de in( na memoria partilhada->mPartilhadaZonaMsg)
		mPartilhadaZonaMsg->in = (mPartilhadaZonaMsg->in + 1) % Buffers;

		_tprintf(TEXT("CMD>"));

		readTChars(cmd, BufferSize);

		_stprintf_s(mPartilhadaZonaMsg->buf[pos], BufferSize, cmd);

		// libertar o mutex
		ReleaseMutex(hMutex);

		_tprintf(TEXT("Escrever para buffer %d o valor '%s'\n"), pos, mPartilhadaZonaMsg->buf[pos]);
		Sleep(1000);
		ReleaseSemaphore(PodeLer, 1, NULL);
	};

}