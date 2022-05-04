#include "calculator.h"
#include "stack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPS 5
static const char ops[OPS] = {'+', '-', '*', '/', '^'};
static const char deps[OPS][OPS] = {{'+', '-', '*', '/', '^'},
                                    {'+', '-', '*', '/', '^'},
                                    {'*', '/', '^'},
                                    {'*', '/', '^'},
                                    {}};

char calc(stack_char **op, stack_double **num) {
  if (!*op)
    return -1;
  if (!(*num && (*num)->next))
    return -2;
  double s = stack_double_pop(num);
  double f = stack_double_pop(num);
  char operation = stack_char_pop(op);
  if (operation == '+')
    stack_double_push(num, f + s);
  else if (operation == '-')
    stack_double_push(num, f - s);
  else if (operation == '*')
    stack_double_push(num, f * s);
  else if (operation == '/') {
    if (isless(fabs(s), EPSILON))
      return 1;
    stack_double_push(num, f / s);
  } else if (operation == '^') {
    stack_double_push(num, pow(f, s));
    if (isnan((*num)->top))
      return 2;
  }
  return 0;
}

char calc_error(char *message, char err) {
  if (err == -2)
    sprintf(message, "An extra operation was detected at the end");
  else if (err == 1)
    sprintf(message, "A Division by zero was detected");
  else if (err == 2)
    sprintf(message, "A fractional power for negative numbers was detected");
  else
    sprintf(message, "An unknown error was detected");
  return err;
}

double parse_number(const char *arr, int *i) {
  double y = 0;
  while (arr[*i] && arr[*i] >= '0' && arr[*i] <= '9') {
    y *= 10;
    y += arr[*i] - '0';
    (*i)++;
  }
  if (arr[*i] == '.') {
    (*i)++;
    double pre = 1;
    while (arr[*i] && arr[*i] >= '0' && arr[*i] <= '9') {
      pre /= 10;
      y += (arr[*i] - '0') * pre;
      (*i)++;
    }
  }
  (*i)--;
  return y;
}

double calculator_atolf(const char *snum) {
  int i = 0;
  while (snum[i] == ' ')
    i++;
  char op = '+';
  if (snum[i] == '+' || snum[i] == '-')
    op = snum[i++];
  if (snum[i] == '\0')
    return NAN;
  double num = parse_number(snum, &i);
  if (op == '-')
    num *= -1;
  i++;
  while (snum[i] == ' ')
    i++;
  if (snum[i] == '\0')
    return num;
  return NAN;
}

void double_nums_error(char *message, const char *arr, int i, const int size) {
  int n = sprintf(message, "Double numbers were detected here: ...");
  for (int js = (int)fmax(i - THRESHB, 0),
           je = (int)fmin(size - 1, i + THRESHF);
       js <= je; js++) {
    sprintf(message + n, "%c", arr[js]);
    n++;
  }
  sprintf(message + n, " ...");
}

void double_op_error(char *message, char last_op, const char *arr, int i,
                     const int size) {
  int n = sprintf(message, "Double operations \"%c%c\" were detected here: ...",
                  last_op, arr[i]);
  for (int js = (int)fmax(i - 1 - THRESHB, 0),
           je = (int)fmin(size - 1, i + THRESHF - 1);
       js <= je; js++) {
    sprintf(message + n, "%c", arr[js]);
    n++;
  }
  sprintf(message + n, " ...");
}

void undefined_char_error(char *message, const char *arr, int i,
                          const int size) {
  int n = sprintf(
      message, "An undefined character '%c' was detected here: ... ", arr[i]);
  for (int js = (int)fmax(i - THRESHB, 0),
           je = (int)fmin(size - 1, i + THRESHF);
       js <= je; js++) {
    sprintf(message + n, "%c", arr[js]);
    n++;
  }
  sprintf(message + n, " ...");
}

void double_decimal_error(char *message, const char *arr, int i,
                          const int size) {
  int n = sprintf(message, "Two decimal points were detected here: ... ");
  for (int js = (int)fmax(i - THRESHB, 0),
           je = (int)fmin(size - 1, i + THRESHF);
       js <= je; js++) {
    sprintf(message + n, "%c", arr[js]);
    n++;
  }
  sprintf(message + n, " ...");
}

char process_op(char arr_i, char *last_op, stack_char **op,
                stack_double **num) {
  char type = 0;
  for (int j = 0; j < OPS; j++)
    if (arr_i == ops[j]) {
      type = j + 1;
      break;
    }
  if (type) {
    if (*last_op && arr_i != '+' && arr_i != '-')
      return 3;

    if (*last_op == '+' || *last_op == '-') {
      if (*last_op == arr_i)
        *last_op = '+';
      else
        *last_op = '-';
    } else if (*last_op)
      *last_op = arr_i;
    else {
      type--;

      while (*op) {
        char deptype = 0;
        for (int j = 0; j < OPS; j++)
          if ((*op)->top == deps[type][j]) {
            deptype = j + 1;
            break;
          }
        if (!deptype)
          break;

        char ret = calc(op, num);
        if (ret)
          return ret;
      }

      if (arr_i == '+' || arr_i == '-')
        stack_char_push(op, '+');
      else
        stack_char_push(op, arr_i);
      *last_op = arr_i;
    }
    return 0;
  }
  return 6;
}

char process_num(const char *arr, int *i, char *last_op, stack_double **num) {
  if (arr[*i] != '.' && (arr[*i] < '0' || arr[*i] > '9'))
    return 4;

  if (!*last_op)
    return -1;

  double y = parse_number(arr, i);
  if (arr[*i + 1] == '.')
    return 5;

  if (*last_op == '-')
    y *= -1;
  *last_op = 0;
  stack_double_push(num, y);
  return 0;
}

char calculator_eval(const char *arr, double x, double *ans, char *message) {
#define clear_all()                                                            \
  stack_char_clear(&op);                                                       \
  stack_double_clear(&num);

  const int size = strlen(arr);
  stack_double *num = NULL;
  stack_char *op = NULL;

  stack_double_push(&num, 0);
  stack_char_push(&op, '+');
  char last_op = '+';

  for (int i = 0; arr[i] != '\0'; i++) {
    if (arr[i] == ' ')
      continue;

    if (arr[i] == 'x') {
      if (!last_op) {
        double_nums_error(message, arr, i, size);
        *ans = NAN;
        clear_all();
        return -1;
      }
      double y = x;
      if (last_op == '-')
        y *= -1;
      last_op = 0;
      stack_double_push(&num, y);
      continue;
    }

    char ret = process_op(arr[i], &last_op, &op, &num);
    if (ret == 6)
      ret = process_num(arr, &i, &last_op, &num);
    if (!ret)
      continue;

    if (ret == -1)
      double_nums_error(message, arr, i, size);
    else if (ret == 3)
      double_op_error(message, last_op, arr, i, size);
    else if (ret == 4)
      undefined_char_error(message, arr, i, size);
    else if (ret == 5)
      double_decimal_error(message, arr, i + 1, size);
    else
      calc_error(message, ret);
    *ans = NAN;
    clear_all();
    return ret;
  }

  while (op || num->next) {
    char ret = calc(&op, &num);
    if (ret) {
      calc_error(message, ret);
      *ans = NAN;
      clear_all();
      return ret;
    }
  }

  *ans = num->top;
  sprintf(message, "");
  clear_all();
  return 0;
#undef end
}
