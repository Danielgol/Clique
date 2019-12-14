#include <stdio.h>
#include <stdlib.h>

typedef struct{
    //No
    int index;//Id do no
    int status; //(No clique, serve para indicar se eh candidato ao clique)
    int estimativa; //(No clique, serve para salvar o grau)
    struct Out* saidas; //Nos que saem desse no
    struct Node* precedente; //Serve para unir o clique)
    //Grafo
    struct Node* prox;//Proximo na lista de nos (geral)
}Node;

typedef struct{
    int distancia;
    struct Node* node;
    struct Out* prox;
}Out;



void zerarNodes(Node* grafo){
    if(grafo != NULL){
        grafo->estimativa = 1000;
        grafo->precedente = NULL;
        grafo->status = 0;
        zerarNodes(grafo->prox);
    }
}

Node* buscarNode(Node* atual, int index){
    if(atual == NULL){
        return NULL;
    }
    if(atual->index == index){
        return atual;
    }else{
        return buscarNode(atual->prox, index);
    }
}

Node* inserir(Node* atual, int index){
    if(atual == NULL){
        Node* novo = malloc(sizeof(Node));
        novo->estimativa = 1000;
        novo->index = index;
        novo->saidas = NULL;
        novo->precedente = NULL;
        novo->prox = NULL;
        novo->status = 0;
        return novo;
    }

    Node* busca = buscarNode(atual, index);
    if(busca != NULL){
        return atual;
    }

    atual->prox = inserir(atual->prox, index);
    return atual;
}

Out* inserirNasSaidas(Out* atual, Node* node, int distancia){
    if(atual == NULL){
        Out* novo = malloc(sizeof(Out));
        novo->node = node;
        novo->prox = NULL;
        novo->distancia = distancia;
        return novo;
    }

    if(atual->distancia < distancia){
        Out* novo = malloc(sizeof(Out));
        novo->node = node;
        novo->prox = atual;
        novo->distancia = distancia;
        return novo;
    }else{
        atual->prox = inserirNasSaidas(atual->prox, node, distancia);
        return atual;
    }
}

void ligarNodes(Node* grafo, int index1, int index2, int distancia){
    Node* node1 = buscarNode(grafo, index1);
    Node* node2 = buscarNode(grafo, index2);
    if(node1 != NULL && node2 != NULL){
        node1->saidas = inserirNasSaidas(node1->saidas, node2, distancia);
        node2->saidas = inserirNasSaidas(node2->saidas, node1, distancia);//MAO DUPLA
    }
}

void imprimirClique(Node* atual){
    if(atual == NULL){
        printf("CLIQUE: -");
    }else{
        imprimirClique(atual->precedente);
        printf("|%i|-", atual->index);
    }
}

void fecharNodes(Out* atual){
    if(atual != NULL){
        Node* node = atual->node;
        node->status = 1;
        fecharNodes(atual->prox);
    }
}

int calcularGrau(Out* saida, Node* selecionado, int soma){
    if(saida != NULL){
        Node* node = saida->node;
        if(node->status == 1 && node != selecionado){
            soma += 1;
        }
        return calcularGrau(saida->prox, selecionado, soma);
    }
    return soma;
}

void estimarSubgrafo(Out* atual, Node* selecionado, int numero){
    if(atual != NULL){
        Node* node = atual->node;
        Out* saidas = node->saidas;
        node->estimativa = calcularGrau(node->saidas, selecionado, 0);
        if(node->estimativa < numero-2){
            node->status = 0;
            node->estimativa = 0;
        }
        estimarSubgrafo(atual->prox, selecionado, numero);
    }
}

int verificarConexoes(Out* lista_saidas, Out* lista_node){

    int soma = 0;
    Out* aux_ls = lista_saidas;

    while(lista_node != NULL){
        Node* atual_node = lista_node->node;

        while(aux_ls != NULL){
            Node* atual_saidas = aux_ls->node;
            if(atual_node == atual_saidas && atual_node->status == 1 && atual_saidas->status == 1){
                //printf("\n%i == %i", atual_saidas->index, atual_node->index);
                soma += 1;
            }
            aux_ls = aux_ls->prox;
        }

        aux_ls = lista_saidas;
        lista_node = lista_node->prox;
    }

    return soma;
}

void separarClique(Out* atual, int numero){
    if(atual != NULL){
        Node* node = atual->node;
        Out* saidas = node->saidas;

        if(node->status == 1){
            int conexoes = 0;

            while(saidas != NULL){
                Node* saida = saidas->node;
                if(saida->status == 1){
                    conexoes += verificarConexoes(node->saidas, saida->saidas);
                }
                if(saida == node){
                    conexoes += 1;
                }
                saidas = saidas->prox;
            }

            if(conexoes <= numero-2){ //conexoes == 0 (tirar numero-2)
                node->status = 0;
                node->estimativa = 0;
            }else{
                //printf("\nPASSEI %i", node->index);
                node->estimativa = conexoes/2;//conexoes/2
            }
        }

        separarClique(atual->prox, numero);
    }
}

Node* juntarClique(Out* atual){
    if(atual != NULL){
        Node* node = atual->node;
        if(node->estimativa > 0){
            node->precedente = juntarClique(atual->prox);
            return node;
        }else{
            return juntarClique(atual->prox);
        }
    }
    return NULL;
}

Node* acharClique(Node* selecionado, int numero){
    fecharNodes(selecionado->saidas);
    estimarSubgrafo(selecionado->saidas, selecionado, numero);

    /*
    Out* saidas = selecionado->saidas;
    while(saidas != NULL){
        Node* node = saidas->node;
        if(node->status == 1){
            printf("\n|%i|", node->index);
        }
        saidas = saidas->prox;
    }
    */

    //selecionado->status = 0;
    selecionado->status = 1;
    separarClique(selecionado->saidas, numero);
    selecionado->precedente = juntarClique(selecionado->saidas);

    return selecionado;
}


int main(){


    Node* grafo = NULL;

    grafo = inserir(grafo, 1);
    grafo = inserir(grafo, 2);
    grafo = inserir(grafo, 3);
    grafo = inserir(grafo, 4);
    grafo = inserir(grafo, 5);
    grafo = inserir(grafo, 6);
    grafo = inserir(grafo, 7);
    grafo = inserir(grafo, 8);
    grafo = inserir(grafo, 9);
    grafo = inserir(grafo, 10);
    grafo = inserir(grafo, 11);
    grafo = inserir(grafo, 12);
    grafo = inserir(grafo, 13);
    grafo = inserir(grafo, 14);
    grafo = inserir(grafo, 15);
    grafo = inserir(grafo, 16);
    grafo = inserir(grafo, 17);
    grafo = inserir(grafo, 18);
    grafo = inserir(grafo, 19);
    grafo = inserir(grafo, 20);
    grafo = inserir(grafo, 21);

    ligarNodes(grafo, 1, 2, 8);
    ligarNodes(grafo, 1, 3, 10);
    ligarNodes(grafo, 2, 3, 8);
    ligarNodes(grafo, 2, 4, 14);
    ligarNodes(grafo, 3, 4, 10);
    ligarNodes(grafo, 4, 5, 12);
    ligarNodes(grafo, 4, 6, 16);
    ligarNodes(grafo, 5, 6, 10);
    ligarNodes(grafo, 5, 10, 6);
    ligarNodes(grafo, 5, 11, 6);
    ligarNodes(grafo, 6, 7, 8);
    ligarNodes(grafo, 6, 9, 6);
    ligarNodes(grafo, 7, 8, 6);
    ligarNodes(grafo, 9, 14, 6);
    ligarNodes(grafo, 9, 15, 4);
    ligarNodes(grafo, 10, 12, 6);
    ligarNodes(grafo, 10, 14, 6);
    ligarNodes(grafo, 10, 16, 4);
    ligarNodes(grafo, 11, 12, 6);
    ligarNodes(grafo, 12, 13, 8);
    ligarNodes(grafo, 12, 16, 6);
    ligarNodes(grafo, 14, 16, 6);
    ligarNodes(grafo, 14, 18, 8);
    ligarNodes(grafo, 15, 18, 10);
    ligarNodes(grafo, 16, 17, 6);
    ligarNodes(grafo, 16, 18, 10);
    ligarNodes(grafo, 16, 19, 8);
    ligarNodes(grafo, 16, 20, 8);
    ligarNodes(grafo, 17, 18, 6);
    ligarNodes(grafo, 17, 19, 6);
    ligarNodes(grafo, 18, 19, 10);
    ligarNodes(grafo, 19, 20, 8);
    ligarNodes(grafo, 20, 21, 22);

    Node* clique = NULL;

    clique = acharClique(buscarNode(grafo, 16), 4);
    printf("(16,4) 4: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 16), 3);
    printf("(16,3) 8: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 4), 3);
    printf("(4,3) 5: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 10), 4);
    printf("(10,4) 1: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 10), 3);
    printf("(10,3) 4: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 7), 3);
    printf("(7,3) 1: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 15), 3);
    printf("(15,3) 1: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 20), 3);
    printf("(20,3) 3: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 14), 3);
    printf("(14,3) 4: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 1), 3);
    printf("(1,3) 3: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    clique = acharClique(buscarNode(grafo, 3), 3);
    printf("(3,3) 4: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");


    printf("\nADICIONANDO ARESTAS:\n\n");
    ligarNodes(grafo, 5, 14, 10);
    ligarNodes(grafo, 10, 11, 10);
    printf("5-14\n10-11\n");
    clique = acharClique(buscarNode(grafo, 10), 4);
    printf("(10,4) 1: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n");

    clique = acharClique(buscarNode(grafo, 10), 3);
    printf("(10,3) 6: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    ligarNodes(grafo, 5, 12, 10);
    printf("5-12\n");
    clique = acharClique(buscarNode(grafo, 10), 4);
    printf("(10,4) 4: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    ligarNodes(grafo, 14, 12, 10);
    printf("12-14\n");
    clique = acharClique(buscarNode(grafo, 10), 4);
    printf("(10,4) 6: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n");

    //CLIQUE DE 5 (5-14 JA FOI LIGADO)
    ligarNodes(grafo, 10, 6, 10);
    ligarNodes(grafo, 10, 9, 10);
    ligarNodes(grafo, 6, 14, 10);
    ligarNodes(grafo, 5, 9, 10);
    printf("\n5-9\n6-10\n6-14\n9-10\n");
    clique = acharClique(buscarNode(grafo, 10), 4);
    printf("(10,4) 8: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n");

    clique = acharClique(buscarNode(grafo, 10), 5);
    printf("(10,5) 5: ");
    imprimirClique(clique);
    zerarNodes(grafo);
    printf("\n\n");

    return 0;
}
