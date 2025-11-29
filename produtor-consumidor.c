#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TAM 5
#define NUM_PROD 3

// Variáveis globais
int buffer[TAM];
int num_docs = 0;
int next_doc_id = 0;

pthread_mutex_t mutex;
sem_t docs_disponiveis, espaco_livre;

// Funções
void printFila() {
    printf("Fila atual: [");
    for (int i = 0; i < TAM; i++) {
        if (i < num_docs)
            printf("%d", buffer[i]);
        else
            printf("_");
        if (i < TAM - 1) printf(", ");
    }
    printf("]\n");
}

void* produtor(void* arg) {
    int id = *(int*)arg;

    while (1) {
        printf("Produtor %d tentando enviar doc...\n", id);

        // tenta pegar espaço livre
        if (sem_trywait(&espaco_livre) != 0) {
            printf("Produtor %d aguardando espaço livre...\n", id);
            sem_wait(&espaco_livre);
        }

        pthread_mutex_lock(&mutex);

        int doc_id = next_doc_id++;
        buffer[num_docs++] = doc_id;

        printf("Produtor %d enviou doc %d (qtd de doc na fila: %d)\n",
               id, doc_id, num_docs);

        printFila();

        pthread_mutex_unlock(&mutex);

        sem_post(&docs_disponiveis);

        sleep(5); // espera maior
    }
}

void* consumidor(void* arg) {
    int doc;

    while (1) {
        printf("Impressora aguardando doc...\n");

        if (sem_trywait(&docs_disponiveis) != 0) {
            printf("Impressora aguardando doc disponível...\n");
            sem_wait(&docs_disponiveis);
        }

        pthread_mutex_lock(&mutex);

        doc = buffer[--num_docs];

        printf("Impressora processou doc %d (fila: %d)\n",
               doc, num_docs);

        printFila();

        pthread_mutex_unlock(&mutex);

        sem_post(&espaco_livre);

        sleep(5);
    }
}
