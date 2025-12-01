#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "produtor-consumidor.c"

// Variáveis globais de sincronização
pthread_mutex_t mutex;
sem_t docs_disponiveis, espaco_livre;

extern void* produtor(void* arg);
extern void* consumidor(void* arg);

int main() {
    pthread_t prod[NUM_PROD], cons;
    int ids[NUM_PROD];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&docs_disponiveis, 0, 0);
    sem_init(&espaco_livre, 0, TAM);

    printf("\n======== Configuração do Sistema ========\n"
    "  · Produtores com períodos diferentes:\n"
    "  · Produtor 1: 700 ms\n"
    "  · Produtor 2: 1200 ms\n"
    "  · Produtor 3: 1800 ms\n"
    "Consumidor: 900 ms\n"
    "Deadline geral: 5000 ms\n\n");

    for (int i = 1; i < NUM_PROD; i++) {
        ids[i] = i; // IDs começam em 1
        pthread_create(&prod[i], NULL, produtor, &ids[i]);
    }

    pthread_create(&cons, NULL, consumidor, NULL);

    for (int i = 1; i < NUM_PROD; i++)
        pthread_join(prod[i], NULL);

    pthread_join(cons, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&docs_disponiveis);
    sem_destroy(&espaco_livre);

    return 0;
}
