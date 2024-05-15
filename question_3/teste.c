#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    FILE* arq = NULL;

    char texto[50] = {};
    char* comando = NULL;
    char* qtd = NULL;

    arq = fopen("cliente1.txt", "r");

    if (arq == NULL) {
        printf("Falha ao abrir o arquivo\n");
        exit(1);
    }

    while (fscanf(arq, "%49[^,],\n", texto) == 1) {
        comando = strtok(texto, " ");
        switch (comando[0]) {
        case 'C':
            printf("Pediu pra consultar\n");
            break;
        case 'D':
            qtd = strtok(NULL, "\n");
            printf("Pediu pra depositar %s\n", qtd);
            break;
        case 'S':
            qtd = strtok(NULL, "\n");
            printf("Pediu pra sacar %s\n", qtd);
            break;
        default:
            break;
        }
    }

    fclose(arq);

    return 0;
}