#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEITURAS 100
#define MAX_LINHA 256
#define RESOLUCAO_ADC 1023.0f

typedef struct {
    char id[20];
    float ganho;
    float desvio;
    float temp_min;
    float temp_max;
} Sensor;

static const Sensor SENSORES[] = {
    { "TEMP1", 260.0f, -20.0f, -10.0f, 190.0f },
    { "TEMP2", 250.0f, -15.0f,   0.0f, 150.0f },
    { "TEMP3", 240.0f, -10.0f, -10.0f, 100.0f }
};

#define NUM_SENSORES ((int)(sizeof(SENSORES) / sizeof(SENSORES[0])))

typedef struct {
    int indice_sensor;
    int instante;
    int valor_bruto;
    float temperatura;
} Leitura;

int procurar_sensor(const char *id, Sensor *sensor_encontrado);
int indice_do_sensor(const char *id);
float converter_valor_temperatura(const Sensor *sensor, int valor_lido);
int validar_temperatura_sensor(const Sensor *sensor, float temperatura);
int linha_em_branco(const char *linha);
int leitura_duplicada(const Leitura leituras[], int n, int indice_sensor, int instante);
void calcular_estatisticas_sensor(const Leitura leituras[], int n, int indice_sensor,
                                  int *count, float *min, int *inst_min,
                                  float *max, int *inst_max, float *media, float *desvio_padrao);

int ler_sequencia_valores(Leitura leituras[], int max_amostras);
void apresentar_resultados(const Leitura leituras[], int n);
void executar_testes(void);

int main(void) {
    executar_testes();

    printf("\n========================================\n");
    printf("       PROGRAMA PRINCIPAL - SPRINT 7    \n");
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

int procurar_sensor(const char *id, Sensor *sensor_encontrado) {
    if (id == NULL) {
        return 0;
    }

    for (int i = 0; i < NUM_SENSORES; i++) {
        if (strcmp(SENSORES[i].id, id) == 0) {
            if (sensor_encontrado != NULL) {
                *sensor_encontrado = SENSORES[i];
            }
            return 1;
        }
    }

    return 0;
}

int indice_do_sensor(const char *id) {
    if (id == NULL) {
        return -1;
    }

    for (int i = 0; i < NUM_SENSORES; i++) {
        if (strcmp(SENSORES[i].id, id) == 0) {
            return i;
        }
    }

    return -1;
}

float converter_valor_temperatura(const Sensor *sensor, int valor_lido) {
    if (sensor == NULL) {
        return 0.0f;
    }
    return ((sensor->ganho / RESOLUCAO_ADC) * (float)valor_lido) + sensor->desvio;
}

int validar_temperatura_sensor(const Sensor *sensor, float temperatura) {
    if (sensor == NULL) {
        return 0;
    }
    return (temperatura >= sensor->temp_min && temperatura <= sensor->temp_max) ? 1 : 0;
}

int linha_em_branco(const char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (!isspace((unsigned char)linha[i])) {
            return 0;
        }
    }
    return 1;
}

int leitura_duplicada(const Leitura leituras[], int n, int indice_sensor, int instante) {
    for (int i = 0; i < n; i++) {
        if (leituras[i].indice_sensor == indice_sensor && leituras[i].instante == instante) {
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
    for (int i = 0; i < NUM_SENSORES; i++) {
        printf("%s ", SENSORES[i].id);
    }
    printf("\nEscreva FIM para terminar (maximo %d leituras validas):\n\n", max_amostras);

    while (contador < max_amostras) {
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;
        }

        if (linha_em_branco(linha)) {
            continue;
        }

        if (sscanf(linha, " %49s", sensor_str) != 1) {
            continue;
        }

        if (strcmp(sensor_str, "FIM") == 0) {
            if (sscanf(linha, " %49s %49s", sensor_str, extra) == 1) {
                break;
            }
        }

        Sensor sensor_atual;
        if (!procurar_sensor(sensor_str, &sensor_atual)) {
            printf("Sensor desconhecido\n");
            continue;
        }

        int idx_sensor = indice_do_sensor(sensor_str);

        int campos_lidos = sscanf(linha, " %49s %d %d %49s", sensor_str, &instante, &valor, extra);
        if (campos_lidos != 3) {
            printf("Linha invalida\n");
            continue;
        }

        if (leitura_duplicada(leituras, contador, idx_sensor, instante)) {
            printf("Linha duplicada (ignorada)\n");
            continue;
        }

        float temp = converter_valor_temperatura(&sensor_atual, valor);
        if (!validar_temperatura_sensor(&sensor_atual, temp)) {
            printf("Valor fora da gama\n");
            continue;
        }

        leituras[contador].indice_sensor = idx_sensor;
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

void calcular_estatisticas_sensor(const Leitura leituras[], int n, int indice_sensor,
                                  int *count, float *min, int *inst_min,
                                  float *max, int *inst_max, float *media, float *desvio_padrao) {
    *count = 0;
    float soma = 0.0f;

    for (int i = 0; i < n; i++) {
        if (leituras[i].indice_sensor == indice_sensor) {
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

    if (*count == 0) {
        return;
    }

    *media = soma / (float)(*count);

    float soma_sq_diff = 0.0f;
    for (int i = 0; i < n; i++) {
        if (leituras[i].indice_sensor == indice_sensor) {
            float diff = leituras[i].temperatura - *media;
            soma_sq_diff += diff * diff;
        }
    }
    *desvio_padrao = sqrtf(soma_sq_diff / (float)(*count));
}

void apresentar_resultados(const Leitura leituras[], int n) {
    printf("\n========================================================\n");
    printf("              LISTA DE LEITURAS REGISTADAS              \n");
    printf("========================================================\n");
    printf("%-12s | %-10s | %-12s | %-12s\n", "Sensor", "Instante", "Valor Lido", "Temperatura");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-12s | %-10d | %-12d | %8.2f °C\n",
               SENSORES[leituras[i].indice_sensor].id,
               leituras[i].instante,
               leituras[i].valor_bruto,
               leituras[i].temperatura);
    }

    printf("\n========================================================\n");
    printf("                ESTATISTICAS POR SENSOR                 \n");
    printf("========================================================\n");

    for (int s = 0; s < NUM_SENSORES; s++) {
        int count, inst_min, inst_max;
        float min, max, media, dp;

        calcular_estatisticas_sensor(leituras, n, s, &count, &min, &inst_min, &max, &inst_max, &media, &dp);

        if (count > 0) {
            printf("\nSensor: %s (Gama: %.1f °C a %.1f °C)\n",
                   SENSORES[s].id, SENSORES[s].temp_min, SENSORES[s].temp_max);
            printf("  - Numero de valores lidos : %d\n", count);
            printf("  - Temperatura minima      : %.2f °C (no instante %d)\n", min, inst_min);
            printf("  - Temperatura maxima      : %.2f °C (no instante %d)\n", max, inst_max);
            printf("  - Media das temperaturas  : %.2f °C\n", media);
            printf("  - Desvio-padrao           : %.2f °C\n", dp);
        }
    }
}

void executar_testes(void) {
    printf("--- Execucao de Testes Unitarios (Sprint 7) ---\n");

    Sensor s1;
    int achou1 = procurar_sensor("TEMP1", &s1);
    int achou_inv = procurar_sensor("TEMP9", NULL);
    printf("Teste Procura Sensor: TEMP1=%d (id=%s), TEMP9=%d -> [%s]\n",
           achou1, achou1 ? s1.id : "N/A", achou_inv,
           (achou1 == 1 && strcmp(s1.id, "TEMP1") == 0 && achou_inv == 0) ? "SUCESSO" : "FALHA");

    float temp_conv = converter_valor_temperatura(&s1, 523);
    printf("Teste Conversao (TEMP1, 523): Obtido = %.2f °C | Esperado ≈ 112.96 °C -> [%s]\n",
           temp_conv, (fabsf(temp_conv - 112.96f) < 0.05f) ? "SUCESSO" : "FALHA");

    Sensor s2;
    procurar_sensor("TEMP2", &s2);
    /* TEMP2 tem gama [0 °C, 150 °C]: -5 °C é inválido, 25 °C é válido */
    int val_ok = validar_temperatura_sensor(&s2, 25.0f);
    int val_fora = validar_temperatura_sensor(&s2, -5.0f);
    printf("Teste Validacao Gama TEMP2 [0..150]: 25.0=%d (Esp:1), -5.0=%d (Esp:0) -> [%s]\n",
           val_ok, val_fora, (val_ok == 1 && val_fora == 0) ? "SUCESSO" : "FALHA");

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