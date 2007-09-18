#ifndef UNDO_H_
#define UNDO_H_

#include "v3sets.h"
#include <vector>
using namespace std;

struct undo {
	
	undo(){cur_step = new undo_step; clear(); };
	
   typedef std::vector<unsigned int> undo_step;
   typedef std::vector<undo_step *> undo_buffer;
   undo_buffer the_undo;
   undo_step * cur_step;

   void clear(); //reset the undo stack
   void add_point(const V3i &); //add a point to the current step
   void save(); //save current step in the stack
   void restore(undo_step &); //pop the step from stack.
   bool empty();
};

#endif /*UNDO_H_*/
