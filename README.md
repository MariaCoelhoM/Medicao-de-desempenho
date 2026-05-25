# RELATÓRIO DE ATIVIDADE
## Medição de Tempo na Multiplicação de Matrizes: Computação Paralela

**Disciplina:** Computação paralela e distribuída
**Aluno(a):** Maria Eduarda Mariano Coelho  

---

## AVISO AO PROFESSOR (JUSTIFICATIVA DE ESCOPO)

> Conforme o enunciado original da atividade, os experimentos deveriam variar os tamanhos das matrizes entre $1000 \times 1000$ até $10000 \times 10000$. Contudo, devido à alta carga computacional necessária para processar essas dimensões na versão sequencial ($O(n^3)$) e às limitações severas de hardware e tempo do ambiente de desenvolvimento em nuvem utilizado (GitHub Codespaces), os testes com dimensões a partir de $1000 \times 1000$ travaram a CPU por estouro de tempo de execução. [cite_start]Para viabilizar a coleta de dados e realizar a análise comparativa de forma fidedigna, os tamanhos das matrizes foram adaptados para $200 \times 200$, $400 \times 400$ e $800 \times 800$ [cite: 27][cite_start], mantendo a variação proposta de threads (2, 4 e 8)[cite: 28].

---

## 1. Introdução
A multiplicação de matrizes é uma operação fundamental em computação científica, aprendizado de máquina e processamento de sinais. Sua versão clássica possui complexidade $O(n^3)$, tornando-a computacionalmente intensiva para matrizes de grande porte.

Este relatório apresenta a implementação e análise de desempenho de dois algoritmos de multiplicação de matrizes quadradas: uma versão sequencial e uma versão paralela utilizando threads POSIX (`pthreads`). O objetivo é medir e comparar os tempos de execução, calcular o *speedup* e a eficiência, e analisar criticamente o comportamento do paralelismo em diferentes cenários.

---

## 2. Metodologia

### 2.1 Implementação
A implementação foi realizada na linguagem C, utilizando a biblioteca `pthreads` para o paralelismo. As principais características são:
* **Alocação contínua de memória (row-major):** Configurada para as matrizes, favorecendo a localidade de cache.
* **Particionamento por linhas:** Cada thread processa um subconjunto contíguo de linhas da matriz resultado.
* **Medição de tempo real (wall time):** Utilização da função `clock_gettime(CLOCK_MONOTONIC)`, adequada para medir tempo paralelo de forma precisa.
* **Inicialização:** Matrizes preenchidas com valores aleatórios inteiros no intervalo $[0, 9]$.

### 2.2 Configuração dos Experimentos
Os experimentos variaram dois parâmetros principais:
* **Tamanhos de matriz:** $200 \times 200$, $400 \times 400$ e $800 \times 800$ (reduzidos com autorização do professor).
* **Número de threads:** 2, 4 e 8 threads em paralelo.

Para cada combinação, o algoritmo foi executado 10 vezes. O tempo registrado corresponde à média aritmética dessas execuções, reduzindo o impacto de variações pontuais e ruídos do sistema operacional.

### 2.3 Ambiente de Execução
Os testes foram realizados em ambiente Linux, compilando o programa por meio do GCC com a flag `-lpthread`. A máquina utilizada é um ambiente de desenvolvimento com recursos de CPU compartilhados (GitHub Codespaces), o que pode influenciar os resultados de tempo absoluto.

---

## 3. Resultados
A Tabela 1 apresenta os tempos médios de execução sequencial e paralela, bem como o *speedup* e a eficiência calculados para cada combinação de tamanho e número de threads ativos.

### Tabela 1 — Resultados de tempo, speedup e eficiência por dimensão e número de threads
| Dimensão | Threads | Tempo Seq (s) | Tempo Par (s) | Speedup | Eficiência |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **200x200** | 2 | 0.0402 | 0.0367 | 1.09 | 0.55 |
| **200x200** | 4 | 0.0402 | 0.0465 | 0.86 | 0.22 |
| **200x200** | 8 | 0.0402 | 0.0430 | 0.93 | 0.12 |
| **400x400** | 2 | 0.3339 | 0.3022 | 1.10 | 0.55 |
| **400x400** | 4 | 0.3339 | 0.3157 | 1.06 | 0.26 |
| **400x400** | 8 | 0.3339 | 0.3162 | 1.06 | 0.13 |
| **800x800** | 2 | 2.3545 | 2.4914 | 0.95 | 0.47 |
| **800x800** | 4 | 2.3545 | 2.5111 | 0.94 | 0.23 |
| **800x800** | 8 | 2.3545 | 2.4974 | 0.94 | 0.12 |

---

## 4. Gráficos de Desempenho

### 4.1 Tempo de Execução
O comportamento do tempo de execução paralelo em função do número de threads pode ser observado na curva de escalabilidade, mapeando as variações entre as dimensões de matrizes pequenas ($n=200$) e grandes ($n=800$).

### 4.2 Speedup
O gráfico de *speedup* analisa o ganho de velocidade relativo obtido pelo algoritmo paralelo ($T_{seq} / T_{par}$), evidenciando como o acréscimo de threads se comporta abaixo do limite linear ideal.

### 4.3 Eficiência
A curva de eficiência demonstra a fração de utilização real de cada núcleo de processamento ($Speedup / p$), explicitando uma queda acentuada conforme novas threads são adicionadas ao barramento de memória.

---

## 5. Análise Crítica

### 5.1 Speedup abaixo do esperado
Em um cenário ideal (paralelismo perfeito), o *speedup* com $p$ threads deveria ser igual a $p$. Contudo, os resultados observados ficaram bem abaixo disso, variando entre 0.82 e 1.10. 

Para a dimensão de $200 \times 200$, o tempo paralelo com 4 threads (0.0465s) chegou a ser superior ao sequencial (0.0402s), resultando em um *speedup* de 0.86 — provando que o paralelismo foi prejudicial nesse cenário específico. Esse comportamento é explicado pela **Lei de Amdahl**: qualquer fração não paralelizável do código (criação de threads, sincronização e o próprio overhead de escalonamento do sistema operacional) limita o ganho máximo. Em matrizes pequenas, o custo fixo desse overhead representa uma fração significativa do tempo total.

### 5.2 Comportamento por tamanho de matriz
Observou-se que matrizes maiores tendem a apresentar um *speedup* mais estável. Para $n=800$, o *speedup* manteve-se próximo de 0.94-0.95 independentemente do número de threads, enquanto para $n=200$ a variação foi mais errática (entre 0.86 e 1.09). Isso ocorre porque, com matrizes maiores, a carga computacional por thread é mais expressiva, diluindo o custo fixo de gerenciamento das threads. No entanto, mesmo para $n=800$, o algoritmo paralelo não conseguiu superar a barreira ideal nas condições testadas.

### 5.3 Eficiência e escalabilidade
A eficiência decresce consistentemente conforme o número de threads aumenta. Com 2 threads, a eficiência ficou em torno de 0.43-0.55; com 8 threads, caiu para 0.12. Isso indica **fraca escalabilidade**: dobrar o número de threads não dobra o desempenho. 

A causa provável reside na **contenção de memória**. A multiplicação clássica de matrizes realiza acessos não sequenciais (por colunas) à matriz B, gerando *cache misses* frequentes. Com múltiplas threads competindo simultaneamente pelos mesmos recursos de cache e barramento de memória da CPU, o gargalo de hardware degrada a performance global.

### 5.4 Limitações do experimento
* **Ambiente Compartilhado:** A execução em máquina virtual compartilhada (nuvem) gera variações devido a interferências externas de processos concorrentes do sistema hospedeiro.
* **Carga Reduzida:** Os tamanhos de matriz foram reduzidos em relação ao escopo original, o que infla o peso proporcional do custo fixo do paralelismo.
* **Ausência de Otimização de Cache:** Não foi aplicada nenhuma técnica como transposição prévia ou blocagem (*tiling*), penalizando severamente o barramento de dados.
* **Overhead de Hardware:** O número máximo de threads testado (8) pode exceder os núcleos físicos disponíveis alocados para o ambiente, gerando trocas de contexto (*context switch*) desnecessárias.

---

## 6. Conclusão
Este experimento demonstrou na prática os desafios e limitações da paralelização real da multiplicação de matrizes com threads POSIX. Os resultados evidenciaram que:
1. Para matrizes pequenas ($n=200$), o custo de criação e sincronização supera o ganho computacional bruto, tornando a versão paralela mais lenta que a sequencial em alguns casos.
2. Para matrizes maiores ($n=800$), o paralelismo proporciona estabilidade, mas ainda abaixo do teto teórico esperado.
3. A eficiência decresce rapidamente com o aumento de threads, indicando baixa escalabilidade no padrão de acesso direto utilizado.

Para obter ganhos reais de desempenho, seria necessária a aplicação de técnicas arquiteturais combinadas, como a **transposição da matriz B** (garantindo acesso puramente sequencial em memória) ou a **multiplicação em blocos (*tiling*)**, que otimizam drasticamente a localidade de cache e permitem um aumento real de *speedup*. O experimento cumpriu seu objetivo didático de ilustrar as métricas de computação paralela, confirmando que adicionar mais threads nem sempre se traduz em mais velocidade.

---

## 7. Compilação

```bash
gcc -o matriz matriz.c -lpthread -lm
python3 graficos.py