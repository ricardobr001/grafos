/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Algoritmos em Grafos
 * Profa. Cândida Nunes da Silva
 *
 * Trabalho 04
 *
 * RA: 562262
 * Aluno: Ricardo Mendes Leal Junior
 * ========================================================================== */

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>

// Definindo as cores que um nó podera ter
#define BRANCO -5
#define CINZA -4
#define PRETO -3

typedef struct vertice
{
    int val;
    struct vertice *prox;
} Vertice;

typedef struct aresta
{
    int u, v;
} Aresta;

typedef struct pilha
{
    int teto;
    Aresta *q;
} Pilha;

void leituraGrafo(int n, int m);
void adicionaListaAdjacencia(Vertice *vet, int x, int y);
void liberaListaAdjacencia(Vertice *vet, int n);
void topSort(Vertice *grafo, int n, int *tempoProjeto);
void dfs(Vertice *grafo, Pilha *fila, int *cor, int *d, int *f, int *pred, int n);
int dfsAux(Vertice *grafo, Pilha *fila, int *cor, int *d, int *f, int *pred, int *tempo, int u);
void inserePilha(Pilha *p, Aresta a);
Aresta removePilha(Pilha *p);
int tempoMinimo(Pilha *p, int *tempoProjeto, int tam);

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

/* Função que le o grafo, inicializa os vetores dist, pred, cor e fila e faz a busca */
void leituraGrafo(int n, int m)
{
    int i, x, y, z;
    Vertice *grafo = (Vertice*) calloc(n, sizeof(Vertice)); // Já setando os valores como NULL
    int *tempoProjeto = (int*) malloc(n * sizeof(int)); // tempoProjeto = |V|

    // Lendo o peso de cada projeto
    for(i = 0 ; i < n ; i++)
    {
        scanf("%d", &tempoProjeto[i]);
    }

    // Lendo os vértices e arestas
    for (i = 0 ; i < m ; i++)
    {
        scanf("%d %d", &x, &y);

        // Inserindo na lista de adjacencia
        adicionaListaAdjacencia(grafo, x-1, y-1);
    }

    // Ordenação topológica
    topSort(grafo, n, tempoProjeto);

    // Liberando memória alocada
    liberaListaAdjacencia(grafo, n);
    free(grafo);
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

/* Função que faz a ordenação topológica do grafo */
void topSort(Vertice *grafo, int n, int *tempoProjeto)
{
    int i, tempo = 0, raiz = 0, auxRaiz;
    Pilha pilha;
    Vertice *aux;

    // Alocando memória para os vetores do grafo
    pilha.q = (Aresta*) malloc((n-1) * sizeof(Aresta)); // a = |V| - 1 (A árvore terá n-1 arestas no máximo)
    int *cor = (int*) malloc(n * sizeof(int)); // cor = |V|
    int *d = (int*) malloc(n * sizeof(int)); // d = |V|
    int *f = (int*) malloc(n * sizeof(int)); // f = |V|
    int *pred = (int*) malloc(n * sizeof(int)); // pred = |V|

    // Inicialmente a pilha está vazia
    pilha.teto = 0;

    // Inicializa os vetores antes da DFS
    for (i = 0 ; i < n ; i++)
    {
        cor[i] = BRANCO; // Nenhum vertice ainda foi visitado
        d[i] = -1; // Nenhum vértice foi descoberto
        f[i] = -1; // Nenhum vértice terminou de ser explorado
        pred[i] = -1; // Nenhum vértice tem predecessor
    }

    // dfs no grafo
    dfs(grafo, &pilha, cor, d, f, pred, n);

    // Faz a soma dos tempos
    tempoMinimo(&pilha, tempoProjeto, (n - 1));

    // Liberando a memória alocada para os vetores do grafo normal e invertido
    free(cor);
    free(d);
    free(f);
    free(pred);
    free(pilha.q);
}

/* Função que executa a DFS para a raiz */
void dfs(Vertice *grafo, Pilha *pilha, int *cor, int *d, int *f, int *pred, int n)
{
    int i, tempo = 0, flag = 0; // O tempo inicia em 0
    Aresta a;
    Vertice *aux;

    // Executa a DFS para todo nó ainda marcado como BRANCO (Podemos ter várias raízes para a árvore)
    for (i = 0 ; i < n ; i++)
    {
        if (cor[i] == BRANCO)
        {
            a.v = dfsAux(grafo, pilha, cor, d, f, pred, &tempo, i);

            a.u = i;
            inserePilha(f, a); // Inserindo na pilha a aresta de u do vértice v que acabou de ser explorado
        }
    }
}

/* Função que executa a DFS para os demais vértices, retorna o último vértice que foi explorado */
int dfsAux(Vertice *grafo, Pilha *pilha, int *cor, int *d, int *f, int *pred, int *tempo,  int u)
{
    Aresta a;
    Vertice *aux = grafo[u].prox; // aux recebe o primeiro vértice da lista de adj de u

    cor[u] = CINZA; // u acabou de ser descoberto
    d[u] = *tempo; // u foi descoberto no tempo x
    (*tempo)++; // Incrementa o tempo

    // Enquanto não explorar todos os vértices que u alcança
    while (aux != NULL)
    {
        // Se o vértice explorado atualmente não tiver sido explorado
        if (cor[aux->val] == BRANCO)
        {
            pred[aux->val] = u; // O predecessor do vértice descoberto se torna o vértice u
            a.v = dfsAux(grafo, pilha, cor, d, f, pred, tempo, aux->val); // Explora o próximo vértice

            a.u = u;
            inserePilha(f, a); // Inserindo na pilha a aresta de u do vértice v que acabou de ser explorado
        }
    }

    // Quando o vértice terminar de ser explorado
    cor[u] = PRETO; // A cor do vértice u se torna PRETO
    f[u] = *tempo; // O vértice u terminou de ser explorado no tempo x
    (*tempo)++; // Incrementamos o tempo
    aux = aux->prox;

    return u; // Retorna o vértice que acabou de ser explorado
}

/* Função que insere uma nova aresta na fila */
void inserePilha(Pilha *p, Aresta a)
{
    p->q[p->teto] = a;
    p->teto++;
}

/* Função que remove uma aresta da fila */
Aresta removePilha(Pilha *p)
{
    Aresta aux = p->q[p->teto - 1];
    p->teto--;

    return aux;
}

int tempoMinimo(Pilha *p, int *tempoProjeto, int tam)
{
    int i, j, v = 0, tempoAux, tempoMin = 0;
    Aresta auxAresta;

    // Laço que irá percorrer a pilha
    for (i = 0 ; i < tam ; i++)
    {
        auxAresta = removePilha(p); // Recuperando o topo da pilha
        tempoAux = tempoProjeto[auxAresta.u];
    }
}