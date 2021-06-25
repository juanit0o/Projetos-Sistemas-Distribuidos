#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"
#include "entry.h"

struct tree_t {
	/** a preencher pelo grupo */

  struct entry_t *node;
  struct tree_t *left;
  struct tree_t *right;
  //tree_t left
  //tree_t right

};


#endif
