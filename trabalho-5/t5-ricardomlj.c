/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Algoritmos em Grafos
 * Profa. Cândida Nunes da Silva
 *
 * Trabalho 05 - Orquideas
 *
 * RA: 562262
 * Aluno: Ricardo Mendes Leal Junior
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // INT_MAX

typedef struct vertice
{
    int val, custo;
    struct vertice *prox;
} Vertice;

typedef struct vetor
{
    int qtdd;
    int *q;
} Vetor;

void leituraGrafo(int n, int m, int k);
void adicionaListaAdjacencia(Vertice *vet, int x, int y, int c);
void liberaListaAdjacencia(Vertice *vet, int n);
void inicializa(int k, int n, int *dist, int *pred, int *solucao, Vetor *v);
void insereVetor(Vetor *vet, int v);
void dijkstra(Vertice *grafo, int *dist, int *pred, int *solucao, Vetor *vet, int n);
void imprimeSolucao(int k, int n, int *pred);
// void insereSolucao(Solucao *s, int v);

int main()
{
    int n, m, k;

    scanf("%d %d %d", &n, &m, &k);

    leituraGrafo(n, m, k);

    return 0;
}

/* Função que le o grafo, inicializa os vetores dist, pred, cor e fila e faz a busca */
void leituraGrafo(int n, int m, int k)
{
    // estufas = [0, k - 1]
    // centro de distribuição = [k, n - 1]
    int i, x, y, c, flag = 0;
    Vetor vet;
    Vertice *grafo = (Vertice*) calloc(n, sizeof(Vertice)); // Já setando os valores como NULL
    int *solucao = (int*) malloc(n * sizeof(int));
    int *dist = (int*) malloc(n * sizeof(int)); // dist = |V|
    int *pred = (int*) malloc(n * sizeof(int)); // pred = |V|
    vet.q = (int*) malloc(n * sizeof(int));
    // solucao.vet = (int*) malloc(n * sizeof(int)); // fila = |V|

    // Lendo os vértices e arestas
    for (i = 0 ; i < m ; i++)
    {
        scanf("%d %d %d", &x, &y, &c);
        adicionaListaAdjacencia(grafo, x, y, c);
        adicionaListaAdjacencia(grafo, y, x, c);
    }

    inicializa(k, n, dist, pred, solucao, &vet);
    dijkstra(grafo, dist, pred, solucao, &vet, n);
    imprimeSolucao(k, n, pred);

    // Liberando memória alocada
    liberaListaAdjacencia(grafo, n);
    free(grafo);
    free(dist);
    free(pred);
    free(solucao);
    free(vet.q);
    // free(fila.q);
}

/* Função que adiciona um novo vértice y na lista de adjacência do vértice x */
void adicionaListaAdjacencia(Vertice *vet, int x, int y, int c)
{
    Vertice *aux, *novo;
    novo = (Vertice*) calloc(1, sizeof(Vertice));
    novo->val = y;
    novo->custo = c;

    // Se não tiver inserido nenhum vertice adjacente para o vertice x
    if (vet[x].prox == NULL)
    {
        // Insere o y   
        vet[x].prox = novo;
    }
    else
    {
        // Caso contrário, andamos a lista e inserimos o y no final da lista
        aux = vet[x].prox;

        while (aux->prox != NULL)
        {
            aux = aux->prox;
        }

        aux->prox = novo;
    }
}

/* Função que libera a memória alocada para armazenar a lista de adjacência */
void liberaListaAdjacencia(Vertice *vet, int n)
{
    Vertice *aux, *proximo;
    int i;

    for (i = 0 ; i < n ; i++)
    {
        // Se existir alguém adjacente, liberamos a lista
        if (vet[i].prox != NULL)
        {
            aux = vet[i].prox;

            // Percorremos a lista, liberando os nós alocados
            while (aux->prox != NULL)
            {
                proximo = aux->prox;
                free(aux);
                aux = proximo;                
            }
            free(aux);
            vet[i].prox = NULL;
        }
    }
}

/* Função que inicia o vetor dist */
void inicializa(int k, int n, int *dist, int *pred, int *solucao, Vetor *v)
{
    int i;

    v->qtdd = 0;
    // s->qtdd = 0; // Inicialmente a solução está vazia
    // f->ultimo = 0;

    // As estufas iniciam com distancia 0 e já estão na solução
    for (i = 0 ; i < k ; i++)
    {
        solucao[i] = 1;
        dist[i] = 0;
        insereVetor(v, i);
        // insereSolucao(s, i);
        // solucao[i] = 1;
        // insereFila(f, i);
        pred[i] = i;
    }

    // Os centros de distribuição com INT_MAX
    for ( ; i < n ; i++)
    {
        dist[i] = INT_MAX;
        pred[i] = -1;
        solucao[i] = 0;
        // solucao[i] = 0;
    }
}

/* Função que insere um novo vértice na fila */
void insereVetor(Vetor *vet, int v)
{
    vet->q[vet->qtdd] = v;
    vet->qtdd++;
}

void dijkstra(Vertice *grafo, int *dist, int *pred, int *solucao, Vetor *vet, int n)
{
    int i, u, v, custoUV, minCusto, solucaoV, predSolucao, vezes;
    Vertice *aux;

    // Enquanto todos os vértices não entrarem na solução
    while (vet->qtdd != n)
    {
        minCusto = INT_MAX;
        vezes = vet->qtdd;

        // Anda o vetor de solução para atualizar o custo conhecido de U até V
        // Andando todas as arestas conhecidas
        for (i = 0 ; i < vezes ; i++)
        {
            // Recupera o vértice alcançado pelo o vértice do vetor
            u = vet->q[i];
            aux = grafo[u].prox;
            // printf("\n\nu = %d\n", u);
            // Anda a lista de adjacência caso exista
            while (aux != NULL)
            {
                v = aux->val;
                custoUV = aux->custo;

                // printf("Verificando v[%d] -- pred = %d\n", v, pred[v]);
                // printf("%d está na solucao - %d\n", v, solucao[v]);
                
                // Se o vértice alcançado não estiver na solução
                if (!solucao[v])
                {
                    // printf("dist[%d] = %d -> %d + %d (%d)\n", v, dist[v], dist[u], custoUV, dist[u] + custoUV);
                    dist[v] = dist[u] + custoUV; // Atualiza a distância conhecida até V
                        
                    // printf("u < minCusto? %d < %d\n", dist[v], minCusto);
                    if (dist[v] < minCusto)
                    {
                        // printf("Sim!!!\n");
                        minCusto = dist[v];
                        solucaoV = v;
                        predSolucao = pred[u];
                        // printf("pred[%d] = %d\n", solucaoV, predSolucao);
                        // printf("minCusto = %d\n", minCusto);
                    }
                    // Removendo o critério de desempate acerta mais casos na saida??????
                    else if (dist[v] == minCusto)
                    {
                        // printf("%d\n", pred[u]);
                        // printf("É igual!\n");
                        if (pred[u] < predSolucao)
                        {
                            predSolucao = pred[u];
                        }
                    }
                }
                // else { printf("Pulou, ja esta na solucao!\n"); }
                // printf("\n");
                aux = aux->prox;
            }

            // printf("===============\nTerminou de explorar o vértice %d\n===============\n\n", u);
        }
        // printf("Entra na solucao - %d\n", solucaoV);
        // printf("pred[%d] = %d\n", solucaoV, predSolucao);
        // printf("Custo = %d\n\n", minCusto);
        
        pred[solucaoV] = predSolucao; // Predecessor do centro de distribuição é a estufa i
        solucao[solucaoV] = 1; // V entra na solução
        insereVetor(vet, solucaoV); // Salvando no vetor solução
        // printf("Finalizou o for, recomeçando!\n");
    }
}

void imprimeSolucao(int k, int n, int *pred)
{
    for ( ; k < n - 1 ; k++)
    {
        printf("%d ", pred[k]);
    }
    printf("%d\n", pred[k]);
}