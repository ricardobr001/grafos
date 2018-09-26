#include <stdio.h>
#include <stdlib.h>

// Definindo as cores que um nó podera ter
#define BRANCO 0;
#define CINZA 1;
#define PRETO 2;

typedef struct vertice
{
    int val;
    struct vertice *prox;
} Vertice;

void leituraGrafo(int n, int m);
void adicionaListaAdjacencia(Vertice *vet, int x, int y);

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
    int i, x, y;

    // Alocando a lista de adjacencia já setando os valores como NULL
    Vertice *vet = (Vertice*) calloc(n, sizeof(Vertice));

    for (i = 0 ; i < m ; i++)
    {
        scanf("%d %d", &x, &y);
        adicionaListaAdjacencia(vet, x, y);
    }
}

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