#include <stdio.h>

void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[])
{
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  FILE *fp = fopen("/home/miracle/ics2020/nemu/tools/gen-expr/input", "r");
  if (fp == NULL)
    printf("无法打开文件。\n");
  else
  {
    char str[60000];
    while (!feof(fp))
    {
      fgets(str, 60000, fp);
      printf("%s\n", str);
    }
    fclose(fp);
  }

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
