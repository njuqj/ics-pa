#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <memory/paddr.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct
{
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Let the program step through N instructions and then pause execution, when N is not given, the default is 1 ", cmd_si},
    {"info", "info r: Print the register status\n       info w: Print the watchpoint information", cmd_info},
    {"x", "Scan memory", cmd_x},
    {"p", "expression evaluation", cmd_p},
    {"w", "Pauses program execution when the value of the expression EXPR changes", cmd_w},
    {"d", "Delete the watchpoint", cmd_d}

    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args)
{
  if (args == NULL)
  {
    cpu_exec(1);
  }
  else
  {
    uint64_t n;
    sscanf(args, "%ld", &n);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *args)
{
  switch (args[0])
  {
  case 'r':
    printf("EAX: 0x%08x\n", cpu.eax);
    printf("ECX: 0x%08x\n", cpu.ecx);
    printf("EDX: 0x%08x\n", cpu.edx);
    printf("EBX: 0x%08x\n", cpu.ebx);
    printf("ESP: 0x%08x\n", cpu.esp);
    printf("EBP: 0x%08x\n", cpu.ebp);
    printf("ESI: 0x%08x\n", cpu.esi);
    printf("EDI: 0x%08x\n", cpu.edi);
    break;
  case 'w':
    printf("watchpoint\n"); // 待修改
    break;

  default:
    printf("invalid input!\n");
    break;
  }
  return 0;
}

static int cmd_x(char *args)
{
  uint32_t length;
  uint32_t addr;
  // printf("%s\n", args);
  if (sscanf(args, "%u %x", &length, &addr) != 2) // 如果输入不合法或者只有一个输入，输出提示信息然后返回
  {
    printf("invalid input!\n");
    return 0;
  }
  uint8_t *c = guest_to_host(addr);
  printf("0x%x: ", addr);
  for (int i = 0; i < length; i++)
  {
    printf("%02x", *(c + i));
    if (!((i + 1) % 4))
    {
      printf("\n0x%x: ", addr + i + 1);
    }
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args)
{
  bool success = true;
  expr(args, &success);
  if (!success)
    printf("Error for expr\n");
  return 0;
}

static int cmd_w(char *args)
{
  printf("The value of the current expression is: ");
  bool success = true;
  uint32_t val = expr(args, &success);
  if (success)
  {
    WP *wp = new_wp();
    wp->val = val;
    strcpy(wp->expr, args);
    printf("Watchpoint added successfully!\n");
  }
  else
  {
    printf("Failed to add watchpoint!\n");
  }
  // print_wp();
  return 0;
}

static int cmd_d(char *args)
{
  int NO = 0;
  if (!sscanf(args, "%d", &NO))
    printf("input error!\n");
  free_wp(NO);
  print_wp();
  return 0;
}

void ui_mainloop()
{
  if (is_batch_mode())
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}
