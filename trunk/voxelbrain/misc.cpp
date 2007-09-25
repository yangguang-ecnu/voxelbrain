#include "misc.h"

using namespace std;

float smooth_bell(float x){
  if(x<0)x=-x;
  if(x>1)return 0.0f;
  float square_x = x*x;
  return 2*square_x*x-3*square_x+1; // 2*x^3-3*x^2+1, unit bell.
};

bool environment::parse(int argc, char ** argv){
	if(argc == 1){ // ok, just everything as usual
		input_header = "bSS081a_1_3.hdr";
		input_data = "bSS081a_1_3.img";
		output_header = "bSS081a_1_3.hdr";
		output_data = "bSS081a_1_3.img";
		return true;
	}else if (argc == 2) {
		string filename(argv[1]);
		string data(filename);
		string::size_type pos = filename.find(".hdr");
		if(string::npos == pos){
		   err = "Incorrect extension\n";
		   return false;
		}else{
			data.replace(pos, 4, ".img");
		}
		input_header = filename;
		input_data = data;
		output_header = filename;
		output_data = data;
		return true;
	}else{
		err = "Zero or one argument expected.\n";
		return false;		
	}
}
