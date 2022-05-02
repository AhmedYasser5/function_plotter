#ifndef STACK_HEADER
#define STACK_HEADER

#include <stdlib.h>

typedef struct stack_char {
  char top;
  struct stack_char *next;
} stack_char;
void stack_char_push(stack_char **st, char val);
char stack_char_pop(stack_char **st);
void stack_char_clear(stack_char **st);

typedef struct stack_double {
  double top;
  struct stack_double *next;
} stack_double;
void stack_double_push(stack_double **st, double val);
double stack_double_pop(stack_double **st);
void stack_double_clear(stack_double **st);

#endif
