# Introduction #

  * source files
  * libs
  * includes

Source files are written by me, totally and completely understood, etc.
Libraries - reusable stuff, linked or copied there from respective dependencies.
Includes - respective .h files, needed to support libs; own includes are placed in root.

Once something becomes big, it becomes a separate library... sort of.

# Code organisation #

For the purpose of this discussion, let us consider code as a sequence of elements and several classifications on them. For example, an element is a language statement, which, in turn, can be included in a function, class, file, etc.

The elements can be accessed through interfaces.  The idea is to make the current element set as small as possible, or at least provide separation between sets of elements which can be reasoned about independently.

Problems of information organisation:

  * different ontologies.
    * header - interface of a module
    * potential candidate for includes, once it matures.
  * class names
    * finer grained usage abstraction (Buzz-Buzz-Buzz)
  * variable names
    * information on intended behaviour
    * refactoring
  * file names
    * compilation
    * separation into library
