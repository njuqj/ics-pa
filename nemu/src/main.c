#include <stdio.h>
#include <stdint.h>

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
    char str[60030];
    char e[60000];
    uint32_t res = 0;
    while (!feof(fp))
    {
      fgets(str, 60030, fp);
      sscanf(str, "%u %s", &res, e);
      printf("%u %s\n", res, e);
    }
    fclose(fp);
  }

  /* Start engine. */
  // engine_start();

  return is_exit_status_bad();
}
