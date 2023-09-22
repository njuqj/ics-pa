#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp()
{
  if (free_ == NULL)
  {
    printf("There are too many watchpoints!\n");
    return NULL;
  }
  WP *p = free_;
  free_ = free_->next;
  if (head == NULL)
  {
    head = p;
    p->next = NULL;
  }
  else if (head->NO > p->NO)
  {
    p->next = head;
    head = p;
  }
  else
  {
    WP *wp = head;
    while (wp->next != NULL && wp->next->NO < p->NO)
      wp = wp->next;
    p->next = wp->next;
    wp->next = p;
  }
  return p;
}