#pragma once

typedef struct stack_char {
  char top;
  struct stack_char *restrict next;
} stack_char;
void stack_char_push(stack_char **restrict st, char val);
char stack_char_pop(stack_char **restrict st);
void stack_char_clear(stack_char **restrict st);

typedef struct stack_double {
  double top;
  struct stack_double *restrict next;
} stack_double;
void stack_double_push(stack_double **restrict st, double val);
double stack_double_pop(stack_double **restrict st);
void stack_double_clear(stack_double **restrict st);
