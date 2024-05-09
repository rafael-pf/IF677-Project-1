/*ATENÇAO: Os arquivos que possam ser usados como teste deverão terminar com '$' e não possuir esse caracter no restante
do texto. A lógica usada foi passar cada texto pra uma thread e fazer uma busca de caracter por caracter buscando uma 
correspondência.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 2 //mesmo número de textos
#define NUM_LETRAS 100000

char nomes_arquivos[NUM_THREADS][501] = {{"texto1.txt"}, {"texto2.txt"}}; //vetor de palavras pra armazenar o nome dos arquivos de teste
int qtd_palavras = 0;
char *palavra = "piloto"; //palavra que quero achar

pthread_mutex_t mutex;

void *procuraTexto(void *arg){
    
    char tempTexto[NUM_LETRAS];

    strcpy(tempTexto, ((char *) arg)); //passando o texto da thread atual para a variavel tempTexto

    int acertos = 0; //quantidade de letras certas da palavra procurada
    
    for(int i = 0; i < strlen(tempTexto); i++){
        if(tempTexto[i] == palavra[acertos]){
            acertos++;
        }
        else{
            acertos = 0;
        }
        if(acertos == strlen(palavra)){
            pthread_mutex_lock(&mutex); //trava o mutex pra impedir a condição de corrida entre as threads
            qtd_palavras++; //aumenta contador de achar a palavra
            pthread_mutex_unlock(&mutex); //destrava o mutex
        }
    }
}



int main(void){

    pthread_mutex_init(&mutex, NULL); //inicializando o mutex

    pthread_t threads[NUM_THREADS];

    FILE *arq[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        arq[i] = fopen(nomes_arquivos[i], "r"); //abrindo cada arquivo
        if(arq[i] == NULL){
            printf("falha na abertura do arquivo %d\n", i + 1);
            exit(1);
        }
    }

    char texto[NUM_THREADS][NUM_LETRAS]; //vetor pra armazenar textos

    for(int i = 0; i < NUM_THREADS; i++){
        fscanf(arq[i], " %[^$] ", texto[i]); //lendo os arquivos até o caracter de parada escolhido '$'
    }

    for(int i = 0; i < NUM_THREADS; i++){
        int rc = pthread_create(&(threads[i]), NULL, procuraTexto, (void *) &(texto[i])); //criando threads
        if(rc){
            printf("falha na criacao das threads\n");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL); //aguarda a conclusão da thread
    }

    printf("A palavra %s foi encontrada %d vezes\n", palavra, qtd_palavras);

    for(int i = 0; i < NUM_THREADS; i++){
        fclose(arq[i]); //fechando os arquivos abertos
    }
    pthread_exit(NULL); //dando exit nas threads
    pthread_mutex_destroy(&mutex); //destruindo o mutex que já foi utilizado

    return 0;
}
