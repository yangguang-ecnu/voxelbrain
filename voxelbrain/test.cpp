#include <stdio.h>
#include "undo.h"
#include "volio.h"

int main(){
	//testing undo
	
   undo test;
   test.add_point(1);
   test.add_point(2);
   test.save();
   test.add_point(3);
   test.add_point(4);
   undo::undo_step a;
   test.restore(a);
   for(undo::undo_step::iterator i = a.begin(); i != a.end(); i++)
	   printf("restored %d\n", *i);
   printf("Done with tests\n");

   raw_volume rw;
   rw.read_nifti_file("bSS081a_1_3.hdr", "bSS081a_1_3.img");
   printf("\nCenter: %f\n", rw(120,128,80));
   
   // testing loading
}
