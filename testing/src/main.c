#include "calculator.h"
#include "stack.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int rand_int(int start, int end);
static char rand_char(char start, char end);
static double rand_double();
static char isequal(double x, double y);

static void test_stack_char_push();
static void test_stack_char_pop();
static void test_stack_char_clear();

static void test_stack_double_push();
static void test_stack_double_pop();
static void test_stack_double_clear();

static void test_stack_point_push();
static void test_stack_point_pop();
static void test_stack_point_clear();

char calc(stack_char **op, stack_double **num);
char calc_error(char **message, char err);
double parse_number(const char *arr, int *i);

static void test_calc();
static void test_calc_error();
static void test_parse_number();
static void test_calculator_atolf();
static void test_calculator_eval();

int main(int argc, char *argv[]) {
  srand(time(0));

  test_stack_char_push();
  test_stack_char_pop();
  test_stack_char_clear();

  test_stack_double_push();
  test_stack_double_pop();
  test_stack_double_clear();

  test_stack_point_push();
  test_stack_point_pop();
  test_stack_point_clear();

  test_calc();
  test_calc_error();
  test_parse_number();
  test_calculator_atolf();
  test_calculator_eval();
  return EXIT_SUCCESS;
}

static int rand_int(int start, int end) {
  return rand() % (end - start + 1) + start;
}

static char rand_char(char start, char end) { return rand_int(start, end); }

static double rand_double() {
  uint64_t m = 1LL * rand() * rand();
  double n;
  char *cn = (char *)(&n), *cm = (char *)(&m);
  for (int i = 0; i < 8; i++, cn++, cm++)
    *cn = *cm;
  return n;
}

static char isequal(double x, double y) { return isless(fabs(x - y), EPSILON); }

static void test_stack_char_push() {
  stack_char *st = NULL;
  char c = rand_char(INT8_MIN, INT8_MAX);
  stack_char_push(&st, c);
  assert(st != NULL && st->top == c);
  free(st);
}

static void test_stack_char_pop() {
  stack_char *st = NULL;
  stack_char_push(&st, rand_char(INT8_MIN, INT8_MAX));
  stack_char_pop(&st);
  assert(st == NULL);
}

static void test_stack_char_clear() {
  stack_char *st = NULL;
  int n = rand_int(2, 10);
  for (int i = 0; i < n; i++)
    stack_char_push(&st, rand_char(INT8_MIN, INT8_MAX));
  stack_char_clear(&st);
  assert(st == NULL);
}

static void test_stack_double_push() {
  stack_double *st = NULL;
  double n = rand_double();
  stack_double_push(&st, n);
  assert(st != NULL && isequal(st->top, n));
  free(st);
}

static void test_stack_double_pop() {
  stack_double *st = NULL;
  stack_double_push(&st, rand_double());
  stack_double_pop(&st);
  assert(st == NULL);
}

static void test_stack_double_clear() {
  stack_double *st = NULL;
  int n = rand_int(2, 10);
  for (int i = 0; i < n; i++)
    stack_double_push(&st, rand_double());
  stack_double_clear(&st);
  assert(st == NULL);
}

static void test_stack_point_push() {
  stack_point *st = NULL;
  double x = rand_double(), y = rand_double();
  stack_point_push(&st, x, y);
  assert(st != NULL && isequal(st->top_x, x) && isequal(st->top_y, y));
  free(st);
}

static void test_stack_point_pop() {
  stack_point *st = NULL;
  stack_point_push(&st, rand_double(), rand_double());
  stack_point_pop(&st);
  assert(st == NULL);
}

static void test_stack_point_clear() {
  stack_point *st = NULL;
  int n = rand_int(2, 10);
  for (int i = 0; i < n; i++)
    stack_point_push(&st, rand_double(), rand_double());
  stack_point_clear(&st);
  assert(st == NULL);
}

static void test_calc() {}

static void test_calc_error() {}

static void test_parse_number() {}

static void test_calculator_atolf() {}

static void test_calculator_eval() {}
