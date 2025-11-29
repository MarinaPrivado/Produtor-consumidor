#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "produtor-consumidor.c"

#define TAM 5
#define NUM_PROD 3
// Importar símbolos do produtor_consumidor.c
extern pthread_mutex_t mutex;
extern sem_t docs_disponiveis, espaco_livre;

extern void* produtor(void* arg);
extern void* consumidor(void* arg);

int main() {
    pthread_t prod[NUM_PROD], cons;
    int ids[NUM_PROD];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&docs_disponiveis, 0, 0);
    sem_init(&espaco_livre, 0, TAM);

    for (int i = 0; i < NUM_PROD; i++) {
        ids[i] = i; // IDs começam em 0
        pthread_create(&prod[i], NULL, produtor, &ids[i]);
    }

    pthread_create(&cons, NULL, consumidor, NULL);

    for (int i = 0; i < NUM_PROD; i++)
        pthread_join(prod[i], NULL);

    pthread_join(cons, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&docs_disponiveis);
    sem_destroy(&espaco_livre);

    return 0;
}
