#pragma once

#ifndef EPSILON
#define EPSILON 1e-8
#endif
#define THRESHF 5
#define THRESHB 5

double calculator_atolf(const char *snum);
char calculator_eval(const char *arr, double x, double *ans, char *message);
