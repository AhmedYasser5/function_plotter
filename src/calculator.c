#include "calculator.h"

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
  double s = pop_double(num);
  double f = pop_double(num);
  char operation = pop_char(op);
  if (operation == '+')
    push_double(num, f + s);
  else if (operation == '-')
    push_double(num, f - s);
  else if (operation == '*')
    push_double(num, f * s);
  else if (operation == '/') {
    if (!isless(s, 0) && !isless(0, s))
      return 1;
    push_double(num, f / s);
  } else if (operation == '^')
    push_double(num, pow(f, s));
  return 0;
}

void calc_error(char **message, char err) {
  if (err == 1)
    sprintf(*message, "Divide by zero error");
  else if (err == -1)
    sprintf(*message, "Extra numbers detected");
  else if (err == -2)
    sprintf(*message, "Extra operations detected");
  else
    sprintf(*message, "Unknown error");
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

double eval(const char *arr, double x, char *message) {
#define clear_all()                                                            \
  clear_char(&op);                                                             \
  clear_double(&num);

  const int size = strlen(arr);
  stack_double *num = NULL;
  stack_char *op = NULL;
  push_double(&num, 0);
  push_char(&op, '+');
  char last_op = '+';
  for (int i = 0; arr[i]; i++) {
    if (arr[i] == ' ')
      continue;

    if (arr[i] == 'x') {
      last_op = 0;
      push_double(&num, x);
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
        clear_all();
        return -INFINITY;
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
            return -INFINITY;
          }
        }
        push_char(&op, arr[i]);
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
      clear_all();
      return -INFINITY;
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
      clear_all();
      return -INFINITY;
    }
    if (last_op == 'n')
      y *= -1;
    last_op = 0;
    push_double(&num, y);
  }

  while (op || num->next) {
    char ret;
    if (ret = calc(&op, &num)) {
      calc_error(&message, ret);
      clear_all();
      return -INFINITY;
    }
  }

  double ans = num->top;
  sprintf(message, "");
  clear_all();
  return ans;
#undef end
}
