#ifndef MISC_H_
#define MISC_H_

// functions that don't depend on anything and can be useful in many places

#include <string>

float smooth_bell(float x); // f(-1)=0; f(0)=1; f(1)=0.

struct environment {
   bool parse(int argc, char ** argv);
   std::string input_header;  // where to read data from
   std::string input_data; 
   std::string output_header; // where to write data to
   std::string output_data;
   std::string err;
};

#endif /*MISC_H_*/
