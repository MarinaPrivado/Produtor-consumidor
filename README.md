'# Sistema Produtor–Consumidor (Impressora compartilhada)
Implementação em C usando pthreads, semáforos e buffer circular FIFO.

## 📌 Descrição Geral
Este projeto implementa um sistema do tipo **Produtor–Consumidor** utilizando:

- `pthread` (threads POSIX)
- `semáforos` (`sem_t`)
- `mutex` para exclusão mútua
- Buffer circular FIFO


O objetivo é simular um sistema em tempo real (de soft real-time) no contexto de uma impressora compartilhada onde há **produtores** gerando documentos com diferentes períodos, enquanto um **consumidor (a impressora)** processa os documentos dentro de um **deadline máximo de 5 segundos**.

---

O sistema imprime em tempo real:
- Envio de documentos
- Estado da fila
- Processamento pelo consumidor
- Avisos de deadline perdido ou alcançado 

---

## 📁 Estrutura do Projeto

├── main.c # Arquivo principal (criação de threads e init) 
├── produtor-consumidor.c     # Lógica dos produtores, consumidor e buffer 
└── README.md                 # Este arquivo

