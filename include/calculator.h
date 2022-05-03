#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

#include "stack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EPSILON
#define EPSILON 0.05
#endif

#define OPS 5
#define THRESHF 5
#define THRESHB 5

double calculator_parse_number(const char *arr, int *i);
double calculator_eval(const char *arr, double x, char *message);

#endif
