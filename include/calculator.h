#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

#include "stack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define OPS 5
#define MAX 128
#define THRESHF 5
#define THRESHB 5

char calc(stack_char **op, stack_double **num);
void calc_error(char **message, char err);
double parse_number(const char *arr, int *i);
char *eval(char arr[MAX], double x);

#endif
