#include <stdio.h>
#include <math.h>

#define MAX_LEITURAS 20
#define TEMP_MIN -10.0f
#define TEMP_MAX 190.0f

float converter_sensor_temperatura(int valor_sensor);
int validar_temperatura(float temperatura);
int ler_sequencia_valores(float temperaturas[], int max_amostras);
void calcular_estatisticas(const float temperaturas[], int n, float *min, float *max, float *media, float *desvio_padrao);
void apresentar_resultados(const float temperaturas[], int n, float min, float max, float media, float desvio_padrao);
void executar_testes(void);

int main() {

    executar_testes();
    
    printf("\n========================================\n");
    printf("       PROGRAMA PRINCIPAL - SENSOR      \n");
    printf("========================================\n");

    float temperaturas[MAX_LEITURAS];
    int contador = 0;
    float min, max, media, desvio_padrao;

    contador = ler_sequencia_valores(temperaturas, MAX_LEITURAS);

    if (contador == 0) {
        printf("\nNenhuma temperatura valida foi registada.\n");
        return 0;
    }

    calcular_estatisticas(temperaturas, contador, &min, &max, &media, &desvio_padrao);

    apresentar_resultados(temperaturas, contador, min, max, media, desvio_padrao);

    return 0;
}

float converter_sensor_temperatura(int valor_sensor) {
    return (260.0f * valor_sensor / 1023.0f) - 20.0f;
}

int validar_temperatura(float temperatura) {
    return (temperatura >= TEMP_MIN && temperatura <= TEMP_MAX);
}

int ler_sequencia_valores(float temperaturas[], int max_amostras) {
    int n_amostras = max_amostras;
    int x;
    int contador = 0;

    printf("Digite o numero de amostras a ler (maximo %d): ", max_amostras);
    while (scanf("%d", &n_amostras) != 1 || n_amostras <= 0 || n_amostras > max_amostras) {
        while (getchar() != '\n');
        printf("Valor invalido. Insira um numero entre 1 e %d: ", max_amostras);
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

        float temp = converter_sensor_temperatura(x);

        if (validar_temperatura(temp)) {
            temperaturas[contador] = temp;
            printf("A temperatura e igual a: %.2f °C\n", temp);
            contador++;
        } else {
            printf("Valor fora da gama\n");
        }
    }

    return contador;
}


void calcular_estatisticas(const float temperaturas[], int n, float *min, float *max, float *media, float *desvio_padrao) {
    if (n <= 0) return;

    *min = temperaturas[0];
    *max = temperaturas[0];
    float soma = 0.0f;

    for (int i = 0; i < n; i++) {
        soma += temperaturas[i];
        if (temperaturas[i] > *max) {
            *max = temperaturas[i];
        }
        if (temperaturas[i] < *min) {
            *min = temperaturas[i];
        }
    }

    *media = soma / n;

    float soma_sq_diff = 0.0f;
    for (int i = 0; i < n; i++) {
        soma_sq_diff += (temperaturas[i] - *media) * (temperaturas[i] - *media);
    }
    *desvio_padrao = sqrt(soma_sq_diff / n);
}

void apresentar_resultados(const float temperaturas[], int n, float min, float max, float media, float desvio_padrao) {
    printf("\n--- Lista de Temperaturas Registadas ---\n");
    for (int i = 0; i < n; i++) {
        printf("Leitura %d: %.2f °C\n", i + 1, temperaturas[i]);
    }

    printf("\n--- Estatisticas das Temperaturas ---\n");
    printf("a. Numero de valores lidos: %d\n", n);
    printf("b. Temperatura maxima: %.2f °C\n", max);
    printf("   Temperatura minima: %.2f °C\n", min);
    printf("c. Media das temperaturas: %.2f °C\n", media);
    printf("   Desvio-padrao: %.2f\n", desvio_padrao);
}

void executar_testes(void) {
    printf("--- Execucao de Testes Unitarios ---\n");

    int sensor_teste = 523;
    float temp_convertida = converter_sensor_temperatura(sensor_teste);
    printf("Teste Conversao (Sensor %d): Obtido = %.2f °C | Esperado ≈ 112.96 °C -> [%s]\n", 
           sensor_teste, temp_convertida, (fabs(temp_convertida - 112.96f) < 0.05f) ? "SUCESSO" : "FALHA");

    float t_valida = 25.0f;
    float t_invalida = 200.0f;
    printf("Teste Validacao (%.1f °C): Obtido = %d | Esperado = 1 -> [%s]\n", 
           t_valida, validar_temperatura(t_valida), validar_temperatura(t_valida) == 1 ? "SUCESSO" : "FALHA");
    printf("Teste Validacao (%.1f °C): Obtido = %d | Esperado = 0 -> [%s]\n", 
           t_invalida, validar_temperatura(t_invalida), validar_temperatura(t_invalida) == 0 ? "SUCESSO" : "FALHA");

    float amostras_teste[] = {10.0f, 20.0f, 30.0f};
    float min_t, max_t, med_t, dp_t;
    calcular_estatisticas(amostras_teste, 3, &min_t, &max_t, &med_t, &dp_t);
    printf("Teste Estatisticas (Min/Max/Media): Min=%.1f (Esp: 10.0), Max=%.1f (Esp: 30.0), Media=%.1f (Esp: 20.0) -> [%s]\n",
           min_t, max_t, med_t, (min_t == 10.0f && max_t == 30.0f && med_t == 20.0f) ? "SUCESSO" : "FALHA");
    
    printf("----------------------------------------\n");
}