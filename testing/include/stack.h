#pragma once

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

typedef struct stack_point {
  double top_x, top_y;
  struct stack_point *next;
} stack_point;
void stack_point_push(stack_point **st, double x, double y);
void stack_point_pop(stack_point **st);
void stack_point_clear(stack_point **st);
