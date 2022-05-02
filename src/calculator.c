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

char *eval(char arr[MAX], double x) {
#define end()                                                                  \
  clear_char(&op);                                                             \
  clear_double(&num);                                                          \
  return message;

  char *message = (char *)malloc(sizeof(char) * MAX);
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
        arr[(int)fmin(MAX - 1, i + THRESHF)] = '\0';
        sprintf(message, "Double operations detected here: ... %s ...",
                arr + (int)fmax(0, i - 1 - THRESHB));
        end();
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
            end();
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
      arr[(int)fmin(i + THRESHF, MAX - 1)] = '\0';
      sprintf(message, "Undefined character '%c' here: ... %s ...", arr[i],
              arr + (int)fmax(i - THRESHB, 0));
      end();
    }

    int j = (int)fmax(i - THRESHB, 0);
    double y = parse_number(arr, &i);
    if (arr[i + 1] == '.') {
      arr[(int)fmin(i + THRESHF, MAX - 1)] = '\0';
      sprintf(message, "Two decimal points detected here: ... %s ...", arr + j);
      end();
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
      end();
    }
  }
  printf("%lf", num->top);
  sprintf(message, "");
  end();
#undef end
}
