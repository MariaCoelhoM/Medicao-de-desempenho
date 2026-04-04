#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Semáforos para controlar a entrada (proporção 2H:1O)
sem_t sem_h;
sem_t sem_o;

// Semáforos de barreira para sincronizar os 3 átomos
sem_t barreira_h;
sem_t barreira_o;

// Mutex para proteger contadores globais
pthread_mutex_t mutex;
int h_prontos = 0;
int o_prontos = 0;

void* thread_hidrogenio(void* arg) {
    // Espera sua vez na fila de 2 hidrogênios
    sem_wait(&sem_h);
    
    pthread_mutex_lock(&mutex);
    h_prontos++;
    printf("  [H] Átomo de Hidrogênio pronto para reagir. (%d/2 H, %d/1 O)\n", h_prontos, o_prontos);
    
    // Se já temos 2H e 1O, liberamos a reação
    if (h_prontos == 2 && o_prontos == 1) {
        sem_post(&barreira_h);
        sem_post(&barreira_h);
        sem_post(&barreira_o);
    }
    pthread_mutex_unlock(&mutex);

    // Espera autorização para a reação química
    sem_wait(&barreira_h);
    
    // Libera a vaga para o próximo H entrar no futuro
    sem_post(&sem_h);
    return NULL;
}

void* thread_oxigenio(void* arg) {
    // Espera sua vez na fila de 1 oxigênio
    sem_wait(&sem_o);

    pthread_mutex_lock(&mutex);
    o_prontos++;
    printf("  [O] Átomo de Oxigênio pronto para reagir. (%d/2 H, %d/1 O)\n", h_prontos, o_prontos);

    if (h_prontos == 2 && o_prontos == 1) {
        sem_post(&barreira_h);
        sem_post(&barreira_h);
        sem_post(&barreira_o);
    }
    pthread_mutex_unlock(&mutex);

    // Espera autorização
    sem_wait(&barreira_o);

    // REGRA: O Oxigênio realiza a produção da água
    printf("\n>>> SUCESSO: H2O FORMADA! O Oxigênio uniu os átomos. <<<\n\n");

    pthread_mutex_lock(&mutex);
    h_prontos = 0;
    o_prontos = 0;
    pthread_mutex_unlock(&mutex);

    // Libera a vaga para o próximo O entrar
    sem_post(&sem_o);
    return NULL;
}

int main() {
    char opcao;
    pthread_t t;

    // Inicialização
    sem_init(&sem_h, 0, 2);      // Limite de 2 H por molécula
    sem_init(&sem_o, 0, 1);      // Limite de 1 O por molécula
    sem_init(&barreira_h, 0, 0); // Bloqueia H até ter o trio
    sem_init(&barreira_o, 0, 0); // Bloqueia O até ter o trio
    pthread_mutex_init(&mutex, NULL);

    printf("Comandos: 'h' para Hidrogênio, 'o' para Oxigênio, 'q' para sair.\n");

    while (1) {
        scanf(" %c", &opcao);

        if (opcao == 'h' || opcao == 'H') {
            pthread_create(&t, NULL, thread_hidrogenio, NULL);
            pthread_detach(t); // Limpa recursos automaticamente ao acabar
        } 
        else if (opcao == 'o' || opcao == 'O') {
            pthread_create(&t, NULL, thread_oxigenio, NULL);
            pthread_detach(t);
        } 
        else if (opcao == 'q') {
            break;
        }
    }

    return 0;
}