/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#ifndef __MGZ_IO_H__
#define __MGZ_IO_H__

#include <string>
#include <vector>

using namespace std;

#define MRI_UCHAR   0
#define MRI_INT     1
#define MRI_LONG    2
#define MRI_FLOAT   3
#define MRI_SHORT   4
#define MRI_BITMAP  5
#define MRI_TENSOR  6

#define UNUSED_SPACE_SIZE 256
#define USED_SPACE_SIZE   (3*sizeof(float)+4*3*sizeof(float))
#define STRLEN 256

//data;
namespace mgh {
  //object: raw data;
  typedef string raw; 

  //object: volume;
  struct volume{
  	int starting_pos; //where it was taken from.
  	int data_type;    //what kind of values were stored
  	
    int width;
    int depth;
    int height;
    
    int * data; int size;  
  };
  

  // messages:  
  int subscript(const volume &, int x, int y, int z);
  int create(volume &, int x, int y, int z); //constructor
  int get(const volume &, int x, int y, int z); 
  void set(volume &, int x, int y, int z, int val);

  int get_int(const raw &, int & pos);
  short get_short(const raw &, int & pos);
  char get_char(const raw &, int & pos);
  float get_float(const raw &, int & pos);
  
  void set_int(raw &, int , int & pos);
  void set_short(raw &, short, int & pos);
  void set_char(raw &, char, int & pos);
  void set_float(raw &, float, int & pos);
  
  void read(string, raw &);                  //main logic
  void write(string, const raw &);
  void parse(raw & , volume &, bool read);        //extract volume info
};
#endif //__MGZ_IO_H__






