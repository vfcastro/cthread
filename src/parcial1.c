#include <stdio.h>
#include <stdlib.h>
#include "../include/cdata.h"
#include "../include/support.h"

#define QTD_TCB_MAX 30

char* LOG_THREAD_CREATE = "Criacao de TCB";
char* LOG_THREAD_DISPATCH = "Retirada de TCB da fila de apto e insercao na fila de executando";
char* LOG_THREAD_FINISH = "Retirada de TCB da fila de executando e liberacao do espaço alocado";
char* LOG_THREAD_BLOCK = "Retirada de TCB da fila de executando e insercao na fila de bloqueado";
char* LOG_THREAD_YIELD = "Retirada de TCB da fila de executando e insercao na fila de apto";
char* LOG_THREAD_UNBLOCK = "Retirada de TCB da fila de bloqueado e insercao na fila de apto";

int QTD_APTO_ATUAL,QTD_EXEC_ATUAL,QTD_BLOQ_ATUAL,QTD_TCB_CRIADOS,QTD_TCB_TERMINADOS = 0;
FILA2 apto,executando,bloqueado;

void imprime_status(int iteracao, int resto, char* acao);

int main (){
	
	int resto = -1;
	int iteracao = 0;
	
	if(CreateFila2((PFILA2)&apto) || CreateFila2((PFILA2)&executando) || CreateFila2((PFILA2)&bloqueado)){
		printf("Erro ao inicializar filas!");
		exit(1);
	}
	
	while(QTD_TCB_CRIADOS < QTD_TCB_MAX || QTD_APTO_ATUAL > 0 || QTD_EXEC_ATUAL > 0 || QTD_BLOQ_ATUAL > 0){
		resto = Random2() % 12;
		
		if(resto == 0 || resto == 6){
			imprime_status(iteracao,resto,LOG_THREAD_CREATE);
		}		
		if(resto == 1 || resto == 7){
			imprime_status(iteracao,resto,LOG_THREAD_DISPATCH);		
		}
		if(resto == 2 || resto == 8){
			imprime_status(iteracao,resto,LOG_THREAD_FINISH);
		}
		if(resto == 3 || resto == 9){
			imprime_status(iteracao,resto,LOG_THREAD_BLOCK);
		}
		if(resto == 4 || resto == 10){
			imprime_status(iteracao,resto,LOG_THREAD_YIELD);
		}
		if(resto == 5 || resto == 11){
			imprime_status(iteracao,resto,LOG_THREAD_UNBLOCK);
		}

		iteracao++;
	}	

	return 0;
}

void imprime_status(int iteracao, int resto, char* acao){
	printf("iteracao %d:\tresto: %d\tacao: %s\n",iteracao,resto,acao);
	printf("\t\tElementos em apto: %d\n",QTD_APTO_ATUAL);
	printf("\t\tElementos em executando: %d\n",QTD_EXEC_ATUAL);
	printf("\t\tElementos em bloqueado: %d\n",QTD_BLOQ_ATUAL);
	printf("\t\tElementos terminados: %d\n",QTD_TCB_TERMINADOS);
}


