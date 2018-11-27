/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA:          Pietro Zuntini Bonfim
 * Aluno:       743588
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY     11
#define TAM_NOME            51
#define TAM_MARCA           51
#define TAM_DATA            11
#define TAM_ANO             3
#define TAM_PRECO           8
#define TAM_DESCONTO        4
#define TAM_CATEGORIA       51

#define TAM_REGISTRO        192
#define MAX_REGISTROS       1000
#define TAM_ARQUIVO         (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA          "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE    "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO   "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO          "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA        "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO           "Arquivo vazio!"
#define INICIO_BUSCA            "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM         "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO        "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO          "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO         "**********************EXCLUIR*********************\n"
#define SUCESSO                 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                   "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO       "Registro %s inserido com sucesso.\n\n"

/* Registro do produto */
typedef struct produto {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA]; /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
    char pk[TAM_PRIMARY_KEY];
    int rrn;
    struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct hashtable {
    int tam;
    Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o produto */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char *chave, int tam);  //todo

/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a);

/*Funções do Menu*/

//OK
void liberar_tabela(Hashtable *tabela);
void cadastrar(Hashtable *tabela);
void buscar(Hashtable tabela);
int alterar(Hashtable tabela);

//todo
void carregar_tabela(Hashtable *tabela);
int remover(Hashtable *tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */

// Inicializacao da tabela hash
void criar_tabela(Hashtable *tabela, int tam);

// Impressao da tabela hash
void imprimir_tabela(Hashtable tabela);

// Insercoes na lista de chaves
void inserir_lista(Chave **primeiro, char *chave);

// Busca na lista de chaves
int buscar_lista(Chave **primeiro, char *chave);



/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main()
{

    /* Arquivo */
    int carregarArquivo = 0;
    scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
    // if (carregarArquivo) //todo
        // carregar_arquivo(); //todo

    /* Tabela Hash */
    int tam;
    scanf("%d%*c", &tam);
    tam = prox_primo(tam);

    Hashtable tabela;
    criar_tabela(&tabela, tam);
    // if (carregarArquivo) //todo
    //     carregar_tabela(&tabela); //todo

    /* Execução do programa */
    int opcao = 0;
    while (opcao != 6) {

        scanf("%d%*c", &opcao);
        switch (opcao) {

        case 1:
            cadastrar(&tabela);
            break;
        
        case 2:
            printf(INICIO_ALTERACAO);
            if (alterar(tabela))
                printf(SUCESSO);
            else
                printf(FALHA);
            break;
        
        case 3:
            printf(INICIO_BUSCA);
            buscar(tabela);
            break;
        
        case 4:
            printf(INICIO_EXCLUSAO);
            printf("%s", (remover(&tabela)) ? SUCESSO : FALHA); //todo
            break;
        
        case 5:
            printf(INICIO_LISTAGEM);
            imprimir_tabela(tabela); //todo
            break;
        
        case 6:
            liberar_tabela(&tabela);
            break;

        
        case 10:
            printf(INICIO_ARQUIVO);
            printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
            break;

        default:
            printf(OPCAO_INVALIDA);
            break;
        }
    }
    return 0;
}



/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */



/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo()
{
    scanf("%[^\n]\n", ARQUIVO);
}


/* ============================================================================================
   ================================== FUNCOES DE HASH =========================================
   ============================================================================================ */

/***************************** Auxiliar para a função de hash ***********************************/
short f(char x) {
    return (x < 59) ? x - 48 : x - 54;
}

/************************************ Funcao de Hash ********************************************/
short hash(const char *chave, int tam) {
    int resultado = 0;
    for (int i = 0; i < 8; i++)
        resultado += (i+1) * f(chave[i]);
    return resultado % tam;
}


/* ============================================================================================
   ============================= RECUPERAR E EXIBIR REGISTRO ==================================
   ============================================================================================ */

/* Recupera o registro no ARQUIVO de dados e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;

	// Recebe os dados da string temp retirada do ARQUIVO de dados em determinado RRN
	p = strtok(temp, "@");
	strcpy(j.pk, p);
	p = strtok(NULL, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.marca, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.ano, p);
	p = strtok(NULL, "@");
	strcpy(j.preco, p);
	p = strtok(NULL, "@");
	strcpy(j.desconto, p);
	p = strtok(NULL, "@");
	strcpy(j.categoria, p);

    return j;
}

/* Exibe o produto */
int exibir_registro(int rrn)
{
    if (rrn < 0)
        return 0;
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    printf("%s\n", j.ano);
    sscanf(j.desconto, "%d", &desconto);
    sscanf(j.preco, "%f", &preco);
    preco = preco * (100 - desconto);
    preco = ((int)preco) / (float)100;
    printf("%07.2f\n", preco);
    strcpy(categorias, j.categoria);
    for (cat = strtok(categorias, "|"); cat != NULL; cat = strtok(NULL, "|"))
        printf("%s ", cat);
    printf("\n");
    return 1;
}

/* ============================================================================================
   ================================ FUNCOES AUXILIARES ========================================
   ============================================================================================ */


int eh_primo(int n) {
    int cont = 0;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0)
            cont++;
    }
    if (cont == 2)
        return 1;
    else
        return 0;
}
int prox_primo(int a) {
    while (!eh_primo(a))
        a++;
    return a;
}


/* ============================================================================================
   =================================== INICIALIZACAO ==========================================
   ============================================================================================ */

void criar_tabela(Hashtable *tabela, int tam) {

    // Inicializa o tamanho
    tabela->tam = tam;

    // Inicializa a tabela
    tabela->v = (Chave**) malloc(tam * sizeof(Chave));

    // // Inicializa as listas de chaves de cada posição da tabela hash
    // for (int i = 0; i < tam; i++) {
    //     tabela->v[i] = (Chave*) malloc(sizeof(Chave));
    //     memset(tabela->v[i]->pk, 0, TAM_PRIMARY_KEY);
    //     tabela->v[i]->rrn = -1;
    // }

}


/* ============================================================================================
   ====================================== IMPRESSAO ===========================================
   ============================================================================================ */

void imprimir_tabela(Hashtable tabela) {

    for (int i = 0; i < tabela.tam; i++) {

        printf("[%d]", i);

        Chave *atual = tabela.v[i];
        while (atual) {
            printf(" %s", atual->pk);
            atual = atual->prox;
        }
        printf("\n");

    }

}


/* ============================================================================================
   ====================================== LIBERACAO ===========================================
   ============================================================================================ */

void liberar_tabela(Hashtable *tabela) {

    for (int i = 0; i < tabela->tam; i++) {
        Chave *atual = tabela->v[i];
        Chave *temp;
        while (atual) {
            temp = atual;
            atual = atual->prox;
            free(temp);
        }
        atual = NULL;
    }
    free(tabela->v);

}


/* ============================================================================================
   ================================= INTERACAO COM O USUARIO ==================================
   ============================================================================================ */


/***************************************** CADASTRO *********************************************/

// Gera a chave primária 
void gerar_chave(Produto *P) {
    P->pk[0] = '\0';                // Garante que os dados serão concatenados corretamente na chave primária
    strncat(P->pk, P->nome, 2);     // N1N2 
    strncat(P->pk, P->marca, 2);    // M1M2
 
    char *dAux;                     // DDMM
    char dataAux[11];               // Cria uma string dataAux para não perder o valor da data original com o strtok
    strcpy(dataAux, P->data);
    dAux = strtok(dataAux, "/");    // DD
    strncat(P->pk, dAux, 2);
    dAux = strtok(NULL, "/");       // MM
    strncat(P->pk, dAux, 2);
    
    strncat(P->pk, P->ano, 2);      // AL
}
void ler_entrada(Produto *novo) {
    scanf("%[^\n]s", novo->nome);
    getchar();
    scanf("%[^\n]s", novo->marca);
    getchar();
    scanf("%[^\n]s", novo->data);
    getchar();
    scanf("%[^\n]s", novo->ano);
    getchar();
    scanf("%[^\n]s", novo->preco);
    getchar();
    scanf("%[^\n]s", novo->desconto);
    getchar();
    scanf("%[^\n]s", novo->categoria);
    getchar();
}
void cadastrar(Hashtable *tabela) {

    char entrada[193];
	Produto novo;


	// Lê a entrada do usuário e gera a chave primária
	ler_entrada(&novo);
	gerar_chave(&novo);


    // Verifica se já não existe uma chave igual
    int posicao = hash(novo.pk, tabela->tam);
    int resultadoBusca = buscar_lista(&(tabela->v[posicao]), novo.pk);
    if (resultadoBusca != -1) {
        printf(ERRO_PK_REPETIDA, novo.pk);
        return;   
    }


    // Coloca os dados na string entrada[]
	sprintf(entrada, "%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);

    // Completa os espaços restantes com '#'
    int necessarios = 192 - strlen(entrada);
    for (int i = 0; i < necessarios; i++) {
        strcat(entrada, "#");
    }

	// Coloca a entrada no ARQUIVO de dados
	strcat(ARQUIVO, entrada);


    /***** Insere na lista da posição encontrada *****/
    inserir_lista(&(tabela->v[posicao]), novo.pk);
    printf(REGISTRO_INSERIDO, novo.pk);

    nregistros++;
    
}


void inserir_lista(Chave **primeiro, char *chave) {

    /* Caso lista vazia ou o primeiro ja eh maior que a chave a ser inserida */
    if ((*primeiro) == NULL || strcmp((*primeiro)->pk, chave) > 0) {
        Chave *novo = (Chave*) malloc(sizeof(Chave));
        strcpy(novo->pk, chave);
        novo->rrn = nregistros;
        novo->prox = (*primeiro);
        (*primeiro) = novo;
        return;
    }

    /* Caso meio da lista */
    Chave *aux = (*primeiro);
    while (aux->prox && strcmp(aux->prox->pk, chave) < 0)
        aux = aux->prox;
    
    /* Se eh igual nao insere repetido */
    if (aux->prox && strcmp(aux->pk, chave) == 0)
        return;

    Chave *novo = (Chave*) malloc(sizeof(Chave));
    strcpy(novo->pk, chave);
    novo->prox = aux->prox;
    aux->prox = novo;


}


/****************************************** BUSCA ***********************************************/

int buscar_lista(Chave **primeiro, char *chave) {

    Chave *aux = *primeiro;
    while (aux) {
        if (strcmp(aux->pk, chave) == 0)
            return aux->rrn;
        aux = aux->prox;
    }
    return -1;

}
void buscar(Hashtable tabela) {

    // Recebe a chave primária
    char chave[TAM_PRIMARY_KEY];
    scanf("%[^\n]s", chave);
    getchar();

    // Verifica se a tabela não está vazia
    if (nregistros == 0) {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    int posicao = hash(chave, tabela.tam);
    int resultadoBusca = buscar_lista(&(tabela.v[posicao]), chave);
    if (resultadoBusca == -1) {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }
    else {
        exibir_registro(resultadoBusca);
    }



}


/**************************************** ALTERACAO *********************************************/

int alterar(Hashtable tabela) {

    char chave[TAM_PRIMARY_KEY];
    char novoDesconto[TAM_DESCONTO];
 
    // Recebe a chave primária
    scanf("%[^\n]s", chave);
    getchar();


    // Busca se existe a chave primaria

    int posicao = hash(chave, tabela.tam);
    int resultadoBusca = buscar_lista(&(tabela.v[posicao]), chave);
    if (resultadoBusca == -1) {
        // Nao encontrou
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }


    // Recebe o novo desconto
    scanf("%[^\n]s", novoDesconto);
    getchar();

    // Verifica se o novo desconto é válido (está entre 0 e 100), caso contrário pede novamente
    while (strcmp(novoDesconto, "100") > 0 || strcmp(novoDesconto, "000") < 0) {
        printf(CAMPO_INVALIDO);
        scanf("%[^\n]s", novoDesconto);
        getchar();
    }


    // Altera o desconto no ARQUIVO
    char *p = ARQUIVO + 192*(resultadoBusca);
 
    int arr = 0;
    while (*p && arr < 6) {
        if (*p == '@')
            arr++;
        p++;
    }
 
    // Altera o desconto
    *p = novoDesconto[0];
    p++;
    *p = novoDesconto[1];
    p++;
    *p = novoDesconto[2];
 
    return 1;

}


/***************************************** REMOCAO **********************************************/

int remover_lista(Chave **primeiro, char *chave) {

    if ((*primeiro) == NULL)
        return 0;
    Chave *atual = *primeiro;
    Chave *anterior = NULL;

    while (atual) {
        printf("atual: %s\n", atual->pk);
        if (strcmp(atual->pk, chave) == 0) {
            printf("a ser removido: %s\n", atual->pk);
            // Achou a chave
            if (atual == *primeiro) {
                if (atual->prox == NULL) {
                    free(atual);
                    *primeiro = NULL;
                    return 1;
                }
                else {
                    *primeiro = atual->prox;
                    free(atual);
                    return 1;
                }
            }
            else {
                anterior->prox = atual->prox;
                free(atual);
                return 1;
            }
            anterior = atual;
            atual = atual->prox;
        }
    }
    return 0;

}
int remover(Hashtable *tabela) {

    char chave[TAM_PRIMARY_KEY];
 
    // Recebe a chave primária
    scanf("%[^\n]s", chave);
    getchar();


    // Verifica se o registro existe
    int posicao = hash(chave, tabela->tam);
    
    int resultadoRemocao = remover_lista(&(tabela->v[posicao]), chave);
    if (resultadoRemocao == 0) {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }


    return 1;

}