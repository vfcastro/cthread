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
int thread_create();
int thread_dispatch();
int thread_finish();
int thread_block();
int thread_yield();
int thread_unblock();


int main (){
	
	int resto = -1;
	int iteracao = 0;
	
	if(CreateFila2((PFILA2)&apto) || CreateFila2((PFILA2)&executando) || CreateFila2((PFILA2)&bloqueado)){
		printf("Erro ao inicializar filas!\n");
		exit(-1);
	}
	
	while(QTD_TCB_CRIADOS < QTD_TCB_MAX || QTD_APTO_ATUAL > 0 || QTD_EXEC_ATUAL > 0 || QTD_BLOQ_ATUAL > 0){
		resto = Random2() % 12;
		
		if(resto == 0 || resto == 6){
			if(QTD_TCB_CRIADOS < QTD_TCB_MAX){
				if(thread_create())
					printf("Erro ao criar thread (chamada thread_create)!\n");
				imprime_status(iteracao,resto,LOG_THREAD_CREATE);
			}
		}		
		if(resto == 1 || resto == 7){
			if(thread_dispatch())
					printf("Erro ao despachar thread (chamada thread_dispatch)!\n");
			imprime_status(iteracao,resto,LOG_THREAD_DISPATCH);		
		}
		if(resto == 2 || resto == 8){
			if(thread_finish())
					printf("Erro ao finalizar thread (chamada thread_finish)!\n");
			imprime_status(iteracao,resto,LOG_THREAD_FINISH);
		}
		if(resto == 3 || resto == 9){
			if(thread_block())
					printf("Erro ao bloquear thread (chamada thread_block)!\n");
			imprime_status(iteracao,resto,LOG_THREAD_BLOCK);
		}
		if(resto == 4 || resto == 10){
			if(thread_yield())
					printf("Erro ao ceder thread (chamada thread_yield)!\n");		
			imprime_status(iteracao,resto,LOG_THREAD_YIELD);
		}
		if(resto == 5 || resto == 11){
			if(thread_unblock())
					printf("Erro ao desbloquead thread (chamada thread_unblock)!\n");		
			imprime_status(iteracao,resto,LOG_THREAD_UNBLOCK);
		}
		//sleep(1);
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

int thread_create(){
	TCB_t *ptcb = malloc(sizeof(TCB_t));
	if(ptcb == NULL){
		printf("thread_create: Erro ao criar thread (alocar TCB)!\n");
		return -1;
	}

	PNODE2 pnode = malloc(sizeof(NODE2));
	if(pnode == NULL){
		printf("thread_create: Erro ao criar thread (alocar NODE2)!\n");
		return -1;
	}	
	
	pnode->node = ptcb;
	
	if(AppendFila2((PFILA2)&apto, pnode)){
		printf("thread_create: Erro ao inserir thread na fila de apto!\n");
		free(ptcb);
		free(pnode);
		return -1;		
	}
	
	QTD_APTO_ATUAL++;
	QTD_TCB_CRIADOS++;
	
	return 0;
}

int thread_dispatch(){
	if(QTD_APTO_ATUAL == 0 || QTD_EXEC_ATUAL != 0)
		return 0;
		
	if(FirstFila2((PFILA2)&apto)){
		printf("thread_dispatch: Erro ao acessar primeira thread da fila de apto!\n");
		return -1; 
	}	
	
	TCB_t *ptcb = GetAtIteratorFila2((PFILA2)&apto);
	if(ptcb == NULL){
		printf("thread_dispatch: Erro ao acessar TCB da fila de apto!\n");
		return -1; 
	}
	
	if(DeleteAtIteratorFila2((PFILA2)&apto)){
		printf("thread_dispatch: Erro ao retirar primeira thread da fila de apto!\n");
		return -1; 
	}
		
	PNODE2 pnode = malloc(sizeof(NODE2));
	if(pnode == NULL){
		printf("thread_dispatch: Erro ao alocar NODE2 para fila executando!\n");
		return -1;
	}			
	
	pnode->node = ptcb;
			
	if(AppendFila2((PFILA2)&executando, pnode)){
		printf("thread_dispatch: Erro ao inserir thread na fila de executando!\n");
		free(pnode);
		return -1;		
	}
	
	QTD_APTO_ATUAL--;
	QTD_EXEC_ATUAL++;
	
	return 0;
}

int thread_finish(){
	if(QTD_EXEC_ATUAL == 0)
		return 0;	

	if(FirstFila2((PFILA2)&executando)){
		printf("thread_finish: Erro ao acessar thread da fila de executando!\n");
		return -1; 
	}	
	
	TCB_t *ptcb = GetAtIteratorFila2((PFILA2)&executando);
	if(ptcb == NULL){
		printf("thread_finish: Erro ao acessar TCB da fila de executando!\n");
		return -1; 
	}
	
	if(DeleteAtIteratorFila2((PFILA2)&executando)){
		printf("thread_finish: Erro ao retirar thread da fila de executando!\n");
		return -1; 
	}

	free(ptcb);
	QTD_EXEC_ATUAL--;
	QTD_TCB_TERMINADOS++;

	return 0;
}

int thread_block(){
	if(QTD_EXEC_ATUAL == 0)
		return 0;
		
	if(FirstFila2((PFILA2)&executando)){
		printf("thread_block: Erro ao acessar thread da fila de executando!\n");
		return -1; 
	}	
	
	TCB_t *ptcb = GetAtIteratorFila2((PFILA2)&executando);
	if(ptcb == NULL){
		printf("thread_block: Erro ao acessar TCB da fila de executando!\n");
		return -1; 
	}
	
	if(DeleteAtIteratorFila2((PFILA2)&executando)){
		printf("thread_block: Erro ao retirar thread da fila de executando!\n");
		return -1; 
	}
		
	PNODE2 pnode = malloc(sizeof(NODE2));
	if(pnode == NULL){
		printf("thread_block: Erro ao alocar NODE2 para fila bloqueado!\n");
		return -1;
	}			
	
	pnode->node = ptcb;
			
	if(AppendFila2((PFILA2)&bloqueado, pnode)){
		printf("thread_block: Erro ao inserir thread na fila de bloqueado!\n");
		free(pnode);
		return -1;		
	}
	
	QTD_BLOQ_ATUAL++;
	QTD_EXEC_ATUAL--;
	
	return 0;
}

int thread_yield(){
	if(QTD_EXEC_ATUAL == 0)
		return 0;
		
	if(FirstFila2((PFILA2)&executando)){
		printf("thread_yield: Erro ao acessar thread da fila de executando!\n");
		return -1; 
	}	
	
	TCB_t *ptcb = GetAtIteratorFila2((PFILA2)&executando);
	if(ptcb == NULL){
		printf("thread_yield: Erro ao acessar TCB da fila de executando!\n");
		return -1; 
	}
	
	if(DeleteAtIteratorFila2((PFILA2)&executando)){
		printf("thread_yield: Erro ao retirar thread da fila de executando!\n");
		return -1; 
	}
		
	PNODE2 pnode = malloc(sizeof(NODE2));
	if(pnode == NULL){
		printf("thread_yield: Erro ao alocar NODE2 para fila de apto!\n");
		return -1;
	}			
	
	pnode->node = ptcb;
			
	if(AppendFila2((PFILA2)&apto, pnode)){
		printf("thread_yield: Erro ao inserir thread na fila de apto!\n");
		free(pnode);
		return -1;		
	}
	
	QTD_APTO_ATUAL++;
	QTD_EXEC_ATUAL--;
	
	return 0;
}

int thread_unblock(){
	if(QTD_BLOQ_ATUAL == 0)
		return 0;
		
	if(FirstFila2((PFILA2)&bloqueado)){
		printf("thread_unblock: Erro ao acessar thread da fila de bloqueado!\n");
		return -1; 
	}	
	
	TCB_t *ptcb = GetAtIteratorFila2((PFILA2)&bloqueado);
	if(ptcb == NULL){
		printf("thread_unblock: Erro ao acessar TCB da fila de bloqueado!\n");
		return -1; 
	}
	
	if(DeleteAtIteratorFila2((PFILA2)&bloqueado)){
		printf("thread_unblock: Erro ao retirar thread da fila de bloqueado!\n");
		return -1; 
	}
		
	PNODE2 pnode = malloc(sizeof(NODE2));
	if(pnode == NULL){
		printf("thread_unblock: Erro ao alocar NODE2 para fila de apto!\n");
		return -1;
	}			
	
	pnode->node = ptcb;
			
	if(AppendFila2((PFILA2)&apto, pnode)){
		printf("thread_unblock: Erro ao inserir thread na fila de apto!\n");
		free(pnode);
		return -1;		
	}
	
	QTD_APTO_ATUAL++;
	QTD_BLOQ_ATUAL--;
	
	return 0;
}







