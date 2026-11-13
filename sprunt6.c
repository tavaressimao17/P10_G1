#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEITURAS 100
#define MAX_LINHA 256
#define TEMP_MIN -10.0f
#define TEMP_MAX 190.0f

typedef struct {
    char nome[20];
    float declive;   // m
    float ordenada;  // b
} SensorConfig;

const SensorConfig SENSORES[] = {
    {"TEMP1", 260.0f / 1023.0f, -20.0f}, // Calibração original do Sprint 5
    {"TEMP2", 200.0f / 1023.0f, -10.0f}, // Exemplo de calibração para TEMP2
    {"TEMP3", 0.25f,            -15.0f}  // Exemplo de um terceiro sensor
};

#define NUM_SENSORES (sizeof(SENSORES) / sizeof(SENSORES[0]))

typedef struct {
    int id_sensor;
    int instante;
    int valor_bruto;
    float temperatura;
} Leitura;

int procurar_sensor(const char *nome);
float converter_sensor_temperatura(int id_sensor, int valor_sensor);
int validar_temperatura(float temperatura);
int linha_em_branco(const char *linha);
int leitura_duplicada(const Leitura leituras[], int n, int id_sensor, int instante);
int ler_sequencia_valores(Leitura leituras[], int max_amostras);
void calcular_estatisticas_sensor(const Leitura leituras[], int n, int id_sensor, 
                                  int *count, float *min, int *inst_min, 
                                  float *max, int *inst_max, float *media, float *desvio_padrao);
void apresentar_resultados(const Leitura leituras[], int n);
void executar_testes(void);

int main(void) {
    executar_testes();

    printf("\n========================================\n");
    printf("       PROGRAMA PRINCIPAL - SPRINT 6    \n");
    printf("========================================\n");

    Leitura leituras[MAX_LEITURAS];
    int total_leituras = ler_sequencia_valores(leituras, MAX_LEITURAS);

    if (total_leituras == 0) {
        printf("\nNenhuma temperatura valida foi registada.\n");
        return 0;
    }

    apresentar_resultados(leituras, total_leituras);

    return 0;
}

int procurar_sensor(const char *nome) {
    for (int i = 0; i < (int)NUM_SENSORES; i++) {
        if (strcmp(SENSORES[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

float converter_sensor_temperatura(int id_sensor, int valor_sensor) {
    return (SENSORES[id_sensor].declive * valor_sensor) + SENSORES[id_sensor].ordenada;
}

int validar_temperatura(float temperatura) {
    return (temperatura >= TEMP_MIN && temperatura <= TEMP_MAX);
}

int linha_em_branco(const char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (!isspace((unsigned char)linha[i])) {
            return 0;
        }
    }
    return 1;
}

int leitura_duplicada(const Leitura leituras[], int n, int id_sensor, int instante) {
    for (int i = 0; i < n; i++) {
        if (leituras[i].id_sensor == id_sensor && leituras[i].instante == instante) {
            return 1;
        }
    }
    return 0;
}

int ler_sequencia_valores(Leitura leituras[], int max_amostras) {
    char linha[MAX_LINHA];
    char sensor_str[50];
    int instante, valor;
    char extra[50];
    int contador = 0;

    printf("Introduza as leituras no formato: SENSOR INSTANTE VALOR\n");
    printf("Sensores conhecidos: ");
    for (int i = 0; i < (int)NUM_SENSORES; i++) {
        printf("%s ", SENSORES[i].nome);
    }
    printf("\nEscreva FIM para terminar (maximo %d leituras validas):\n\n", max_amostras);

    while (contador < max_amostras) {
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break; // Fim de ficheiro (EOF)
        }

        if (linha_em_branco(linha)) {
            continue;
        }

        if (sscanf(linha, " %49s", sensor_str) != 1) {
            continue;
        }

        if (strcmp(sensor_str, "FIM") == 0) {
            // Verifica se não há lixo depois de FIM
            if (sscanf(linha, " %49s %49s", sensor_str, extra) == 1) {
                break;
            }
        }

        int id_sensor = procurar_sensor(sensor_str);
        if (id_sensor == -1) {
            printf("Sensor desconhecido\n");
            continue;
        }

        int campos_lidos = sscanf(linha, " %49s %d %d %49s", sensor_str, &instante, &valor, extra);
        if (campos_lidos != 3) {
            printf("Linha invalida\n");
            continue;
        }

        if (leitura_duplicada(leituras, contador, id_sensor, instante)) {
            printf("Linha duplicada (ignorada)\n");
            continue;
        }

        float temp = converter_sensor_temperatura(id_sensor, valor);
        if (!validar_temperatura(temp)) {
            printf("Valor fora da gama\n");
            continue;
        }

        // Regista a leitura válida
        leituras[contador].id_sensor = id_sensor;
        leituras[contador].instante = instante;
        leituras[contador].valor_bruto = valor;
        leituras[contador].temperatura = temp;
        contador++;

        printf("%.2f °C\n", temp);
    }

    if (contador == max_amostras) {
        printf("\nLimite maximo de %d leituras atingido.\n", max_amostras);
    }

    return contador;
}

void calcular_estatisticas_sensor(const Leitura leituras[], int n, int id_sensor, 
                                  int *count, float *min, int *inst_min, 
                                  float *max, int *inst_max, float *media, float *desvio_padrao) {
    *count = 0;
    float soma = 0.0f;

    for (int i = 0; i < n; i++) {
        if (leituras[i].id_sensor == id_sensor) {
            float t = leituras[i].temperatura;
            int inst = leituras[i].instante;

            if (*count == 0) {
                *min = t;
                *inst_min = inst;
                *max = t;
                *inst_max = inst;
            } else {
                if (t < *min) {
                    *min = t;
                    *inst_min = inst;
                }
                if (t > *max) {
                    *max = t;
                    *inst_max = inst;
                }
            }
            soma += t;
            (*count)++;
        }
    }

    if (*count == 0) return;

    *media = soma / (*count);

    float soma_sq_diff = 0.0f;
    for (int i = 0; i < n; i++) {
        if (leituras[i].id_sensor == id_sensor) {
            float diff = leituras[i].temperatura - *media;
            soma_sq_diff += diff * diff;
        }
    }
    *desvio_padrao = sqrtf(soma_sq_diff / (*count));
}

void apresentar_resultados(const Leitura leituras[], int n) {
    printf("\n========================================================\n");
    printf("              LISTA DE LEITURAS REGISTADAS              \n");
    printf("========================================================\n");
    printf("%-12s | %-10s | %-12s | %-12s\n", "Sensor", "Instante", "Valor Lido", "Temperatura");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-12s | %-10d | %-12d | %8.2f °C\n",
               SENSORES[leituras[i].id_sensor].nome,
               leituras[i].instante,
               leituras[i].valor_bruto,
               leituras[i].temperatura);
    }

    printf("\n========================================================\n");
    printf("                ESTATISTICAS POR SENSOR                 \n");
    printf("========================================================\n");
    
    for (int s = 0; s < (int)NUM_SENSORES; s++) {
        int count, inst_min, inst_max;
        float min, max, media, dp;

        calcular_estatisticas_sensor(leituras, n, s, &count, &min, &inst_min, &max, &inst_max, &media, &dp);

        if (count > 0) {
            printf("\nSensor: %s\n", SENSORES[s].nome);
            printf("  - Numero de valores lidos : %d\n", count);
            printf("  - Temperatura minima      : %.2f °C (no instante %d)\n", min, inst_min);
            printf("  - Temperatura maxima      : %.2f °C (no instante %d)\n", max, inst_max);
            printf("  - Media das temperaturas  : %.2f °C\n", media);
            printf("  - Desvio-padrao           : %.2f °C\n", dp);
        }
    }
}

void executar_testes(void) {
    printf("--- Execucao de Testes Unitarios (Sprint 6) ---\n");

    int id1 = procurar_sensor("TEMP1");
    int id_inv = procurar_sensor("TEMP9");
    printf("Teste Procura Sensor: TEMP1=%d (Esp: 0), TEMP9=%d (Esp: -1) -> [%s]\n",
           id1, id_inv, (id1 == 0 && id_inv == -1) ? "SUCESSO" : "FALHA");

    float temp_conv = converter_sensor_temperatura(id1, 523);
    printf("Teste Conversao (TEMP1, 523): Obtido = %.2f °C | Esperado ≈ 112.96 °C -> [%s]\n",
           temp_conv, (fabsf(temp_conv - 112.96f) < 0.05f) ? "SUCESSO" : "FALHA");

    printf("Teste Validacao (25.0 °C / 200.0 °C): [%s]\n",
           (validar_temperatura(25.0f) == 1 && validar_temperatura(200.0f) == 0) ? "SUCESSO" : "FALHA");

    Leitura mock[3] = {
        {0, 10, 0, 10.0f}, 
        {1, 15, 0, 99.0f}, 
        {0, 20, 0, 30.0f}  
    };
    int cnt, imin, imax;
    float min, max, med, dp;
    calcular_estatisticas_sensor(mock, 3, 0, &cnt, &min, &imin, &max, &imax, &med, &dp);
    printf("Teste Estatisticas TEMP1: N=%d, Min=%.1f (inst %d), Max=%.1f (inst %d), Med=%.1f -> [%s]\n",
           cnt, min, imin, max, imax, med,
           (cnt == 2 && min == 10.0f && imin == 10 && max == 30.0f && imax == 20 && med == 20.0f) ? "SUCESSO" : "FALHA");

    printf("-----------------------------------------------\n");
}