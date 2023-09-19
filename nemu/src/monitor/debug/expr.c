#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,

  /* TODO: Add more token types */

};

static struct rule
{
  char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},  // spaces
    {"\\+", '+'},       // plus
    {"==", TK_EQ},      // equal
    {"-", '-'},         // minus
    {"\\*", '*'},       // multiply
    {"/", '/'},         // division
    {"\\(", '('},       // left-half bracket
    {"\\)", ')'},       // right-half bracket
    {"[0-9]+", TK_NUM}, // nums

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        /*Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '(':
        case ')':
        case TK_EQ:
          tokens[nr_token].type = rules[i].token_type;
          nr_token++;
          break;
        case TK_NUM:
          tokens[nr_token].type = TK_NUM;
          // printf("%.*s\n", substr_len, substr_start);
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          // printf("%s\n", tokens[nr_token].str);
          nr_token++;
          break;
        default:
          assert(0);
          // TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  printf("%d %d\n", ')', tokens[nr_token - 1].type);
  //  printf("%d %d/n", nr_token - 1, tokens[nr_token - 1].type);

  /*for (int j = 0; j < nr_token; j++)
  {
    printf("%d ", tokens[j].type);
    if (strlen(tokens[j].str) > 0)
      printf("%s", tokens[j].str);
    printf("\n");
  }*/

  return true;
}

static bool check_parenthess(int p, int q)
{
  if (tokens[p].type != '(' || tokens[q].type != ')')
    return false;
  else
  {
    int count = 1;
    p++;
    while (count > 0 && p < q)
    {
      if (tokens[p].type == '(')
        count++;
      else if (tokens[p].type == ')')
        count--;
      p++;
    }
    if (count == 1 && p == q)
      return true;
    else
      return false;
  }
}

static uint32_t eval(int p, int q)
{
  if (p > q)
  {
    printf("error\n");
    return 0;
  }
  else if (p == q)
  {
    // printf("case num\n");
    if (tokens[p].type == TK_NUM)
    {
      uint32_t res = 0;
      // printf("%s\n", tokens[p].str);
      sscanf(tokens[p].str, "%u", &res);
      return res;
    }
    else
    {
      printf("Error! for no TK_NUM\n");
      return 0;
    }
  }
  else if (check_parenthess(p, q) == true)
  {
    // printf("%d()%d!\n", p, q);
    return eval(p + 1, q - 1);
  }
  else
  {
    // printf("case normal\n");
    if (q == p + 1)
    {
      printf("Error! for only two TKs\n");
      return 0;
    }
    int op = 0;
    int op_type = 0;
    int cur_op = p;
    while (cur_op < q)
    {
      switch (tokens[cur_op].type)
      {
      case '(':
        int count = 1;
        while (count > 0 && cur_op < q)
        {
          cur_op++;
          if (tokens[cur_op].type == '(')
            count++;
          else if (tokens[cur_op].type == ')')
            count--;
        }
        break;
      case '+':
      case '-':
        op_type = tokens[cur_op].type;
        op = cur_op;
        break;
      case '*':
      case '/':
        if (op_type == '+' || op_type == '-')
          break;
        else
        {
          op_type = tokens[cur_op].type;
          op = cur_op;
          break;
        }
      default:
        break;
      }
      cur_op++;
    }
    if (op == 0)
    {
      printf("Error! for no op\n");
      return 0;
    }
    else
    {
      uint32_t val1 = eval(p, op - 1);
      uint32_t val2 = eval(op + 1, q);
      // printf("%u %c %u\n", val1, op_type, val2);
      switch (op_type)
      {
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/':
        if (val2 == 0)
        {
          printf("0 cannot be used as a divisor!\n");
          return 0;
        }
        return val1 / val2;
      default:
        assert(0);
      }
    }
  }
}

word_t expr(char *e, bool *success)
{
  // printf("%s\n", e);
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  // printf("%d\n", nr_token);
  // bool res = check_parenthess(0, nr_token - 1);
  // printf("%d\n", res);
  /* TODO: Insert codes to evaluate the expression. */
  printf("%u\n", eval(0, nr_token - 1));
  //  memset(tokens, 0, sizeof(tokens));
  return 0;
}
