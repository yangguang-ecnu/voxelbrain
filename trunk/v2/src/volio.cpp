/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:

 Volume Input and Output.
*/
#include "nifti.h"
#include "volio.h"

using namespace mgh;

void raw_volume::load_mgh(char * name){
  read(name, m_data);
  parse(m_data, vol, true);
  
  data = new short int [vol.size];
  max = -1000.0;
  min = 1000.0;
  for(int i = 0; i < vol.size;i++){
    if(vol.data[i] > max)max = vol.data[i];
    if(vol.data[i] < min)min = vol.data[i];
    data[i] = vol.data[i];
  };

  dim[0] = vol.width;
  dim[1] = vol.height;
  dim[2] = vol.depth;
};

// parses the string taken from m_volume
void raw_volume::load_mgh_data(){
  parse(m_data, vol, true);
  
  data = new short int [vol.size];
  max = -1000.0;
  min = 1000.0;
  for(int i = 0; i < vol.size;i++){
    if(vol.data[i] > max)max = vol.data[i];
    if(vol.data[i] < min)min = vol.data[i];
    data[i] = vol.data[i];
  };

  dim[0] = vol.width;
  dim[1] = vol.height;
  dim[2] = vol.depth;
  printf("Volume successfully loaded: (%d:%d:%d)\n", 
	 dim[0], dim[1], dim[2]);
};

void raw_volume::save_mgh(char * name){
  for(int i = 0; i < vol.size;i++){
    vol.data[i] = data[i];
   // if(vol.data[i] != 0)printf("(%d:%d ) ",i,  data[i]);
  };
  try {
  parse(m_data, vol, false);
  } catch(char * in){
	printf("Unable to parse the file: %s\n", in);
	throw "Parsing failed when writing.";
  };
  write(name, m_data);
 };

void raw_volume::load(const string file_name){
	int size = dim[0]*dim[1]*dim[2]; 

	FILE * in = fopen(file_name.c_str(), "rb");
    if(0 == in){
      printf("File failed to open.\n"); exit(1);
    };
    
    data = new short int [size];
    //read volume
    if(size != fread(data, sizeof(short int), size, in)){
      printf("Cannot read file properly.\n"); exit(1);
   };
   
   fclose(in);
}; 

void raw_volume::save(const string file_name){
	int size = dim[0]*dim[1]*dim[2]; 
	FILE * out = fopen(file_name.c_str(), "wb");
    
	for(int i = 0; i < size; i++)if(data[i]<0)data[i]=0;

	
	if(0 == out){
      printf("File failed to open.\n"); exit(1);
    };
    
    if(size != fwrite(data, sizeof(unsigned short int), size, out)){
      printf("Cannot write file properly.\n"); delete[] data ; exit(1);
   };
};
