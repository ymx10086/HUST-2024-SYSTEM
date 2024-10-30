#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NEQ,
  TK_NUM,
  TK_HEX_NUM,
  TK_AND,
  TK_OR,
  TK_REG,
  TK_NOT
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"\\-", '-'},         // minus
  {"0x[0-9a-fA-F]+", TK_HEX_NUM}, // hex number
  {"[0-9]+", TK_NUM},   // number
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},    // or
  {"\\$[0-9a-zA-Z]+", TK_REG}, // register
  {"!=", TK_NEQ}  ,     // not equal
  {"!", TK_NOT}         // no   
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          case TK_NUM:{
            if (substr_len > 32) { 
              puts("The length of number is too long! "); 
              return false;
            }
            tokens[nr_token].type = '0';
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            ++nr_token;
            break;
          }
          case TK_HEX_NUM:{
            if (substr_len > 32) { 
              puts("The length of number is too long!"); 
              return false; 
            }
            tokens[nr_token].type = '6';
            strncpy(tokens[nr_token].str, substr_start + 2, substr_len - 2);
            tokens[nr_token].str[substr_len - 2] = '\0';
            ++nr_token;
            break;
          }

          // brackets
          case '(':
          case ')':

          // operators
          case '*':
          case '/':
          case '-':
          case '+':
          case TK_EQ:
          case TK_NEQ:
          case TK_AND:
          case TK_OR:{
            tokens[nr_token++].type = rules[i].token_type;
            break;
          }

          case TK_REG:{
            tokens[nr_token].type = 'r';  
            strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
            tokens[nr_token].str[substr_len-1] = '\0';
            ++nr_token;
            break;
          }
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  printf("nr_token: %d\n", nr_token);
  printf("token_str: %s\n", tokens[0].str);
  printf("token_type: %d\n", tokens[0].type);
  printf("token_str: %s\n", tokens[1].str);
  printf("token_type: %d\n", tokens[1].type);
  printf("token_str: %s\n", tokens[2].str);
  printf("token_type: %d\n", tokens[2].type);

  return true;
}

uint32_t expr(char *e, bool *success) {
  printf("%s\n", e);
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // printf("Finish the expression!\n");
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
