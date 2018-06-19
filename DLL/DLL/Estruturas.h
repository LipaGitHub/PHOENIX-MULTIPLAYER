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
#define CIMA TEXT("CIMA")
#define BAIXO TEXT("BAIXO")
#define ESQUERDA TEXT("ESQUERDA")
#define DIREITA TEXT("DIREITA")
#define MAXJOGADORESEXCEDIDO TEXT("Número máximo de jogadores excedido!")
#define JOGOJAINICIADO TEXT("O jogo já foi iniciado pelo Servidor!")
#define POSSIVELJOGAR TEXT("Não é possível iniciar jogar!")
#define ADICIONARCLIENTE TEXT("Um cliente quer aderir ao jogo!")
#define ERROADICIONARCLIENTE TEXT("Erro ocorrido! Um cliente não pode aderir ao jogo!")
#define LIMITEADICIONARCLIENTE TEXT("Já não é possível adicionar um cliente ao jogo!")

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
	DWORD processID; //guarda o ID do processo da thread que controla essa nave (cliente)
	HANDLE tHandle;
	TCHAR nome[25];		//nome do jogador
	int pontos;		//manter o número de pontos atualizados do jogador
	Celula Posicao;
}NaveDefensora;

typedef struct {
	int nInimigas, nPowerups, nDuracao, nProb, nVidasIni;
}DadosConfiguraveis;


typedef struct {
	bool jogoIniciado;	//variável para saber se o jogo já foi lançado pelo servidor ou não
	bool estadoJogo;	//true = o jogo continua(ainda ninguem perdeu/ganhou); false = o jogo acabou
	bool jogoCriado;    //Variavel que representa se o Servidor já criou um Mapa/Jogo
	int nMaxJogadores;
	int nJogadoresAtivos;
	Celula Mapa[L][C];
	NaveDefensora nDefensoras[nJogadores];
	NaveInvasora nInimigas[nJogadores];
	Powerups nPowerups[nObjetos];
	DadosConfiguraveis dConfiguraveis;
}DadosJogo;

typedef struct {
	NaveDefensora nDefensoras[nJogadores];
	NaveInvasora nInimigas[nJogadores];
	Powerups nPowerups[nObjetos];
}MsgCliente;

typedef struct {
	bool mexer;
	NaveDefensora nave;
	TCHAR buf[Buffers][BufferSize];
	int in, out;
	MsgCliente dadosCliente;
}ZonaMsg;




DLL_IMP_API void imprimeMapa(DadosJogo *mPartilhada);

