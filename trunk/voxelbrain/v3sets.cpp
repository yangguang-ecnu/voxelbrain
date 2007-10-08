#include "v3sets.h"
#include <vector>

//scan nearby area for emptiness
inline bool is_border(const V3i & what, const point_list & border, const point_list & body)
{
   	for(int i = -1; i <= 1; i++)
   	   	for(int j = -1; j <= 1; j++)
   	    	for(int k = -1; k <= 1; k++){
   	    		if((body.find(key(what+V3i(i,j,k)))==body.end())
   	    	    && (border.find(key(what+V3i(i,j,k)))==border.end()))
   	    			return true;
   	    	}
   	return false;
}

//check if border is still border
void update_border(point_list & border, const point_list & all) //makes sure that the border is really border;
{
	std::vector<unsigned int> to_move; 
	for(point_list::iterator i = border.begin(); i!=border.end(); i++){
		if(!is_border(key(*i), border, all))to_move.push_back(*i);//move it to body then...
	}
	
	//actually move them
	for(std::vector<unsigned int>::iterator i = to_move.begin(); i!= to_move.end(); i++){
		border.erase(*i);
		//body.insert(*i);
	};

	//done
}