#include <editline/readline.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

/* gcc main.c mpc.c -ledit -lm -g */
char *ltype_name(int t);

#define LASSERT(args, cond, fmt, ...)                                          \
  if (!(cond)) {                                                               \
    lval *err = lval_err(fmt, ##__VA_ARGS__);                                  \
    lval_del(args);                                                            \
    return err;                                                                \
  }

#define LASSERT_TYPE(func, args, index, expect)                                \
  LASSERT(args, args->cell[index]->type == expect,                             \
          "Function '%s' passed incorrect type for argument %i. "              \
          "Got %s, Expected %s.",                                              \
          func, index, ltype_name(args->cell[index]->type),                    \
          ltype_name(expect))

#define LASSERT_NUM(func, args, num)                                           \
  LASSERT(args, args->count == num,                                            \
          "Function '%s' passed incorrect number of arguments. "               \
          "Got %i, Expected %i.",                                              \
          func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index)                                   \
  LASSERT(args, args->cell[index]->count != 0,                                 \
          "Function '%s' passed {} for argument %i.", func, index);

/* Forward Declarations */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
mpc_parser_t *Number;
mpc_parser_t *Symbol;
mpc_parser_t *String;
mpc_parser_t *Comment;
mpc_parser_t *Sexpr;
mpc_parser_t *Qexpr;
mpc_parser_t *Expr;
mpc_parser_t *Lispy;

/* Lisp Value */
enum {
  LVAL_NUM,
  LVAL_ERR,
  LVAL_SYM,
  LVAL_SEXPR,
  LVAL_QEXPR,
  LVAL_FUN,
  LVAL_STR
};
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef lval *(*lbuiltin)(lenv *, lval *);

typedef struct lval {
  int type;

  /* basic */
  long num;
  char *err;
  char *sym;
  char *str;

  /* function */
  lbuiltin builtin;
  lenv *env;
  lval *formals;
  lval *body;

  /* expressions */
  int count;
  struct lval **cell;
} lval;

struct lenv {
  lenv *par;
  int count;
  char **syms;
  lval **vals;
};

/* Function Prototypes */

void lval_print(lval *v);
void lval_del(lval *v);
lval *lval_add(lval *v, lval *x);
lval *lval_sexpr(void);
lval *lval_eval(lenv *e, lval *v);
lval *lval_take(lval *v, int i);
lval *lval_pop(lval *v, int i);
lval *lval_copy(lval *v);
lval *lval_err(char *fmt, ...);
lval *lval_sym(char *s);
lval *lval_fun(lbuiltin func);
lval *lval_lambda(lval *formals, lval *body);

void lenv_put(lenv *e, lval *k, lval *v);

lval *builtin(lenv *e, lval *a, char *func);
lval *builtin_put(lenv *e, lval *a);
lval *builtin_op(lenv *e, lval *a, char *op);
lval *builtin_list(lenv *e, lval *a);
lval *builtin_head(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_add(lenv *e, lval *a);
lval *builtin_tail(lenv *e, lval *a);
lval *builtin_eval(lenv *e, lval *a);
lval *builtin_join(lenv *e, lval *a);
lval *builtin_def(lenv *e, lval *a);

lval *builtin_if(lenv *e, lval *a);
lval *builtin_eq(lenv *e, lval *a);
lval *builtin_ne(lenv *e, lval *a);
lval *builtin_gt(lenv *e, lval *a);
lval *builtin_lt(lenv *e, lval *a);
lval *builtin_le(lenv *e, lval *a);
lval *builtin_ge(lenv *e, lval *a);
lval *builtin_load(lenv *e, lval *a);
lval *builtin_print(lenv *e, lval *a);
lval *builtin_error(lenv *e, lval *a);

