#include "stack.h"
#include <stdlib.h>

void stack_char_push(stack_char **st, char val) {
  stack_char *tmp = (stack_char *)malloc(sizeof(stack_char));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

char stack_char_pop(stack_char **st) {
  stack_char *tmp = *st;
  char ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void stack_char_clear(stack_char **st) {
  while (*st)
    stack_char_pop(st);
}

void stack_double_push(stack_double **st, double val) {
  stack_double *tmp = (stack_double *)malloc(sizeof(stack_double));
  tmp->top = val;
  tmp->next = *st;
  *st = tmp;
}

double stack_double_pop(stack_double **st) {
  stack_double *tmp = *st;
  double ret = tmp->top;
  *st = (*st)->next;
  free(tmp);
  return ret;
}

void stack_double_clear(stack_double **st) {
  while (*st)
    stack_double_pop(st);
}

void stack_point_push(stack_point **st, double x, double y) {
  stack_point *tmp = (stack_point *)malloc(sizeof(stack_point));
  tmp->top_x = x;
  tmp->top_y = y;
  tmp->next = *st;
  *st = tmp;
}

void stack_point_pop(stack_point **st) {
  stack_point *tmp = *st;
  *st = (*st)->next;
  free(tmp);
}

void stack_point_clear(stack_point **st) {
  while (*st)
    stack_point_pop(st);
}
