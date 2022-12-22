#define EPSILON 1e-8

#include "calculator.h"
#include "stack.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 128

static void test_stack_char_push();
static void test_stack_char_pop();
static void test_stack_char_clear();

static void test_stack_double_push();
static void test_stack_double_pop();
static void test_stack_double_clear();

static void test_calc();
static void test_calculator_atolf();
static void test_calculator_eval();

int main(int argc, char *argv[]) {
  printf("Testing has started...\n");
  srand(time(0));

  test_stack_char_push();
  test_stack_char_pop();
  test_stack_char_clear();

  test_stack_double_push();
  test_stack_double_pop();
  test_stack_double_clear();

  test_calc();
  test_calculator_atolf();
  test_calculator_eval();

  printf("\nAll tests succeeded");
  return EXIT_SUCCESS;
}

static int rand_int(int start, int end) {
  return rand() % (end - start + 1) + start;
}

static char rand_char(char start, char end) { return rand_int(start, end); }

/* random double that might be NAN, -NAN, INF, -INF, ... */
static double rand_double() {
  uint64_t m = 1LL * rand() * rand();
  double n;
  char *cn = (char *)(&n), *cm = (char *)(&m);
  for (int i = 0; i < 8; i++, cn++, cm++)
    *cn = *cm;
  return n;
}

static double rand_proper_double() {
  char *num = (char *)malloc(sizeof(char) * MAX_SIZE);
  sprintf(num, "%d.%d", rand(), rand());
  double x;
  sscanf(num, "%lf", &x);
  free(num);
  return x;
}

static char isequal(double x, double y) { return isless(fabs(x - y), EPSILON); }

static void test_stack_char_push() {
  printf("\ntest_stack_char_push() started\n");
  fflush(stdout);

  stack_char *st = NULL;
  char c = rand_char(INT8_MIN, INT8_MAX);
  stack_char_push(&st, c);
  assert(st != NULL && st->top == c);

  free(st);
  printf("test_stack_char_push() succeeded\n");
  fflush(stdout);
}

static void test_stack_char_pop() {
  printf("\ntest_stack_char_pop() started\n");
  fflush(stdout);

  stack_char *st = NULL;
  stack_char_push(&st, rand_char(INT8_MIN, INT8_MAX));
  stack_char_pop(&st);
  assert(st == NULL);

  printf("test_stack_char_pop() succeeded\n");
  fflush(stdout);
}

static void test_stack_char_clear() {
  printf("\ntest_stack_char_clear() started\n");
  fflush(stdout);

  stack_char *st = NULL;
  int n = rand_int(2, 10);
  for (int i = 0; i < n; i++)
    stack_char_push(&st, rand_char(INT8_MIN, INT8_MAX));

  stack_char_clear(&st);
  assert(st == NULL);
  stack_char_clear(&st);
  assert(st == NULL);

  printf("test_stack_char_clear() succeeded\n");
  fflush(stdout);
}

static void test_stack_double_push() {
  printf("\ntest_stack_double_push() started\n");
  fflush(stdout);

  stack_double *st = NULL;
  double n = rand_double();
  stack_double_push(&st, n);
  assert(st != NULL && isequal(st->top, n));

  free(st);
  printf("test_stack_double_push() succeeded\n");
  fflush(stdout);
}

static void test_stack_double_pop() {
  printf("\ntest_stack_double_pop() started\n");
  fflush(stdout);

  stack_double *st = NULL;
  stack_double_push(&st, rand_double());
  stack_double_pop(&st);
  assert(st == NULL);

  printf("test_stack_double_pop() succeeded\n");
  fflush(stdout);
}

static void test_stack_double_clear() {
  printf("\ntest_stack_double_clear() started\n");
  fflush(stdout);

  stack_double *st = NULL;
  int n = rand_int(2, 10);
  for (int i = 0; i < n; i++)
    stack_double_push(&st, rand_double());

  stack_double_clear(&st);
  assert(st == NULL);
  stack_double_clear(&st);
  assert(st == NULL);

  printf("test_stack_double_clear() succeeded\n");
  fflush(stdout);
}

char calc(stack_char **op, stack_double **num);
#define OPS 5
static const char ops[OPS] = {'+', '-', '*', '/', '^'};

static void test_calc() {
#define clear_all()                                                            \
  stack_char_clear(&op);                                                       \
  stack_double_clear(&num);

  printf("\ntest_calc() started\n");
  fflush(stdout);

  // not enough operators
  stack_char *op = NULL;
  stack_double *num = NULL;
  for (int j = 0; j <= 2; j++) {
    for (int jj = 0; jj < j; jj++)
      stack_double_push(&num, rand_double());

    char ret = calc(&op, &num);
    assert(ret == -1);
    stack_double_clear(&num);
  }
  printf("\ttest_no_operators succeeded\n");
  fflush(stdout);

  // not enough numbers
  for (int i = 0; i < OPS; i++) {
    for (int j = 0; j < 2; j++) {
      stack_char_push(&op, ops[i]);
      for (int jj = 0; jj < j; jj++)
        stack_double_push(&num, rand_double());

      char ret = calc(&op, &num);
      assert(ret == -2);
      clear_all();
    }
  }
  printf("\ttest_insufficient_numbers succeeded\n");
  fflush(stdout);

  // testing divide by zero
  stack_double_push(&num, rand_int(-5, 5));
  stack_double_push(&num, EPSILON / rand_int(2, 10000));
  stack_char_push(&op, '/');
  char ret = calc(&op, &num);
  assert(ret == 1);
  clear_all();
  printf("\ttest_division_by_zero succeeded\n");
  fflush(stdout);

  // testing NANs
  stack_double_push(&num, rand_int(-100, -1));
  stack_double_push(&num, 0.5);
  stack_char_push(&op, '^');
  ret = calc(&op, &num);
  assert(ret == 2);
  clear_all();
  printf("\ttest_nans succeeded\n");
  fflush(stdout);

  // testing normal operation
  for (int i = 0; i < OPS; i++) {
    stack_char_push(&op, ops[i]);
    stack_double_push(&num, rand_int(-100, 100));
    stack_double_push(&num, rand_int(-100, 100));

    char ret = calc(&op, &num);
    assert(ret == 0 && op == NULL && num != NULL && num->next == NULL);
    clear_all();
  }
  printf("\ttest_normal_operation succeeded\n");

  printf("test_calc() succeeded\n");
  fflush(stdout);
#undef clear_all
}

static void test_all_signs(char *eq, char ans, int start, int from) {
  char old = eq[start];
  assert(isnan(calculator_atolf(eq + from)) == !ans);
  eq[start] = '+';
  assert(isnan(calculator_atolf(eq)) == !ans);
  eq[start] = '-';
  assert(isnan(calculator_atolf(eq)) == !ans);
  eq[start] = old;
}

static void test_calculator_atolf() {
  printf("\ntest_calculator_atolf() started\n");
  fflush(stdout);

  char *eq = (char *)malloc(sizeof(char) * MAX_SIZE);

  // empty equation
  sprintf(eq, " ");
  test_all_signs(eq, false, 0, 1);
  printf("\ttest_empty_equation succeeded\n");
  fflush(stdout);

  // proper number
  sprintf(eq, " %lf", rand_proper_double());
  test_all_signs(eq, true, 0, 1);
  printf("\ttest_proper_number succeeded\n");
  fflush(stdout);

  // whitespaces
  int spaces_before = rand_int(1, 10), spaces_after = rand_int(1, 10);
  int n = 0;
  for (; n < spaces_before; n++)
    sprintf(eq + n, " ");
  n += sprintf(eq + n, " %lf", rand_proper_double());
  for (int i = 0; i < spaces_after; i++, n++)
    sprintf(eq + n, " ");
  test_all_signs(eq, true, spaces_before, 0);
  printf("\ttest_spaces succeeded\n");
  fflush(stdout);

  // unknown characters at the end
  sprintf(eq + n, "%c", rand_char('a', 'z'));
  test_all_signs(eq, false, spaces_before, 0);
  sprintf(eq + n, "");
  printf("\ttest_bad_char_at_end succeeded\n");
  fflush(stdout);

  // unknown characters at the beginning
  sprintf(eq + spaces_before - 1, "%c", rand_char('a', 'z'));
  test_all_signs(eq, false, spaces_before, 0);
  printf("\ttest_bad_char_at_beg succeeded\n");
  fflush(stdout);

  free(eq);
  printf("test_calculator_atolf() succeeded\n");
  fflush(stdout);
}

static double test_error_code(const char *eq, double x, int ans) {
  if (isnan(x))
    x = rand_proper_double();

  double y;
  char *dummy = NULL;
  char err = calculator_eval(eq, x, &y, &dummy);
  free(dummy);

  assert(isnan(y) == !!ans && err == ans);
  return y;
}

static void test_calculator_eval() {
  printf("\ntest_calculator_eval() started\n");
  fflush(stdout);

  char *eq = (char *)malloc(sizeof(char) * MAX_SIZE);

  // test error -2
  sprintf(eq, "x*3.3 *    ");
  test_error_code(eq, NAN, -2);
  printf("\ttest_error_-2 succeeded\n");
  fflush(stdout);

  // test error -1
  sprintf(eq, "3.3 * x 10.1");
  test_error_code(eq, NAN, -1);
  sprintf(eq, "10.1 x *");
  test_error_code(eq, NAN, -1);
  printf("\ttest_error_-1 succeeded\n");
  fflush(stdout);

  // test error 1
  sprintf(eq, "x / %lf", EPSILON / rand_int(2, 10000));
  test_error_code(eq, NAN, 1);
  printf("\ttest_error_1 succeeded\n");
  fflush(stdout);

  // test error 2
  sprintf(eq, "-x ^ 0.5");
  test_error_code(eq, -5, 2);
  printf("\ttest_error_2 succeeded\n");
  fflush(stdout);

  sprintf(eq, "-2/3 * x^0.04 - x^0.2 - -8^2");
  test_error_code(eq, -5, 2);
  printf("\ttest_error_2 succeeded\n");
  fflush(stdout);

  // test error 3
  sprintf(eq, "x**3.3");
  test_error_code(eq, NAN, 3);
  printf("\ttest_error_3 succeeded\n");
  fflush(stdout);

  // test error 4
  sprintf(eq, "x* u *3.3");
  test_error_code(eq, NAN, 4);
  printf("\ttest_error_4 succeeded\n");
  fflush(stdout);

  // test error 5
  sprintf(eq, "x*3...3");
  test_error_code(eq, NAN, 5);
  printf("\ttest_error_5 succeeded\n");
  fflush(stdout);

  // test a proper equation's result
  sprintf(eq, "2/-3 * x^-2 + x^3 - -8^2");
  double y = test_error_code(eq, 5, 0);
  assert(isequal(y, 14173.0 / 75));
  printf("\ttest_proper_equation_result succeeded\n");
  fflush(stdout);

  sprintf(eq, "-2/3 * x^2 - x^3 + -8^2");
  y = test_error_code(eq, 5, 0);
  assert(isequal(y, -617.0 / 3));
  printf("\ttest_proper_equation_result succeeded\n");
  fflush(stdout);

  free(eq);
  printf("test_calculator_eval() succeeded\n");
  fflush(stdout);
}
