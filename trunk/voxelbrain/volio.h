#ifndef __volume_io__
#define __volume_io__

#include <string>
#include <stdlib.h>
#include "v3.h"

using namespace std;

struct raw_volume{
	int read_nifti_file(char * hdr_file, char * data_file);
	int write_nifti_file(char * hdr_file, char * data_file);
	void load(const string file_name);
	void save(const string file_name);
	int dim[3];
	
	typedef short int MY_DATA;
	MY_DATA * data;

	float operator ()(int x, int y, int z){
	  return ((float)data[x+dim[0]*y+dim[1]*dim[0]*z]);
	};

	float operator ()(float x, float y, float z){
	  return ((float)data[(int)x+dim[0]*(int)y+dim[1]*dim[0]*(int)z]);
	};

	float operator ()(const V3i & wh){
	  return ((float)data[wh.x+dim[0]*wh.y+dim[1]*dim[0]*wh.z]);
	};

	float get(int x, int y, int z){
	  return ((float)data[x+dim[0]*y+dim[1]*dim[0]*z]);
	};

	void set(int x, int y, int z, short int in){
		data[x+dim[0]*y+dim[1]*dim[0]*z] = in;
	};

	void set(const V3i & wh, short int in){
		data[wh.x+dim[0]*wh.y+dim[1]*dim[0]*wh.z] = in;
	};

	raw_volume(){data = NULL;};
	~raw_volume(){if(data != NULL)delete [] data;};
};



#endif // __volume_io__
