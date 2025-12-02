#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TAM 5
#define NUM_PROD 4
#define DEADLINE_MS 5000   // 5s

// Períodos (em ms)
int PERIODOS_PRODUTORES[NUM_PROD] = {700, 1200, 1800};
int PERIODO_CONSUMIDOR = 900;  // 

// Estrutura do documento com deadline
typedef struct {
    int id;
    struct timespec deadline;
} Doc;

// FIFO circular
Doc buffer[TAM];
int inicio = 0;
int fim = 0;
int num_docs = 0;
int next_doc_id = 0;


extern pthread_mutex_t mutex;
extern sem_t docs_disponiveis, espaco_livre;

// Função auxiliar para adicionar ms ao timespec
void add_ms_to_timespec(struct timespec* ts, long ms) {
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (ms % 1000) * 1000000;

    if (ts->tv_nsec >= 1000000000) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000;
    }
}

void printFila() {
    printf("Fila atual: [");

    for (int i = 0; i < TAM; i++) {
        if (i < num_docs) {
            int idx = (inicio + i) % TAM;
            printf("%d", buffer[idx].id);
        } else {
            printf("_");
        }
        if (i < TAM - 1) printf(", ");
    }

    printf("]\n");
}

void* produtor(void* arg) {
    int id = *(int*)arg;
    int periodo = PERIODOS_PRODUTORES[id];
    while (1) {
        printf("Produtor %d tentando enviar doc...\n", id);
        
        // Espera espaço
        if (sem_trywait(&espaco_livre) != 0) {
            printf("Produtor %d aguardando espaco livre...\n", id);
            sem_wait(&espaco_livre);
        }
        pthread_mutex_lock(&mutex);
        
        Doc d; // Criar documento
        d.id = next_doc_id++;

        // Deadline = agora + DEADLINE_MS
        clock_gettime(CLOCK_REALTIME, &d.deadline);
        add_ms_to_timespec(&d.deadline, DEADLINE_MS);

        // Inserção FIFO circular
        buffer[fim] = d;
        fim = (fim + 1) % TAM;
        num_docs++;

        printf("[Produtor %d] ENVIOU doc %d (nº de docs na  fila: %d)\n", id, d.id, num_docs);
        printFila();
        pthread_mutex_unlock(&mutex);
        sem_post(&docs_disponiveis);
        usleep(periodo * 1000);// Período da tarefa
    }
}

void* consumidor(void* arg) {
    while (1) {
        // Espera documento
        if (sem_trywait(&docs_disponiveis) != 0) {
           
            printf("Impressora aguardando doc disponivel...\n");
            
            sem_wait(&docs_disponiveis);
        }

        pthread_mutex_lock(&mutex);

        // Remover FIFO
        Doc d = buffer[inicio];
        inicio = (inicio + 1) % TAM;
        num_docs--;

        // Checar deadline
        struct timespec agora;
        clock_gettime(CLOCK_REALTIME, &agora);

        int perdeu_deadline =
            (agora.tv_sec > d.deadline.tv_sec) ||
            (agora.tv_sec == d.deadline.tv_sec &&
             agora.tv_nsec > d.deadline.tv_nsec);

        if (perdeu_deadline) {
            printf("⚠️  Documento %d PERDEU o deadline!\n", d.id);
        } else {
            printf("✔ Documento %d dentro do deadline.\n", d.id);
        }
     
        printf("Impressora PROCESSOU doc %d (nº de docs na fila: %d)\n", d.id, num_docs);
        printFila();
        
        pthread_mutex_unlock(&mutex);
        sem_post(&espaco_livre);
      
        usleep(PERIODO_CONSUMIDOR * 1000); // Período do consumidor
    }
}
