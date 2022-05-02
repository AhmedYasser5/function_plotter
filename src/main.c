#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char arr[MAX];
  scanf("%[^\n]", arr);
  while (1) {
    double x;
    scanf("%lf", &x);
    char *message = eval(arr, x);
    fflush(stdout);
    fprintf(stderr, "\n-------------- %s --------------\n", message);
    free(message);
  }
  return EXIT_SUCCESS;
}
