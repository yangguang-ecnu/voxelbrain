/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#include "mgz_io.h"

using namespace std;
using namespace mgh;

//volume operations

//3 coords to linear subscript in an array
int mgh::subscript(const volume & in, int x, int y, int z){
  return (z % in.depth) * in.width * in.height + (y % in.height) * in.width + (x % in.width);
};

//allocate volume
int mgh::create(volume & vol, int x, int y, int z){
  vol.data = new int[x*y*z];
  vol.size = x*y*z; 
  vol.width = x; vol.height = y; vol.depth = z;

  return 0;
};

//retrieve a voxel value
int mgh::get(const volume & in, int x, int y, int z){ 
  return in.data[subscript(in, x,y,z)];
}; 

//set a voxel value
void mgh::set(volume & in, int x, int y, int z, int val){
  in.data[subscript(in, x,y,z)] = val;
};

//read from file
void mgh::read(string fname, raw & data){
  const int BUF_SIZE = 100000;
  char buf[BUF_SIZE+1]; 
  FILE * src = fopen(fname.c_str(), "rb");
  if(src == NULL)throw "Failed to open file"; 
  data = ""; int bytes_read = 0;
  do{
    bytes_read = fread((void *)buf, 1, BUF_SIZE, src);
    data += string(buf, bytes_read);
  }while(bytes_read == BUF_SIZE);
  fclose(src);
};

void mgh::write(string fname, const raw & data){
  FILE * src = fopen(fname.c_str(), "wb");
  if(src == NULL)throw "Failed to open file";
  int bytes_written = fwrite(data.c_str(), 1, data.size(), src);
  fclose(src);
  if(bytes_written != data.size())throw "Could not write all the data";
};

//raw memory copy from a string
void memcpy_reverse(const raw & data, char * output, int size, int &pos){
	if(data.size() < pos+size)throw "Not enough bytes to read;";
	for(int i = 0; i < size; i++){output[size-i-1]=data[pos+i];};
	pos+=size;
};

//writing the results back
void memcpy_reverse_w(raw & data, const char * input, int size, int &pos){
	if(data.size() < pos+size)throw "Not enough bytes to write;";
	for(int i = 0; i < size; i++){data[pos+i] = input[size-i-1];};
	pos+=size;
};

// raw operations:
int mgh::get_int(const raw & data, int & pos){
  int res; memcpy_reverse(data, (char *)&res, sizeof(int), pos);
  return res;
};

short mgh::get_short(const raw & data, int & pos){
  short res; memcpy_reverse(data, (char *)&res, sizeof(short), pos);
  return res;
};

float mgh::get_float(const raw & data, int & pos){
  float res; memcpy_reverse(data, (char *)&res, sizeof(float), pos);
  return res;
};

char mgh::get_char(const raw & data, int & pos){
  char res = data[pos]; pos++;
  return res;
};

  void mgh::set_int(raw & data, int in, int & pos){int tmp = in; 
  	memcpy_reverse_w(data, (char *)&tmp, sizeof(int), pos);};
  void mgh::set_short(raw & data, short in, int & pos){short tmp = in; 
  	memcpy_reverse_w(data, (char *)&tmp, sizeof(short), pos);};
  void mgh::set_float(raw & data, float in, int & pos){float tmp = in; 
  	memcpy_reverse_w(data, (char *)&tmp, sizeof(float), pos);};
  void mgh::set_char(raw & data, char inc, int & pos){data[pos]=inc; pos++;};


void mgh::parse(raw & data, volume & result, bool read){

  int pos = 0; //current read position

  int   start_frame, end_frame, width, height, depth, nframes, type, x, y, z,
    bpv, dof, bytes, version, ival, unused_space_size, good_ras_flag, i ;
  char   unused_buf[UNUSED_SPACE_SIZE+1] ;
  float  fval, xsize, ysize, zsize, x_r, x_a, x_s, y_r, y_a, y_s,
    z_r, z_a, z_s, c_r, c_a, c_s ;
  short  sval ;
  //  int tag_data_size;
  char *ext;
  char command[STRLEN];
  int nread;
  int tag;

  xsize = ysize = zsize = 0;
  x_r = x_a = x_s = 0;
  y_r = y_a = y_s = 0;
  z_r = z_a = z_s = 0;
  c_r = c_a = c_s = 0;

  printf("reading basic info\n");
  nread = get_int(data, pos);
  if (!nread)throw "Bad version code(?)";

  width = get_int(data, pos) ;
  height = get_int(data, pos) ;
  depth =  get_int(data, pos) ;
  nframes = get_int(data, pos) ;
  type = get_int(data, pos) ;
  dof = get_int(data, pos) ;

  if(read){ //if we are reading, then create
  printf("trying to create...\n");
  create(result, width, height, depth);
  printf("created %d\n;", result.size);
  };
  
  unused_space_size = UNUSED_SPACE_SIZE-sizeof(short) ;

  good_ras_flag = get_short(data, pos) ;
  if (good_ras_flag > 0){     /* has RAS and voxel size info */
    unused_space_size -= USED_SPACE_SIZE ;
    xsize = get_float(data, pos) ;
    ysize = get_float(data, pos) ;
    zsize = get_float(data, pos) ;

    x_r = get_float(data, pos) ; x_a = get_float(data, pos) ; x_s = get_float(data, pos) ;
    y_r = get_float(data, pos) ; y_a = get_float(data, pos) ; y_s = get_float(data, pos) ;

    z_r = get_float(data, pos) ; z_a = get_float(data, pos) ; z_s = get_float(data, pos) ;
    c_r = get_float(data, pos) ; c_a = get_float(data, pos) ; c_s = get_float(data, pos) ;
  }
  /* so stuff can be added to the header in the future */
  ///fread(unused_buf, sizeof(char), unused_space_size, fp) ;
  
  pos += unused_space_size;
  
  if(!read){ //do some sanity checks before writing
	if(depth != result.depth || width != result.width || height != result.height) throw "Dimensions are mismatched";  	
  };
    
//  for(int j = 0; j < result.depth*result.height*result.width; j++)
//	  		if(result.data[j] != 0)printf("<%d:%d>", j, result.data[j]);
 
    
  for (z = 0 ; z < depth ; z++){
    for (i = y = 0 ; y < height ; y++)
      {
	for (x = 0 ; x < width ; x++, i++)
	  {
	  	if(read){
	    switch (type)
	      {
		      case MRI_INT: set(result, x, y, z, get_int(data, pos)); break;
		      case MRI_SHORT: set(result, x, y, z, get_short(data, pos)); break;
		      case MRI_UCHAR: set(result, x, y, z, get_char(data, pos)); break;
		      case MRI_FLOAT: set(result, x, y, z, (int)get_float(data, pos)); break;
		      case MRI_TENSOR: throw "Unable to read tensors";
	      }
	  	}else{ //write
	  		//if(get(result, x, y, z) != 0)printf("(%d,%d,%d,%d) ", x, y, z, get(result, x, y, z));
	    switch (type)
	      {
		      case MRI_INT: set_int(data, get(result, x, y, z), pos); break;
		      case MRI_SHORT: set_short(data, get(result, x, y, z), pos); break;
		      case MRI_UCHAR: set_char(data, get(result, x, y, z), pos); break;
		      case MRI_FLOAT: set_float(data, get(result, x, y, z), pos); break;
		      case MRI_TENSOR: throw "Unable to read tensors";
	      };
	  	};
	  }
      };
  };
};
  







