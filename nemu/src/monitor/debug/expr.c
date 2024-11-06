#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NEQ,
  TK_NUM,
  TK_HEX_NUM,
  TK_AND,
  TK_OR,
  TK_REG,
  TK_NOT,
  TK_NEG,
  TK_POINT
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
            tokens[nr_token].type = rules[i].token_type;
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
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start + 2, substr_len - 2);
            tokens[nr_token].str[substr_len - 2] = '\0';
            ++nr_token;
            break;
          }
          case TK_REG:{
            tokens[nr_token].type = rules[i].token_type;  
            strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
            tokens[nr_token].str[substr_len-1] = '\0';
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
          case TK_OR:
          default: {
            tokens[nr_token++].type = rules[i].token_type;
            break;
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // printf("nr_token: %d\n", nr_token);

  return true;
}

bool check_parentheses(int start, int end) {
    if (tokens[start].type != '(' || tokens[end].type != ')') {
        return false;
    }
    int balance = 0;
    for (int i = start + 1; i < end; i++) {
        if (tokens[i].type == '(') {
            balance++;
        } else if (tokens[i].type == ')') {
            balance--;
        }

        if (balance < 0) {
            return false;
        }
    }
    return balance == 0;
}

// uint32_t expr(char *e, bool *success) {
//   printf("%s\n", e);
//   if (!make_token(e)) {
//     *success = false;
//     return 0;
//   }
//   // printf("Finish the expression!\n");
//   /* TODO: Insert codes to evaluate the expression. */
//   TODO();

//   return 0;
// }

int find_main_op(int start, int end) {
  int op = -1;
  int unpair = 0;
  int priority = 10; 

  for (int i = start; i <= end; i++) {
    if (tokens[i].type == '(') unpair++;
    else if (tokens[i].type == ')') unpair--;

    if (unpair == 0) {
      int current_priority = -1;

      switch (tokens[i].type) {
        case '+': case '-': current_priority = 1; break;
        case '*': case '/': current_priority = 2; break;
        case TK_EQ: case TK_NEQ: current_priority = 3; break;
        case TK_OR: current_priority = 4; break;
        case TK_AND: current_priority = 5; break;
        case TK_NOT: current_priority = 6; break;
        case TK_POINT: current_priority = 7; break;
        case TK_NEG: current_priority = 8; break;
        default: current_priority = 10; break;
      }

      if (current_priority <= priority) {
        op = i;
        priority = current_priority;
      }
    }
  }

  if (op == -1) printf("Can't find main operator...\n");
  // printf("OP: %d\n", op);
  return op;
}

uint32_t eval(int start, int end, bool* success) {
  if (start > end) {
    printf("Invalid sub-expression...\n");
    return 0;
  }

  if (start == end) {
    if (tokens[start].type == TK_NUM) {
      return strtol(tokens[start].str, NULL, 10);
    } else if (tokens[start].type == TK_HEX_NUM) {
      return strtol(tokens[start].str, NULL, 16);
    } else if (tokens[start].type == TK_REG) {
      return isa_reg_str2val(&(tokens[start].str[1]), success);
    } else {
      printf("Single token is not a valid number...\n");
      return 0;
    }
  } else if (check_parentheses(start, end)) {
    return eval(start + 1, end - 1, success);
  } else {
    int op = find_main_op(start, end);
    if (op == -1) {
      printf("No main operator found...\n");
      return 0;
    }

    int val1 = eval(start, op - 1, success);
    int val2 = eval(op + 1, end, success);
    
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': 
        if (val2 == 0) {
          printf("Division by zero error...\n");
          assert(0);
        }
        return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_OR: return val1 || val2;
      case TK_NOT: return !val2;
      case TK_NEG: return -val2;
      case TK_POINT: return paddr_read(val2, 4);
      default: assert(0); break;
    }
  }

  return 0; // 默认返回值
}

// 解析表达式并计算结果
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("Invalid tokens in expression...\n");
    return 0;
  }

  // 转换负号和星号为 TK_NEG 和 TK_POINT
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '-' && (i == 0 || tokens[i-1].type == '(' || tokens[i-1].type == TK_EQ || tokens[i-1].type == TK_NEQ || tokens[i-1].type == '-')) {
      tokens[i].type = TK_NEG;
      printf("TURN ONCE!\n");
    }
    if (tokens[i].type == '*' && (i == 0 || tokens[i-1].type == '+' || tokens[i-1].type == '-' || tokens[i-1].type == '*' || tokens[i-1].type == '/' || tokens[i-1].type == '(' || tokens[i-1].type == TK_EQ || tokens[i-1].type == TK_NEQ)) {
      tokens[i].type = TK_POINT;
    }
  }

  uint32_t result = eval(0, nr_token - 1, success);
  if (!(*success)) {
    printf("Expression evaluation failed...\n");
  }

  return result;
}
