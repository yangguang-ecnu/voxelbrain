#ifndef V3SETS_H_
#define V3SETS_H_

// deals with sets of vaues; either using google sparsehash, or something else.
///TODO:setup
#define GOOGLEHASH 1


#include "v3.h"

inline unsigned int key(const V3i & v){return v.x+(v.y<<10)+(v.z<<20); };
inline const V3i key(unsigned int in){return V3i(in & 0x3ff,(in>>10) & 0x3ff,(in>>20) & 0x3ff); };

///TODO: change to const int!!!
#define impossibleKeyA (1<<31) 
#define impossibleKeyB ((1<<31)+1) 


struct Point{
	V3i pos;
	V3f norm;
	V3f col;
};

#ifdef GOOGLEHASH //if we have GOOGLEHASH
#include <google/sparse_hash_map>
#include <google/dense_hash_map>
#include <google/sparse_hash_set>
#include <google/dense_hash_set>
class PointMapBase: public google::dense_hash_map<size_t, Point>{
public:
	PointMapBase(){
		set_empty_key(impossibleKeyA);
		set_deleted_key(impossibleKeyB);
	};
};
class PointSetBase: public google::dense_hash_set<size_t> {
public:
	PointSetBase(){
		set_empty_key(impossibleKeyA);
		set_deleted_key(impossibleKeyB);
	}
};
#else //vanilla hash map 
#include <hash_map>
#include <hash_set>
using namespace stdext;
class PointMapBase: public hash_map<unsigned int, Point> {};
class PointMapBase: public hash_set<unsigned int> {};
#endif

///backward compatibility
///TODO: remove

typedef PointMapBase point_space;
typedef PointSetBase point_list;

void update_border(point_list & border, const point_list & all); //makes sure that the border is really border;

#endif /*V3SETS_H_*/
