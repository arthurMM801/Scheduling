// Equipe
// Arthur de Melo Maia - 2111056
// Gustavo Herbster Martins Macedo - 2020716
// Matheus Gois - 2015037
// Victor Hugo Paiva - 2127580


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define PROCESS 5

typedef struct PCB {
    int pid;
    int chegada;
    int burst;
    float prioridade;
    struct PCB *prev;
    struct PCB *next;
    struct PCB *dad;
} PCB;


void init_pcb(PCB* pcb, int i, int chegada, int burst, int prioridade){
	pcb->pid = i;
	pcb->chegada = chegada;
	pcb->burst = burst;
	pcb->prioridade = prioridade;
}

typedef struct FCFS FCFS;
struct FCFS{
    PCB* head;
    PCB* tail;
    int size;
};


void init_FCFS(FCFS* fila){
	fila->head = NULL;
	fila->tail = NULL;
	fila->size = 0;
}

void push_FCFS(FCFS* fila, PCB* pcb){
    if(fila->size == 0){
        fila->head = pcb;
        fila->tail = pcb;
    }else{
        fila->tail->next = pcb;
        fila->tail = fila->tail->next;
    }	
    fila->size++;	
};


PCB* pop_FCFS(FCFS* fila){
	if (fila->size == 0){
		return NULL;
	}
    PCB* aux = fila->head;
    fila->head = fila->head->next;
    fila->size--;
    return aux;
}


typedef struct {
    PCB **tree;
    int size;
} Prioridade;

void swap(PCB **tree, int i, int j) {
    PCB *temp = tree[i];
    tree[i] = tree[j];
    tree[j] = temp;
}

void go_down(Prioridade *fila, int i) {
    int j = (i + 1) * 2;
    int n = fila->size;
    if (j <= n) {
        if (j < n) {
            if (fila->tree[j]->prioridade < fila->tree[j - 1]->prioridade) {
                j += 1;
            }
        }
        if (fila->tree[i]->prioridade > fila->tree[j - 1]->prioridade) {
            swap(fila->tree, i, j - 1);
            go_down(fila, j - 1);
        }
    }
}

void go_up(Prioridade *fila, int i) {
    if (i == 0) {
        return;
    }
    int j = (i - 1) / 2;
    if (fila->tree[i]->prioridade < fila->tree[j]->prioridade) {
        swap(fila->tree, i, j);
        go_up(fila, j);
    }
}

void push(Prioridade *fila, PCB *node) {
    fila->tree = (PCB **)realloc(fila->tree, (fila->size + 1) * sizeof(PCB *));
    fila->tree[fila->size] = node;
    fila->size += 1;

    if (fila->size == 1) {
        return;
    } else {
        go_up(fila, fila->size - 1);
    }
}

PCB *pop(Prioridade *fila) {
    if (fila->size > 0) {
        //troca o primeiro peloultimo
        PCB *temp = fila->tree[0];
        fila->tree[0] = fila->tree[fila->size - 1];
        //diminui o tamanho do vetor da arvore
        fila->size -= 1;
        fila->tree = (PCB **)realloc(fila->tree, fila->size * sizeof(PCB *));
        go_down(fila, 0);
        return temp;
    }
    return NULL;
}

float mean(float *lista) {
    float s = 0;
    int i;
    for (i = 0; i < PROCESS; i++) {
        s += lista[i];
    }
    return s / PROCESS;
}

int main() {
    srand(time(NULL)); //Define a seed do gerador de numeros aleatorios
	
	// FCFS

	FCFS fila_FCFS;
	init_FCFS(&fila_FCFS);
	
	PCB* lista_de_PCBS[PROCESS];
	float turnaround_FCFS[PROCESS];
	float espera_FCFS[PROCESS];
	int lista_de_burst[PROCESS];

	// Criando PCBs

    int chegada = 0;
    int i;
    for (i = 0; i < PROCESS; i++) {     // inicializando os PCBS
        int burst = rand() % 9 + 1;
        int prioridade = rand() % 9 + 1;
        if (i < 2) {
            chegada = 0;
        } else {
            chegada = i - 1;
        }
        PCB *pcb = (PCB *)malloc(sizeof(PCB));
        pcb->pid = i;
        pcb->chegada = chegada;
        pcb->burst = burst;
        pcb->prioridade = prioridade;
        pcb->prev = NULL;
        pcb->next = NULL;
        pcb->dad = NULL;

        lista_de_PCBS[i] = pcb;
        lista_de_burst[i] = burst;
    }


    printf("FCFS\n\n");
	//alimentando fila
	for(i = 0; i < PROCESS;i++){
        lista_de_PCBS[i]->burst = lista_de_burst[i];
        espera_FCFS[i] = -lista_de_burst[i];
        turnaround_FCFS[i] = 0;
        push_FCFS(&fila_FCFS, lista_de_PCBS[i]);
    }
    
    
    printf("Prints PCB\n");
    PCB* node_FCFS = fila_FCFS.head;
    for(i = 0; i < PROCESS; i++){
        printf("PID:%d|CHEGADA:%d|BURST:%d\n", node_FCFS->pid, node_FCFS->chegada ,node_FCFS->burst);
        node_FCFS = node_FCFS->next;
    }
    
    // Executando

    printf("\nExecucao\n");
    node_FCFS = pop_FCFS(&fila_FCFS);
    int t = 0;
    while (node_FCFS != NULL){
        while (node_FCFS->burst > 0){
            printf("t,%d PID:%d|BURST:%d\n", t, node_FCFS->pid, node_FCFS->burst);
            t ++;
            node_FCFS->burst --;
            if (node_FCFS->burst == 0){
                turnaround_FCFS[node_FCFS->pid] = t - node_FCFS->chegada;
                espera_FCFS[node_FCFS->pid] += turnaround_FCFS[node_FCFS->pid];   
            }	
        }
		node_FCFS = pop_FCFS(&fila_FCFS);
    }
    
    // Prioridade
    
    printf("\n\nPrioridade\n");
    
    //PCB *lista_de_PCBS_Priority[PROCESS];
    //int lista_de_burst[PROCESS];
    float turnaround_Priority[PROCESS];
    float espera_Priority[PROCESS];

    chegada = 0;
    
    // inicializando os PCBS
    /*
    for (i = 0; i < PROCESS; i++) {     
        int burst = rand() % 9 + 1;
        int prioridade = rand() % 9 + 1;
        if (i < 2) {
            chegada = 0;
        } else {
            chegada = i - 1;
        }
        PCB *pcb = (PCB *)malloc(sizeof(PCB));
        pcb->pid = i;
        pcb->chegada = chegada;
        pcb->burst = burst;
        pcb->prioridade = prioridade;
        pcb->prev = NULL;
        pcb->next = NULL;
        pcb->dad = NULL;

        lista_de_PCBS_Priority[i] = pcb;
        lista_de_burst[i] = burst;
    }
    */
    
    // Alimentando 

    Prioridade fila_Priority;
    fila_Priority.tree = NULL;
    fila_Priority.size = 0;

    for (i = 0; i < PROCESS; i++) {
    	lista_de_PCBS[i]->burst = lista_de_burst[i];
        espera_Priority[i] = -lista_de_burst[i];
        turnaround_Priority[i] = 0;
        //PCB *node = lista_de_PCBS[i];
        push(&fila_Priority, lista_de_PCBS[i]);
    }
    
    printf("\nPrints PCBs\n");

    for (i = 0; i < fila_Priority.size; i++) {
        printf("PID:%d|CHEGADA:%d|BURST:%d|PRIORIDADE:%f\n", fila_Priority.tree[i]->pid, fila_Priority.tree[i]->chegada, fila_Priority.tree[i]->burst, fila_Priority.tree[i]->prioridade);
    }
    
    
    // Executando

	printf("\nExecucao\n");
		
    PCB *node = pop(&fila_Priority);
    t = 0;
    while (node != NULL) {
        while (node->burst > 0) {
            printf("t:%d, PID:%d|BURST: %d\n", t, node->pid, node->burst);
            t += 1;
            node->burst -= 1;
            if (node->burst == 0) {
                turnaround_Priority[node->pid] = t - node->chegada;
                espera_Priority[node->pid] += turnaround_Priority[node->pid];
            }
        }
        node = pop(&fila_Priority);
    }

    
    
    // Shortest Job First
	
	printf("\n\nSJF\n\n");
    //lista_de_PCBS[PROCESS];
    //lista_de_burst[PROCESS];
    float turnaround_SJF[PROCESS];
    float espera_SJF[PROCESS];

 	// inicializando os PCBS
	/*
    chegada = 0;
    for (i = 0; i < PROCESS; i++) {    
        int burst = rand() % 9 + 1;
        float prioridade = rand() % 9 + 1;
        if (i < 2) {
            chegada = 0;
        } else {
            chegada = i - 1;
        }
        PCB *pcb = (PCB *)malloc(sizeof(PCB));   
        pcb->pid = i;
        pcb->chegada = chegada;
        pcb->burst = burst;
        pcb->prioridade = prioridade;
        pcb->prev = NULL;
        pcb->next = NULL;
        pcb->dad = NULL;

        lista_de_PCBS[i] = pcb;
        lista_de_burst[i] = burst;
    }
	*/
	
    Prioridade fila;
    fila.tree = NULL;
    fila.size = 0;
    
    // Alimentando

    for (i = 0; i < PROCESS; i++) {
    	lista_de_PCBS[i]->burst = lista_de_burst[i];
        espera_SJF[i] = -lista_de_burst[i];
        turnaround_SJF[i] = 0;
        PCB *node = lista_de_PCBS[i];
        node->prioridade = node->burst;
        push(&fila, node);
    }
    
    // Print PCBs
    
    printf("Print PCBs\n");

    for (i = 0; i < fila.size; i++) {
        printf("PID:%d|CHEGADA:%d|BURST:%d|PRIORIDADE:%f\n", fila.tree[i]->pid, fila.tree[i]->chegada, fila.tree[i]->burst, fila.tree[i]->prioridade);
    }

	// Executando
	
	printf("\nExecucao\n");

    node = pop(&fila);
    t = 0;
    while (node != NULL) {
        while (node->burst > 0) {
            printf("t:%d, PID:%d|BURST: %d\n", t, node->pid, node->burst);
            t += 1;
            node->burst -= 1;
            if (node->burst == 0) {
                turnaround_SJF[node->pid] = t - node->chegada;
                espera_SJF[node->pid] += turnaround_SJF[node->pid];
            }
        }
        node = pop(&fila);
    }
    
    
    // Round Robin
    printf("\n\nRound Robin\n\n");
    
    
	float turnaround_RR[PROCESS];
	float espera_RR[PROCESS];
	FCFS filaRR;
	init_FCFS(&filaRR);

	
	// Criando PCBs
	/*
	chegada = 0;
	for(i=0; i < PROCESS; i++){
		int burst = (rand() % 9)+1;
		//int burst = list[i];
		int prioridade = (rand() % 9)+1;
		if( i < 2)
			chegada = 0;
		else
			chegada = i-1;
		PCB pcb;
		init_pcb(&pcb, i, chegada, burst, prioridade);
		lista_de_PCBS_RR[i] = pcb;
		lista_de_burst_RR[i] = burst;
	} 
	*/
	
	
	// alimentando
	for(i = 0; i < PROCESS; i++){
		lista_de_PCBS[i]->burst = lista_de_burst[i];
		espera_RR[i] = -lista_de_burst[i];
		turnaround_RR[i] = 0;
		push_FCFS(&filaRR, lista_de_PCBS[i]);
	}


	// Prints dos PCBs
	printf("Prints PCB\n");
	node = filaRR.head;
	for(i = 0; i < PROCESS; i++){
		printf("PID:%d|CHEGADA:%d|BURST:%d\n", node->pid,node->chegada ,node->burst);
		node = node->next;
	}
	
	
	printf("\nSCHEDULING\n");
	// Scheduling
	node = pop_FCFS(&filaRR);
	t = 0;
	while(node != NULL){
		int quantum = 1;
		while(quantum > 0){
			printf("t:%d, PID:%d|BURST:%d\n", t, node->pid, node->burst);
			t++;
			node->burst -= 1;
			if(node->burst == 0){
				turnaround_RR[node->pid] = t - node->chegada;
				espera_RR[node->pid] += turnaround_RR[node->pid];
				break;
			}
			quantum -= 1;
		}
		if(node->burst > 0){
			push_FCFS(&filaRR, node);
		}
		node = pop_FCFS(&filaRR);
	}
	
	
	// Comparando algoritimos
	
	printf("\n\nComparando algoritimos\n\n");
	
	printf("FCFS\n");
	printf("Turnaround: %f\n", mean(turnaround_FCFS));
    printf("Espera: %f\n\n", mean(espera_FCFS));
    
	printf("Priority\n");
	printf("Turnaround: %f\n", mean(turnaround_Priority));
    printf("Espera: %f\n\n", mean(espera_Priority));
	
	printf("SJF\n");
	printf("Turnaround: %f\n", mean(turnaround_SJF));
    printf("Espera: %f\n\n", mean(espera_SJF));
    
    printf("Round Robin\n");
    printf("Turnaround: %f\n", mean(turnaround_RR));
    printf("Espera: %f\n\n", mean(espera_RR));
	
	return 0;
}

