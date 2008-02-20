/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:

  Common interface for saving/loading MRI data in different formats.

*/

#ifndef __volume_io__
#define __volume_io__

#include <string>
#include <stdlib.h>
#include "v3.h"
#include "nifti.h"
#include "mgz_io.h"

using namespace mgh;


using namespace std;


struct raw_volume{

  nifti_1_header hdr;

  raw m_data;
  volume vol;  
	
  int read_nifti_file(const char * hdr_file, const char * data_file);
  int write_nifti_file(const char * hdr_file, const char * data_file);

  void load_mgh(char * name);
  void load_mgh_data();
  void save_mgh(char * name);
  
  void load(const string file_name);
  void save(const string file_name);
  int dim[3];
	
  int min; int max; 
	
  typedef short int MY_DATA;
  MY_DATA * data;

  inline int poz(int x, int y, int z) const {
    return (x%dim[0])+dim[0]*(y%dim[1])+dim[1]*dim[0]*(z%dim[2]); };
  
  float operator ()(int x, int y, int z) const {
    return ((float)data[poz(x,y,z)]);
  };

  float operator ()(float x, float y, float z) const {
    return ((float)data[poz((int)x,(int)y,(int)z)]);
  };

  float operator ()(const V3i & wh) const {
    return (float)data[poz(wh.x, wh.y, wh.z)];
  };

  float get(int x, int y, int z) const{
    return ((float)data[poz(x,y,z)]);
  };

  void set(int x, int y, int z, short int in){
    data[poz(x,y,z)] = in;
  };

  void set(const V3i & wh, short int in){
    data[poz(wh.x, wh.y, wh.z)] = in;
  };

  raw_volume(){data = NULL;};
  ~raw_volume(){if(data != NULL)delete [] data;};
};



#endif // __volume_io__
