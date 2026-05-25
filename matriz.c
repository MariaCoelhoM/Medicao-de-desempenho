#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 16

typedef struct {
    int id, n, num_threads;
    double *A, *B, *C;
} ThreadData;

// Acesso linear
#define IDX(i,j,n) ((i)*(n)+(j))

// Alocar matriz contínua
double* alocar_matriz(int n) {
    return (double*) malloc(n * n * sizeof(double));
}

// Inicializar
void inicializar(double *m, int n) {
    for (int i = 0; i < n*n; i++)
        m[i] = rand() % 10;
}

// ✅ Função auxiliar para medir tempo real (wall time)
double get_wall_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Multiplicação sequencial
void multiplicacao_sequencial(double *A, double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[IDX(i,j,n)] = 0;
            for (int k = 0; k < n; k++) {
                C[IDX(i,j,n)] += A[IDX(i,k,n)] * B[IDX(k,j,n)];
            }
        }
    }
}

// Thread
void* multiplicacao_paralela(void *arg) {
    ThreadData *d = (ThreadData*) arg;

    int inicio = d->id * (d->n / d->num_threads);
    int fim = (d->id == d->num_threads - 1) ? d->n : inicio + (d->n / d->num_threads);

    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < d->n; j++) {
            d->C[IDX(i,j,d->n)] = 0;
            for (int k = 0; k < d->n; k++) {
                d->C[IDX(i,j,d->n)] += d->A[IDX(i,k,d->n)] * d->B[IDX(k,j,d->n)];
            }
        }
    }

    pthread_exit(NULL);
}

// ✅ Tempo sequencial usando wall time
double tempo_seq(double *A, double *B, double *C, int n) {
    double ini = get_wall_time();
    multiplicacao_sequencial(A, B, C, n);
    return get_wall_time() - ini;
}

// ✅ Tempo paralelo usando wall time
double tempo_par(double *A, double *B, double *C, int n, int nt) {

    pthread_t threads[MAX_THREADS];
    ThreadData dados[MAX_THREADS];

    double ini = get_wall_time();

    for (int i = 0; i < nt; i++) {
        dados[i].id = i;
        dados[i].n = n;
        dados[i].num_threads = nt;
        dados[i].A = A;
        dados[i].B = B;
        dados[i].C = C;

        pthread_create(&threads[i], NULL, multiplicacao_paralela, &dados[i]);
    }

    for (int i = 0; i < nt; i++) {
        pthread_join(threads[i], NULL);
    }

    return get_wall_time() - ini;
}

int main() {

    int tamanhos[] = {200, 400, 800};
    int threads[]  = {2, 4, 8};
    int num_repeticoes = 10; // ✅ mínimo exigido pelo enunciado

    FILE *csv = fopen("resultados.csv", "w");
    fprintf(csv, "Dimensao,Threads,Tempo_Seq,Tempo_Par,Speedup,Eficiencia\n");

    printf("\n================= RESULTADOS =================\n\n");
    printf("| Dimensão | Threads | Tempo Seq | Tempo Par | Speedup | Eficiência |\n");
    printf("---------------------------------------------------------------------\n");

    for (int t = 0; t < 3; t++) {

        int n = tamanhos[t];

        printf("\nProcessando n = %d...\n", n);

        double *A = alocar_matriz(n);
        double *B = alocar_matriz(n);
        double *C = alocar_matriz(n);

        if (!A || !B || !C) {
            printf("Erro de memória!\n");
            return 1;
        }

        inicializar(A, n);
        inicializar(B, n);

        // ✅ 10 repetições para o sequencial
        double t_seq = 0;
        for (int i = 0; i < num_repeticoes; i++)
            t_seq += tempo_seq(A, B, C, n);
        t_seq /= num_repeticoes;

        for (int j = 0; j < 3; j++) {

            int nt = threads[j];
            double t_par = 0;

            // ✅ 10 repetições para o paralelo
            for (int i = 0; i < num_repeticoes; i++)
                t_par += tempo_par(A, B, C, n, nt);
            t_par /= num_repeticoes;

            double speedup = t_seq / t_par;
            double eficiencia = speedup / nt;

            // tabela
            printf("| %4dx%-4d | %7d | %9.4f | %9.4f | %7.2f | %10.2f |\n",
                   n, n, nt, t_seq, t_par, speedup, eficiencia);

            // csv
            fprintf(csv, "%d,%d,%.4f,%.4f,%.4f,%.4f\n",
                    n, nt, t_seq, t_par, speedup, eficiencia);
        }

        printf("---------------------------------------------------------------------\n");

        free(A); free(B); free(C);
    }

    fclose(csv);

    printf("\nCSV gerado com sucesso!\n");

    return 0;
}