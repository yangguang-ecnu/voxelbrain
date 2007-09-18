#generate revision info to include in the code.
echo \#define SVN_REVISION \"`svn info | grep Revision`\" > revision.h