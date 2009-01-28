Code is hosted here:
http://code.google.com/p/voxelbrain/

Serious coding problems lead to the changes in the 
development process. Now the process is organized as follows:
 1. Write some small unit of functionality in a GTEST
 2. Factor out anything useful into a file.
 Repeat until done.

So, the architecture is minimal:

  - Scene viewer, accepts scenes which are Drawable;
     The viewer provides navigation and pointing facilities.

Todo:
 Provide facilities for keyboard feedback.
 Integrate propagation into the workflow.
  
