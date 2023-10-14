#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char expr[200];
  uint32_t val;

  /* TODO: Add more members if necessary */

} WP;
WP *new_wp();
void free_wp(int NO);
void print_wp();
bool check_wp();
#endif
