#include <stdio.h>

int main() 
{
int x;
float temp;

printf("Introduza o valor do sensor: ");
scanf("%d", &x);

if (x >= 0 && x <= 1023) {
    temp = 260.0 * x / 1023 - 20;
    printf("A Temperatura é: %.2f\n", temp);
} else {
    printf("Valor invalido\n");
}

return 0;

}