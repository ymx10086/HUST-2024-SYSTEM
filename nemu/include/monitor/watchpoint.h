#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[65536];             // watchpoint expression
  bool valueChanged;            // if the value of the watchpoint has changed
  uint32_t oldValue, newValue;   // the value of the watchpoint


} WP;

/* TODO: Add member functions if necessary */

WP* new_wp(const char* e);
bool free_wp(int no);
void watchpoint_display();
bool check_wp();

#endif
