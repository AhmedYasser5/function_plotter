#include "stack.h"

void push_char(stack_char **st, char val) {
  stack_char *tmp = (stack_char *)malloc(sizeof(stack_char));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

char pop_char(stack_char **st) {
  stack_char *tmp = *st;
  char ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void clear_char(stack_char **st) {
  while (*st)
    pop_char(st);
}

void push_double(stack_double **st, double val) {
  stack_double *tmp = (stack_double *)malloc(sizeof(stack_double));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

double pop_double(stack_double **st) {
  stack_double *tmp = *st;
  double ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void clear_double(stack_double **st) {
  while (*st)
    pop_double(st);
}
