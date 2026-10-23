#include <stdio.h>
#include <math.h>

#define MAX_LEITURAS 20

int main() {
    int x;
    float temp;
    float temperaturas[MAX_LEITURAS];
    int contador = 0;
    int n_amostras = MAX_LEITURAS;

    printf("Digite o numero de amostras a ler (maximo %d): ", MAX_LEITURAS);
    while (scanf("%d", &n_amostras) != 1 || n_amostras <= 0 || n_amostras > MAX_LEITURAS) {
        while (getchar() != '\n'); // 
        printf("Valor invalido. Insira um numero entre 1 e %d: ", MAX_LEITURAS);
    }

    printf("\nInsira os valores do sensor (ou -1 para terminar):\n");

    while (contador < n_amostras) {
        printf("Leitura %d: ", contador + 1);
        
        while (scanf("%d", &x) != 1) {
            while (getchar() != '\n'); 
            printf("Entrada invalida. Insira um valor do sensor (ou -1 para terminar): ");
        }

        if (x == -1) {
            break;
        }

        temp = (260.0f * x / 1023.0f) - 20.0f;

        if (temp >= -10.0f && temp <= 190.0f) {
            temperaturas[contador] = temp;
            printf("A temperatura e igual a: %.2f °C\n", temp);
            contador++;
        } else {
            printf("Valor fora da gama\n");
        }
    }

    if (contador == 0) {
        printf("\nNenhuma temperatura valida foi registada.\n");
        return 0;
    }

    printf("\n--- Lista de Temperaturas Registadas ---\n");
    float soma = 0.0f;
    float max = temperaturas[0];
    float min = temperaturas[0];

    for (int i = 0; i < contador; i++) {
        printf("Leitura %d: %.2f °C\n", i + 1, temperaturas[i]);
        soma += temperaturas[i];
        if (temperaturas[i] > max) {
            max = temperaturas[i];
        }
        if (temperaturas[i] < min) {
            min = temperaturas[i];
        }
    }

    float media = soma / contador;

    float soma_sq_diff = 0.0f;
    for (int i = 0; i < contador; i++) {
        soma_sq_diff += (temperaturas[i] - media) * (temperaturas[i] - media);
    }
    float desvio_padrao = sqrt(soma_sq_diff / contador);

    printf("\n--- Estatisticas das Temperaturas ---\n");
    printf("a. Numero de valores lidos: %d\n", contador);
    printf("b. Temperatura maxima: %.2f °C\n", max);
    printf("   Temperatura minima: %.2f °C\n", min);
    printf("c. Media das temperaturas: %.2f °C\n", media);
    printf("   Desvio-padrao: %.2f\n", desvio_padrao);

    return 0;
}