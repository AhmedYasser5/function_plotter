#ifndef STACK_HEADER
#define STACK_HEADER

#include <stdlib.h>

typedef struct stack_char {
  char top;
  struct stack_char *next;
} stack_char;
void push_char(stack_char **st, char val);
char pop_char(stack_char **st);
void clear_char(stack_char **st);

typedef struct stack_double {
  double top;
  struct stack_double *next;
} stack_double;
void push_double(stack_double **st, double val);
double pop_double(stack_double **st);
void clear_double(stack_double **st);

#endif
