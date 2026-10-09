#include <stdio.h>

int main() {
int x;
float temp;

printf("Insira um valor do sensor: \n");

while (scanf("%d", &x) != 1) {
while (getchar() != '\n');
printf("Insira um valor do sensor: ");
}

temp = (260.0f * x / 1023.0f) - 20.0f;

if (temp >= -10.0f && temp <= 190.0f) {
printf("A temperatura e igual a: %.2f\n", temp);
} else {
printf("Valor fora da gama\n");
}

return 0;
}