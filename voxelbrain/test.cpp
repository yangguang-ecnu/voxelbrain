#include <stdio.h>
#include "undo.h"
#include "volio.h"
#include "v3sets.h"

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
   
   // testing v3sets
   
   // testing loading
   
   point_list body;
   point_list border;
   
   for(int i = 0; i <=40; i++)
	   for(int j = 0; j <=40; j++)
		   for(int k = 0; k <=40; k++){
			   border.insert(key(V3i(i,j,k)));
		   }
   
   update_border(border, body);
   printf("border_size %d; body_size %d\n", border.size(), body.size());
}
