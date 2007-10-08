#include "glbase.h"
#include "stdio.h"
void Xs::draw(){
  if(!hidden){
    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    float d = size/2.0f;
    if(false){
      glBegin(GL_QUADS);
      glDisable(GL_LIGHTING);
      glColor4f(1.0,0.0,0.0,0.5);
      plane(1,2,0,d); // x-y
      glColor4f(0.0,1.0,0.0,0.5);
      plane(1,0,2,d); // x-z
      glColor4f(0.0,0.0,1.0,0.5);
      plane(0,1,2,d); // y-z
      glEnd();
    }else{
      glBegin(GL_LINE_STRIP);
      glColor4f(1.0,0.0,0.0,0.5);
      plane(1,2,0,d, true); // x-y
      glEnd();
      glBegin(GL_LINE_STRIP);
      glColor4f(0.0,1.0,0.0,0.5);
      plane(1,0,2,d, true); // x-y
      glEnd();
      glBegin(GL_LINE_STRIP);
      glColor4f(0.0,0.0,1.0,0.5);
      plane(0,1,2,d,true); // x-y
      glEnd();
    };
	glBegin(GL_LINES);
		glColor4f(0.0,0.0,0.0,0.5);
		glVertex3f(pos.x-d, pos.y, pos.z); glVertex3f(pos.x+d, pos.y, pos.z);
		glVertex3f(pos.x, pos.y-d, pos.z); glVertex3f(pos.x, pos.y+d, pos.z);
		glVertex3f(pos.x, pos.y, pos.z-d); glVertex3f(pos.x, pos.y, pos.z+d);
	glEnd();
  };
};

void Xs::plane(int first, int second, int third, float d, bool closed ){
  float corners[4][3] = {{0.0, -1.0,-1.0},{0.0,-1.0,1.0},{0.0, 1.0, 1.0},{0.0, 1.0,-1.0}};
  for(int i = 0; i < 4; i++){
    glVertex3f(pos.x+d*corners[i][first], pos.y + d*corners[i][second], pos.z + d*corners[i][third]);
  };
  if(closed)glVertex3f(pos.x+d*corners[0][first], pos.y + d*corners[0][second], pos.z + d*corners[0][third]);
};

void sphere_param(const V3f & center , int i, int j, float diameter, int precision){
	float scale = diameter/2.0; 
	float dx = 3.14159 / precision;
	float dy = 3.14159 / precision;
	V3f cur(sin(dx*i)*sin(dy*j), cos(dx*i)*sin(dy*j), cos(dy*j));
	glNormal3f(cur.x, cur.y, cur.z);
	glVertex3f(scale*cur.x+center.x, scale*cur.y+center.y, scale*cur.z+center.z);
}

void gen_sphere(const V3f & in, float diameter, int precision){
	for(int i = 0; i < (precision*2); i++)
		for(int j = 0; j < precision; j++){
			sphere_param(in, i, j, diameter, precision);
			sphere_param(in, i, j+1, diameter, precision);
			sphere_param(in, i+1, j+1, diameter, precision);
			sphere_param(in, i+1, j, diameter, precision);
		}
};


