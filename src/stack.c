#include "stack.h"
#include <stdlib.h>

void stack_char_push(stack_char **restrict st, char val) {
  stack_char *tmp = (stack_char *)malloc(sizeof(stack_char));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

char stack_char_pop(stack_char **restrict st) {
  stack_char *tmp = *st;
  char ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void stack_char_clear(stack_char **restrict st) {
  while (*st)
    stack_char_pop(st);
}

void stack_double_push(stack_double **restrict st, double val) {
  stack_double *tmp = (stack_double *)malloc(sizeof(stack_double));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

double stack_double_pop(stack_double **restrict st) {
  stack_double *tmp = *st;
  double ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void stack_double_clear(stack_double **restrict st) {
  while (*st)
    stack_double_pop(st);
}
