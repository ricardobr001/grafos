/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Algoritmos em Grafos
 * Profa. Cândida Nunes da Silva
 *
 * Trabalho 03
 *
 * RA: 562262
 * Aluno: Ricardo Mendes Leal Junior
 * ========================================================================== */

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // INT_MAX

// Definindo as cores que um nó podera ter
#define BRANCO -5
#define CINZA -4
#define PRETO -3

typedef struct vertice
{
    int val;
    struct vertice *prox;
} Vertice;

void leituraGrafo(int n, int m);
void adicionaListaAdjacencia(Vertice *vet, int x, int y);
void liberaListaAdjacencia(Vertice *vet, int n);
void cfc(Vertice *grafo, Vertice *grafoInvertido, int n);
void dfs(Vertice *grafo, int *cor, int *d, int *f, int *pred, int n, int raiz);
void dfsAux(Vertice *grafo, int *cor, int *d, int *f, int *pred, int *tempo, int u);
int novaRaiz(int *f, int n);
int verificaPred(int *pred, int n, int raiz);

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

    // Alocando memória para o grafo normal
    Vertice *grafo = (Vertice*) calloc(n, sizeof(Vertice)); // Já setando os valores como NULL

    // Alocando memória para o grafo invertido
    Vertice *grafoInvertido = (Vertice*) calloc(n, sizeof(Vertice)); // Já setando os valores como NULL

    // Lendo os vértices e arestas
    for (i = 0 ; i < m ; i++)
    {
        scanf("%d %d %d", &x, &y, &z);

        if (z == 2) // A rua tem duas direções (ida e volta)
        {
            adicionaListaAdjacencia(grafo, x-1, y-1);
            adicionaListaAdjacencia(grafo, y-1, x-1); // Agora adicionamos o vértice y na lista de adjacência do vértice x

            // Inserindo no grafo invertido
            adicionaListaAdjacencia(grafoInvertido, y-1, x-1);
            adicionaListaAdjacencia(grafoInvertido, x-1, y-1); // Agora adicionamos o vértice y na lista de adjacência do vértice x
        }
        else // Só tem um sentido
        {
            adicionaListaAdjacencia(grafo, x-1, y-1);

            // Inserindo no grafo invertido
            adicionaListaAdjacencia(grafoInvertido, y-1, x-1);
        }
    }

    cfc(grafo, grafoInvertido, n);

    // Liberando memória alocada
    liberaListaAdjacencia(grafo, n);
    liberaListaAdjacencia(grafoInvertido, n);
    free(grafo);
    free(grafoInvertido);
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

/* Função que verifica se o grafo é fortemente conexo, printa 1 se for fortemente conexo, 0 caso contrário */
void cfc(Vertice *grafo, Vertice *grafoInvertido, int n)
{
    int i, tempo = 0, raiz = 0, auxRaiz;
    Vertice *aux;

    // Alocando memória para os vetores do grafo normal
    int *cor = (int*) malloc(n * sizeof(int)); // cor = |V|
    int *d = (int*) malloc(n * sizeof(int)); // d = |V|
    int *f = (int*) malloc(n * sizeof(int)); // f = |V|
    int *pred = (int*) malloc(n * sizeof(int)); // pred = |V|

    // Alocando memória para os vetores do grafo invertido
    int *corInv = (int*) malloc(n * sizeof(int)); // cor = |V|
    int *dInv = (int*) malloc(n * sizeof(int)); // d = |V|
    int *fInv = (int*) malloc(n * sizeof(int)); // f = |V|
    int *predInv = (int*) malloc(n * sizeof(int)); // pred = |V|

    // Inicializa os vetores antes da DFS
    for (i = 0 ; i < n ; i++)
    {
        cor[i] = BRANCO; // Nenhum vertice ainda foi visitado
        d[i] = -1; // Nenhum vértice foi descoberto
        f[i] = -1; // Nenhum vértice terminou de ser explorado
        pred[i] = -1; // Nenhum vértice tem predecessor

        corInv[i] = BRANCO; // Nenhum vertice ainda foi visitado
        dInv[i] = -1; // Nenhum vértice foi descoberto
        fInv[i] = -1; // Nenhum vértice terminou de ser explorado
        predInv[i] = -1; // Nenhum vértice tem predecessor
    }

    // dfs no grafo normal
    dfs(grafo, cor, d, f, pred, n, -1);

    // Se o grafo for conexo
    if (verificaPred(pred, n, raiz))
    {
        // raiz = novaRaiz(f, n);
        dfs(grafoInvertido, corInv, dInv, fInv, predInv, n, raiz);
        
        // Verifica se após a dfs no grafo invertido, se ele é conexo
        if (verificaPred(predInv, n, raiz))
        {
            printf("1\n");
        }
        else
        {
            printf("0\n");
        }
    }
    else
    {
        printf("0\n"); // Se for desconexo, imprime 0
    }

    // Liberando a memória alocada para os vetores do grafo normal e invertido
    free(cor);
    free(d);
    free(f);
    free(pred);

    free(corInv);
    free(dInv);
    free(fInv);
    free(predInv);
}

/* Função que executa a DFS para a raiz */
void dfs(Vertice *grafo, int *cor, int *d, int *f, int *pred, int n, int raiz)
{
    int i, tempo = 0, flag = 0; // O tempo inicia em 0
    Vertice *aux;

    // Se for a dfs do grafo normal
    if (raiz == -1)
    {
        // Percorre o vetor cor, executando a dfs no grafo
        // Enquanto não encontrar duas raizes
        for (i = 0 ; i < n ; i++)
        {
            // Se a cor do vértice for BRANCO e não tiver nenhuma raiz
            if (cor[i] == BRANCO)
            {
                // Se encontrou outro vértice branco, e a flag já tiver valor 1
                // O grafo normal possui duas raizes, então não é fortemente conexo
                if (flag)
                {
                    break;
                }

                dfsAux(grafo, cor, d, f, pred, &tempo, i); // Executa a dfs-aux
                flag++; // Incrementa que encontrou uma raiz
            }
        }
    }
    else // Se não, é dfs no grafo invertido
    {
        dfsAux(grafo, cor, d, f, pred, &tempo, raiz);
    }
}

/* Função que executa a DFS para os demais vértices */
void dfsAux(Vertice *grafo, int *cor, int *d, int *f, int *pred, int *tempo, int u)
{
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
            dfsAux(grafo, cor, d, f, pred, tempo, aux->val); // Explora o próximo vértice
        }

        // Caso não entre no if ou quando sair dele, o vértice já foi u explorado por completo
        cor[u] = PRETO; // A cor do vértice u se torna PRETO
        f[u] = *tempo; // O vértice u terminou de ser explorado no tempo x
        (*tempo)++; // Incrementamos o tempo
        aux = aux->prox;
    }
}

/* Função que descobre a nova raiz, que será utilizada na DFS do grafo transposto */
int novaRaiz(int *f, int n)
{
    int i, v, max = -1;

    // Laço que percorre o vetor f
    for (i = 0 ; i < n ; i++)
    {
        // Se o tempo final de exploração for maior que o maximo atual
        if (f[i] > max)
        {
            max = f[i]; // Salva o novo maximo
            v = i; // Salva o vértice v
        }
    }
    
    return v; // Retorna a nova raiz
}

/* Função que verifica se o grafo possui mais de uma raiz */
int verificaPred(int *pred, int n, int raiz)
{
    int i;
    
    // Laço que ira percorrer o vetor pred
    for (i = 0 ;  i < n ; i++)
    {
        // Se algum vértice além da raiz estiver sem predecessor
        if (pred[i] == -1 && i != raiz)
        {
            return 0; // Retorna que o grafo não é fortemente desconexo
        }
    }

    return 1; // Caso andar todo o vetor, o grafo é fortemente conexo
}