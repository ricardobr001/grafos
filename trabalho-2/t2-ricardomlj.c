/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Algoritmos em Grafos
 * Profa. Cândida Nunes da Silva
 *
 * Trabalho 02
 *
 * RA: 562262
 * Aluno: Ricardo Mendes Leal Junior
 * ========================================================================== */

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // INT_MAX

// Definindo as cores que um nó podera ter
#define BRANCO 0
#define CINZA 1
#define PRETO 2

typedef struct vertice
{
    int val;
    struct vertice *prox;
} Vertice;

typedef struct fila
{
    int primeiro, ultimo;
    int *q;
} Fila;

void leituraGrafo(int n, int m);
void adicionaListaAdjacencia(Vertice *vet, int x, int y);
void liberaListaAdjacencia(Vertice *vet, int n);
void insereFila(Fila *f, int v);
int removeFila(Fila *f);
void bfs(Vertice *grafo, Fila *f, int *cor, int *dist, int *pred, int v);
int verificaDist(int *dist, int tam);

int main()
{
    int n, m;

    scanf("%d %d", &n, &m);

    // Enquanto não ler que serão 0 vertices e 0 arestas, continua..
    while (n || m) 
    {
        leituraGrafo(n, m);
        scanf("%d %d", &n, &m);
    }

    return 0;
}

void leituraGrafo(int n, int m)
{
    int i, x, y, v, aux;
    Fila fila;
    Vertice *grafo = (Vertice*) calloc(n, sizeof(Vertice)); // Já setando os valores como NULL
    int *cor = (int*) malloc(n * sizeof(int)); // cor = |V|
    int *dist = (int*) malloc(n * sizeof(int)); // dist = |V|
    int *pred = (int*) malloc(n * sizeof(int)); // pred = |V|
    fila.q = (int*) malloc(n * sizeof(int)); // fila = |V|

    // Lendo os vértices e arestas
    for (i = 0 ; i < m ; i++)
    {
        scanf("%d %d", &x, &y);
        adicionaListaAdjacencia(grafo, x, y);
        adicionaListaAdjacencia(grafo, y, x); // Agora adicionamos o vértice y na lista de adjacência do vértice x
    }

    fila.primeiro = 0; // Inicialmente a fila está vazia
    fila.ultimo = 0;

    // Inicializa os vetores
    for (i = 1 ; i < n ; i++)
    {
        cor[i] = BRANCO; // Nenhum vertice ainda foi visitado
        dist[i] = INT_MAX; // A distância para todos os vértices é desconhecida
        pred[i] = -1; // Nenhum vértice tem predecessor
    }

    // Inicializa os vetores para a raiz, inicialização especial
    cor[0] = CINZA;
    dist[0] = 0;
    pred[0] = 1;

    insereFila(&fila, 0); // Inserindo a raiz na fila

    while (fila.primeiro != fila.ultimo) // Enquanto não chegar no fim da fila explora o grafo
    {
        v = removeFila(&fila); // Remove um vértice da fila
        bfs(grafo, &fila, cor, dist, pred, v); // Busca em largura dos nós vizinhos de v
        cor[v] = PRETO;
    }

    aux = verificaDist(dist, n);

    if (aux == -1) // Se encontrou que o grafo é desconexo
    {
        printf("infinito\n");
    }
    else
    {
        printf("%d\n", aux);
    }

    // Liberando memória alocada
    liberaListaAdjacencia(grafo, n);
    free(grafo);
    free(cor);
    free(dist);
    free(pred);
    free(fila.q);
}

/* Função que adiciona um novo vértice y na lista de adjacência do vértice x */
void adicionaListaAdjacencia(Vertice *vet, int x, int y)
{
    Vertice *aux, *novo;
    novo = (Vertice*) calloc(1, sizeof(Vertice));
    novo->val = y;

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

/* Função que insere um novo vértice na fila */
void insereFila(Fila *f, int v)
{
    f->q[f->ultimo] = v;
    f->ultimo++;
}

/* Função que remove um vértice da fila */
int removeFila(Fila *f)
{
    int aux = f->q[f->primeiro];
    f->primeiro++;

    return aux;
}

/* Busca em largura, a função explora o grafo marcando a distância da raiz até o nó sendo explorado atualmente (v) */
void bfs(Vertice *grafo, Fila *f, int *cor, int *dist, int *pred, int v)
{
    Vertice *aux = grafo[v].prox;

    while (aux != NULL)
    {
        if (cor[aux->val] == BRANCO) // Se o vértice descoberto for branco
        {
            dist[aux->val] = dist[v] + 1; // A distância do vértice descoberto, é a distância do seu antecessor + 1
            pred[aux->val] = v; // O predecessor do vértice atual, é seu antecessor
            cor[aux->val] = CINZA; // A cor do vértice descoberto se torna CINZA
            insereFila(f, aux->val); // Inserimos o vértice atual na fila
        }

        aux = aux->prox; // Após adicionar na fila o vértice branco, anda a lista de adjacência
    }
}

/* Função que verifica se o vetor distância possui algum nó não explorado, se encontrar, retorna -1, se não retorna a maior distância */
int verificaDist(int *dist, int tam)
{
    int i, aux = 0;

    for (i = 0 ; i < tam ; i++)
    {
        // printf("dist[%d] = %d\n", i, dist[i]);
        if (dist[i] > aux) // Se a distância do vértice i for maior aux
        {
            aux = dist[i]; // Atualizamos o valor de aux
        }
        if (dist[i] == INT_MAX) // Se o valor da distância para o vértice i for INT_MAX
        {
            return -1; // Encontramos que o grafo é desconexo
        }
    }

    return aux;
}