#include "volio.h"


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