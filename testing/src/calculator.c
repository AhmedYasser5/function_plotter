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

/* does only one operation using an operator from the top of the stack along
 * with two numbers from the other stack. It returns a non-zero integer if it
 * finds an error */
char calc(stack_char **op, stack_double **num) {
  if (!*op)
    return -1; // op stack is empty
  if (!(*num && (*num)->next))
    return -2; // num stack should have at least two numbers

  // the second operand should be on top of the stack followed by the first
  // operand
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
      return 1; // this translates to a division by zero
    stack_double_push(num, f / s);
  } else if (operation == '^') {
    stack_double_push(num, pow(f, s));
    if (isnan((*num)->top))
      return 2; // this translates to fractional power applied to negative
                // numbers
  }

  return 0; // everything is fine
}

/* puts the correct message corresponding to the error code given */
static char calc_error(char *message, char err) {
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

/* reads a number (integers and doubles) */
static double parse_number(const char *arr, int *i) {
  double y = 0;
  while (arr[*i] >= '0' && arr[*i] <= '9') {
    y *= 10;
    y += arr[*i] - '0';
    (*i)++;
  }

  if (arr[*i] == '.') {
    (*i)++;
    double pre = 1;
    while (arr[*i] >= '0' && arr[*i] <= '9') {
      pre /= 10;
      y += (arr[*i] - '0') * pre;
      (*i)++;
    }
  }

  // an extra increment was made
  (*i)--;
  return y;
}

/* converts a char array into a double */
double calculator_atolf(const char *snum) {
  int i = 0;
  // ignore whitespaces
  while (snum[i] == ' ')
    i++;

  char op = '+';
  if (snum[i] == '+' || snum[i] == '-')
    op = snum[i++];

  if (snum[i] == '\0')
    return NAN; // array is empty

  double num = parse_number(snum, &i);
  if (op == '-')
    num *= -1;

  // ignore whitespaces
  i++;
  while (snum[i] == ' ')
    i++;

  if (snum[i] == '\0')
    return num; // number is valid

  return NAN; // unwanted characters detected at the end
}

/* processes the error message of "double numbers error" */
static void double_nums_error(char *message, const char *arr, int i,
                              const int size) {
  int n = sprintf(message, "Double numbers were detected here: ...");

  for (int js = (int)fmax(i - THRESHB, 0),
           je = (int)fmin(size - 1, i + THRESHF);
       js <= je; js++) {
    sprintf(message + n, "%c", arr[js]);
    n++;
  }

  sprintf(message + n, " ...");
}

/* processes the error message of "double operators error" */
static void double_op_error(char *message, char last_op, const char *arr, int i,
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

/* processes the error message of "undefined character error" */
static void undefined_char_error(char *message, const char *arr, int i,
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

/* processes the error message of "double decimal points error" */
static void double_decimal_error(char *message, const char *arr, int i,
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

/* processes the dependencies of the new operator and adds it to the stack */
static char process_op(char arr_i, char *last_op, stack_char **op,
                       stack_double **num) {
  // identifying the new operator
  char type = 0;
  for (int j = 0; j < OPS; j++)
    if (arr_i == ops[j]) {
      type = j + 1;
      break;
    }

  // if it is not an operator, don't continue
  if (type == 0)
    return 6;

  // if the new operator is not '+' or '-', and was preceded by any other
  // operator, then it is definitely invalid
  if (*last_op && arr_i != '+' && arr_i != '-')
    return 3;

  // the first case is that each of the last operator and the new operator is
  // '+' or '-'
  if (*last_op == '+' || *last_op == '-') {
    if (*last_op == arr_i)
      *last_op = '+';
    else
      *last_op = '-';
  } else if (*last_op) { // or the last operator was '+' or '-' but the new one
                         // is any of the other 3 operators
    *last_op = arr_i;
  } else { // otherwise, it is a new operator that was preceded by a number
    type--;

    // process dependencies:
    // 1- before adding a '+' or a '-', no '+'s, '-'s, '*'s, '/'s, and '^'s
    // should be found on top of the operator stack
    // 2- before adding a '*' or a '/', no '*'s, '/'s, and '^'s should be found
    // on top of the operator stack
    while (*op) {
      char deptype = 0;
      for (int j = 0; j < OPS; j++)
        if ((*op)->top == deps[type][j]) {
          deptype = j + 1;
          break;
        }
      // if it is not one of the dependencies, break
      if (!deptype)
        break;

      char ret = calc(op, num);
      if (ret) // handling computation errors
        return ret;
    }

    // if it is '+' or '-', an addition will be performed, but a negative sign
    // will be added the following number
    if (arr_i == '+' || arr_i == '-')
      stack_char_push(op, '+');
    else
      stack_char_push(op, arr_i);
    *last_op = arr_i;
  }

  return 0;
}

/* this function is similar to atolf, but handles errors as expected from
 * calculator_eval() point of view */
static char process_num(const char *arr, int *i, char *last_op, stack_char **op,
                        stack_double **num) {
  if (arr[*i] != '.' && (arr[*i] < '0' || arr[*i] > '9'))
    return 4; // undefined character error

  if (!*last_op)
    return -1; // double numbers

  double y = parse_number(arr, i);
  if (arr[*i + 1] == '.')
    return 5; // double decimal points

  // if there were a previous sign, this sign is appended to the stack but
  // dependencies should not be calculated
  // they are calculated after appending the negative sign to the result coming
  // from the right
  // for example, 2^-2^2 = 2^(-1*(2^2)) = 2^(-1*4)
  // also, 2*-3*4 = 2*(-1*3*4)
  // also, 2+-2 = 2+(-1*2)
  if (*last_op == '-') {
    stack_double_push(num, -1);
    stack_char_push(op, '*');
  }

  *last_op = 0; // report that the last entity was a number
  stack_double_push(num, y);
  return 0;
}

/* the main function that evaluates a function at a given x, while detecting
 * errors */
char calculator_eval(const char *arr, double x, double *ans, char *message) {
#define clear_all()                                                            \
  stack_char_clear(&op);                                                       \
  stack_double_clear(&num);

  const int size = strlen(arr);
  stack_double *num = NULL;
  stack_char *op = NULL;

  // adding "0 + " to the beginning of stacks helps when the function starts
  // with a negative number
  stack_double_push(&num, 0);
  stack_char_push(&op, '+');
  char last_op = '+';

  for (int i = 0; arr[i] != '\0'; i++) {
    // ignore whitespaces
    if (arr[i] == ' ')
      continue;

    char ret = 0;
    if (arr[i] == 'x') {
      if (!last_op)
        ret = -1; // double numbers
      else {
        // if there were a previous sign, this sign is appended to the stack but
        // dependencies should not be calculated
        // they are calculated after appending the negative sign to the result
        // coming from the right for example, 2^-x^2 = 2^(-1*(x^2))
        // also, 2*-x*4 = 2*(-1*x*4)
        // also, 2+-x = 2+(-1*x)
        if (last_op == '-') {
          stack_double_push(&num, -1);
          stack_char_push(&op, '*');
        }

        last_op = 0;
        stack_double_push(&num, x);
      }
    } else {
      ret = process_op(arr[i], &last_op, &op, &num);
      if (ret == 6) // if not an operator ->
        ret = process_num(arr, &i, &last_op, &op, &num);
    }

    if (!ret)
      continue;

    // handling different errors
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

  // processing the remaining operators and numbers
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
#undef clear_all
}
