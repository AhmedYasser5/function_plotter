#include "calculator.h"
#include "stack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPS 5
const char ops[OPS] = {'+', '-', '*', '/', '^'};
const char deps[OPS][OPS] = {{'+', '-', '*', '/', '^'},
                             {'+', '-', '*', '/', '^'},
                             {'*', '/', '^'},
                             {'*', '/', '^'}};

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

char calc_error(char **message, char err) {
  if (err == -2)
    sprintf(*message, "Extra operations detected");
  else if (err == -1)
    sprintf(*message, "Extra numbers detected");
  else if (err == 1)
    sprintf(*message, "Divide by zero detected");
  else if (err == 2)
    sprintf(*message, "Fractional power with negative numbers detected");
  else
    sprintf(*message, "Unknown error detected");
  fprintf(stderr, "%d\n", err);
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
  if (snum[0] == '\0')
    return NAN;
  int i = 0;
  if (snum[0] == '-' || snum[0] == '+')
    i++;
  double num = parse_number(snum, &i);
  if (snum[0] == '-')
    num *= -1;
  if (snum[i + 1] == '\0')
    return num;
  return NAN;
}

double calculator_eval(const char *arr, double x, char *message) {
#define clear_all()                                                            \
  stack_char_clear(&op);                                                       \
  stack_double_clear(&num);

  const int size = strlen(arr);
  stack_double *num = NULL;
  stack_char *op = NULL;
  stack_double_push(&num, 0);
  stack_char_push(&op, '+');
  char last_op = '+';
  for (int i = 0; arr[i]; i++) {
    if (arr[i] == ' ')
      continue;

    if (arr[i] == 'x') {
      last_op = 0;
      stack_double_push(&num, x);
      continue;
    }

    char type = 0;
    for (int j = 0; j < OPS; j++)
      if (arr[i] == ops[j]) {
        type = j + 1;
        break;
      }
    if (type) {
      if ((arr[i] != '-' && last_op) ||
          (arr[i] == '-' && (arr[i + 1] == '-' || last_op == '-'))) {
        int n =
            sprintf(message, "Double operations detected \"%c%c\" here: ... ",
                    last_op, arr[i]);
        for (int js = (int)fmax(i - 1 - THRESHB, 0),
                 je = (int)fmin(size - 1, i + THRESHF - 1);
             js <= je; js++) {
          sprintf(message + n, "%c", arr[js]);
          n++;
        }
        sprintf(message + n, " ...");
        fprintf(stderr, "3\n");
        clear_all();
        return NAN;
      }
      if (!last_op) {
        type--;
        while (op) {
          char deptype = 0;
          for (int j = 0; j < OPS; j++)
            if (op->top == deps[type][j]) {
              deptype = j + 1;
              break;
            }
          if (!deptype)
            break;
          char ret;
          if (ret = calc(&op, &num)) {
            calc_error(&message, ret);
            clear_all();
            return NAN;
          }
        }
        stack_char_push(&op, arr[i]);
        last_op = arr[i];
        continue;
      } else {
        last_op = 'n';
        i++;
      }
    }

    if (arr[i] != '.' && (arr[i] < '0' || arr[i] > '9')) {
      int n = sprintf(message, "Undefined character '%c' here: ... ", arr[i]);
      for (int js = (int)fmax(i - THRESHB, 0),
               je = (int)fmin(size - 1, i + THRESHF);
           js <= je; js++) {
        sprintf(message + n, "%c", arr[js]);
        n++;
      }
      sprintf(message + n, " ...");
      fprintf(stderr, "4\n");
      clear_all();
      return NAN;
    }

    int j = (int)fmax(i - THRESHB, 0);
    double y = parse_number(arr, &i);
    if (arr[i + 1] == '.') {
      int n = sprintf(message, "Two decimal points detected here: ... ");
      for (int js = (int)fmax(i - THRESHB, 0),
               je = (int)fmin(size - 1, i + THRESHF);
           js <= je; js++) {
        sprintf(message + n, "%c", arr[js]);
        n++;
      }
      sprintf(message + n, " ...");
      fprintf(stderr, "5\n");
      clear_all();
      return NAN;
    }
    if (last_op == 'n')
      y *= -1;
    last_op = 0;
    stack_double_push(&num, y);
  }

  while (op || num->next) {
    char ret;
    if (ret = calc(&op, &num)) {
      calc_error(&message, ret);
      clear_all();
      return NAN;
    }
  }

  double ans = num->top;
  sprintf(message, "");
  clear_all();
  return ans;
#undef end
}
