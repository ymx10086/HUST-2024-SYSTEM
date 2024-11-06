#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(const char* e){
  if (free_ == NULL) {
    fprintf(stderr, "Error: No available watchpoint slots!\n");
    return NULL;
  }

  WP* res = free_;
  free_ = free_->next, res->next = head, head = res;

  size_t expr_len = strlen(e);
  if (expr_len >= sizeof(res->expr)) {
      fprintf(stderr, "Error: Expression is too long (max length: %zu)\n", sizeof(res->expr) - 1);
      return NULL;
  }

  strcpy(res->expr, e);

  bool success = true;
  res->newValue = res->oldValue = expr(res->expr, &success);

  if (!success) {
    fprintf(stderr, "Error: Invalid expression: '%s'\n", e);
    return NULL;
  }

  return res;
}

bool free_wp(int no) {
  WP* p = head;
  WP* pre = NULL;
  while (p != NULL) {
    if (p->NO == no) {
      if (pre == NULL) head = p->next;
      else pre->next = p->next;
      p->next = free_, free_ = p;

      return true; 
    }
    pre = p, p = p->next;
  }

  return false;
}

#include <stdio.h>

void watchpoint_display() {
  WP* p = head;
  if (p == NULL) {
    printf("No watchpoints to display.\n");
    return;
  }

  while (p != NULL) {
    printf("Watchpoint %d:\n", p->NO);
    printf("  Expression: %s\n", p->expr);
    printf("  Old Value: %u\n", p->oldValue);
    printf("  New Value: %u\n", p->newValue);

    if (p->valueChanged) {
        printf("  Value has changed!\n");
    } else {
        printf("  Value has not changed.\n");
    }
    printf("\n");

    p = p->next;
  }
}

bool check_wp() {
  WP* p = head;
  bool flag = false;

  while (p != NULL) {
    bool success = true;
    p->newValue = expr(p->expr, &success);
    
    if (!success) {
      printf("Error: Failed to evaluate expression for watchpoint %d: %s\n", p->NO, p->expr);
      p = p->next;
      continue;
    }

    if (p->newValue != p->oldValue) {
      flag = true;
      
      printf("Watchpoint %d: %s\n", p->NO, p->expr);
      printf("Old Value: %u\n", p->oldValue);
      printf("New Value: %u\n", p->newValue);

      p->oldValue = p->newValue;
    }
    p = p->next;
  }

  return flag;
}


