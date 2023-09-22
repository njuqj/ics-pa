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

void free_wp(int NO)
{
  if (head == NULL)
  {
    printf("There is no watchpoint!\n");
    return;
  }
  WP *p = NULL;
  if (head->NO == NO)
  {
    p = head;
    // printf("%d %d %s\n", p->NO, p->val, p->expr);
    head = head->next;
  }
  else
  {
    WP *wp = head;
    while (wp->next != NULL)
    {
      if (wp->next->NO == NO)
      {
        p = wp->next;
        wp->next = wp->next->next;
      }
      wp = wp->next;
    }
  }
  if (p == NULL)
    printf("no such watchpoint!\n");
  else
  {
    if (free_ == NULL)
    {
      free_ = p;
      p->next = NULL;
    }
    else if (free_->NO > p->NO)
    {
      p->next = free_;
      free_ = p;
    }
    else
    {
      WP *wp = free_;
      while (wp->next != NULL && wp->next->NO < p->NO)
        wp = wp->next;
      p->next = wp->next;
      wp->next = p;
    }
    printf("%d watchpoint deleted succeesfully!\n", NO);
  }
  return;
}

void print_wp()
{
  WP *p = head;
  printf("head:\n");
  while (p != NULL)
  {
    printf("%d %d %s\n", p->NO, p->val, p->expr);
    p = p->next;
  }
  p = free_;
  printf("free:\n");
  while (p != NULL)
  {
    printf("%d ", p->NO);
    p = p->next;
  }
  printf("\n");
  return;
}