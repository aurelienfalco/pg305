#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "list.h"

#define MIN_INTER 100
#define MAX_INTER 1000
#define MAX_CHARS 100

enum tag {ASK, END, INTER, FINISH, NOTHING};

struct task_list{
  struct list_head children;
  unsigned int num_children;
};


struct task{
  struct list_node list;
  int nb_test;
  char* start_word;
};


#endif
