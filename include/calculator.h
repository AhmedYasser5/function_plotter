#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

#include "stack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPS 5
#define THRESHF 5
#define THRESHB 5

char calc(stack_char **op, stack_double **num);
void calc_error(char **message, char err);
double parse_number(const char *arr, int *i);
double eval(const char *arr, double x, char *message);

#endif
