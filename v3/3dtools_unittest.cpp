#include <gtest/gtest.h>

#include "3dtools.h"
#include "fastvolume.h"
#include "loader.h"
#include <GL/glfw.h>

// Test loading of volume
TEST(Mgz, Interface){
  FastVolume volume;
  MgzLoader mri(volume);
  MgzLoader mri_bad(volume);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));
  EXPECT_FALSE(mri_bad.Load("brainmask.mgz.unexistent"));

  int random_voxel = (++volume.vol[5]); //alter a voxel

  EXPECT_TRUE(mri.Save("brainmask.mgz"));
  EXPECT_TRUE(mri.Load("brainmask.mgz"));
  
  EXPECT_EQ(random_voxel, volume.vol[5]);
}; 

TEST(OGL, Quad){
  struct : public Drawable{
    void Draw(){
      glBegin(GL_QUADS);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(100,100,100); //random quad
      glVertex3f(100,-100,100);
      glVertex3f(-100,-100,100);
      glVertex3f(-100, 100,100);
      glEnd();

    };
  } scene;
  runScene(scene);
};

void RandomFunction(V3f, float *);

TEST(OGL, Sphere){
  struct: public Drawable{
    void Draw(){
      DrawSphere( V3f(0,0,0), 98.0f, 40);
    };
  } scene; 
  runScene(scene);
};


/*
  Test navigator operation.
 */
TEST(OGL, Navigator){
  struct: public Drawable{
    void Draw(){
      V3f c(Center());
      c /= 2;

      DrawSphere( c, 3.0f, 40);
    };
  } scene; 
  runScene(scene);
};

//Repersent relations between 3 values in color.
void Colorize(V3f & inp){
  float m = min(inp);
  inp -= V3f(m,m,m); inp /= inp.length();
};

/*
  For each point 3 consequitive samples are taken and their relation is plotted
  on the surface. Provides efficient way to detect problems.
 */
TEST(OGL, SurfaceInfoBars){
  
  struct: public Drawable{
    Surface surf;
    FastVolume vol;
    void Draw(){

      int x, y;
      glfwGetMousePos(&x, &y);

      DrawSurface( surf );

      glDisable(GL_LIGHTING);

      glPushMatrix();
      glTranslatef(-128,-128,-128);
      glBegin(GL_LINES);
      //Draw normals
      glColor3f(1,0,0);
      for(int i = 0; i < surf.v.size(); i++){
	V3f c;
	AnalyzePoint(surf.v[i]+surf.n[i]*sin(0.01*x)*sin(0.01*x)*1.5, surf.n[i], vol, c);
	surf.c[i] = c;
      };
      glEnd();
      glPopMatrix();

      DrawPlane(V3f(0,0,0), V3f(20,0,0), V3f(0,20,0), 5);
      

    };
  } scene; 

  //Load the triangle data.
  EXPECT_TRUE(read_surface_binary(scene.surf, "lh.pial"));
  //Load volume.
  MgzLoader mri(scene.vol);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  AnalyzeSurface(scene.surf, scene.vol);
  
  runScene(scene);  

};


TEST(OGL, Surface){
  
  struct: public Drawable{
    Surface surf;
    FastVolume vol;
    void Draw(){

      DrawSurface( surf );

      glDisable(GL_LIGHTING);


    };
  } scene; 

  EXPECT_TRUE(read_surface_binary(scene.surf, "lh.pial"));
  MgzLoader mri(scene.vol);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  AnalyzeSurface(scene.surf, scene.vol);

  runScene(scene);  

};


TEST(OGL, TexturedQuad){
  struct TexturedScene: public Drawable, public Textured {
    void Draw(){
      V3f off(10*sin(0.1*frame_no_), 10*cos(0.13*frame_no_), 0);
      V3f a(20.0f,20.0f,5.0f); //random quad
      V3f b(20.0f,-20.0f,-5.0f);
      V3f c(-20.0f,-20.0f,-5.0f);
      V3f d(-20.0f, 20.0f,5.0f);
      a+=off; b+=off; c+=off; d+=off;
    
      Range cur(c,a); ExpandRange(cur,b); ExpandRange(cur, d);
      CheckTexture(cur);
        
      glBegin(GL_QUADS);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(SetTexture(a)); 
      glVertex3f(SetTexture(b)); 
      glVertex3f(SetTexture(c)); 
      glVertex3f(SetTexture(d)); 
      glEnd();

    };
  } scene; 
  runScene(scene);
};

TEST(OGL, TexturedSphere){
  struct: public Drawable, public Textured{
    void Draw(){
      glDisable(GL_BLEND);
      glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      DrawSphere(V3f(80*sin(0.05*frame_no_), 
		     80*sin(0.07*frame_no_+34), 
		     80*sin(0.03*frame_no_+65)+0.1), 
		 30.0f, 60,  
		 this);
    };
  } scene;
  runScene(scene);
};

/*
  Mouse navigation - intersection with a sphere.
*/
TEST(OGL, Navigation){
  struct: public Drawable{
    void Draw(){
      Ray in( mousePosition() ); //draw point ray
      glPointSize(5);
      glBegin(GL_POINTS);
      glColor3f(1,0,1);
      for(double f = 0.0; f < 1.0; f+=0.02){
	glVertex3f(in.O+in.D*f);
      };
      glEnd();

      V3f pnt(0,0,0);
      V3f hit_v;
      float radius = 20;
      Intersection hit_point;

      //      printf("from %f %f %f - into %f %f %f\n", in.O.x, in.O.y, in.O.z, in.D.x, in.D.y, in.D.z);
      if(IntersectRaySphere(in, pnt, radius, hit_point).hit){
      }else{
	IntersectRayPlane(in, Ray(V3f(0,0,0), V3f(0,0,-1)), hit_point);
      };
	DrawSphere(in.Travel(hit_point.distance, hit_v), 10, 10);
      
      
      DrawSphere(pnt, radius, 30);
    };
  } scene;
  runScene(scene);
};

TEST(OGL, SurfaceNavigation){
  
  struct: public Drawable{
    Surface surf;
    FastVolume vol;
    Ray in;
    void Draw(){

      int x, y;
      glfwGetMousePos(&x, &y);

      DrawSurface( surf );

      glDisable(GL_LIGHTING);

      glPushMatrix();
      glTranslatef(-128,-128,-128);

      Intersection hit;
      
      if(glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS)in = mousePosition();

      glPointSize(5);
      glBegin(GL_POINTS);
      glColor3f(1,0,1);
      for(double f = 0.0; f < 1.0; f+=0.02){
	glVertex3f(in.O+in.D*f);
      };


      float min_depth = 1000000;
      in.D /= in.D.length();

      for(int i = 0; i < surf.v.size(); i++){
	//	DrawSphere(surf.v[i], 1.0, 3);
	//glVertex3f(surf.v[i]);
	IntersectRaySphere(in, surf.v[i], 2, hit);
	if(hit.hit && (hit.distance < min_depth)) min_depth = hit.distance;
      };
      glEnd();

      DrawSphere(in.Travel(min_depth+10), 15.0, 10);

      glBegin(GL_LINES);
      //Draw normals
      glColor3f(1,0,0);
      for(int i = 0; i < surf.v.size(); i++){
	V3f c;
	AnalyzePoint(surf.v[i]+surf.n[i]*sin(0.01*x)*sin(0.01*x)*1.5, surf.n[i], vol, c);
	surf.c[i] = c;
      };
      glEnd();
      glPopMatrix();
      

    };
  } scene; 

  //Load the triangle data.
  EXPECT_TRUE(read_surface_binary(scene.surf, "lh.pial"));
  //Load volume.
  MgzLoader mri(scene.vol);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  AnalyzeSurface(scene.surf, scene.vol);
  
  runScene(scene);  

  
};


TEST(OGL, TexturedFastVolumeSphere){
  struct: public Drawable, public Textured{
    void Draw(){
      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      DrawSphere(V3f(80*sin(0.05*frame_no_), 
		     80*sin(0.07*frame_no_+34), 
		     80*sin(0.03*frame_no_+65)), 
		 30.0f, 5,  
		 this);
    };
  } scene;

  FastVolume volume;
  MgzLoader mri(volume);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  scene.texturing_fastvolume = & volume;  

  runScene(scene);
};


TEST(OGL, QuadBlending){
  struct : public Drawable{
    void Draw(){
      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin(GL_QUADS);
      glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
      glVertex3f(100,100,100); //random quad
      glVertex3f(100,-100,100);
      glVertex3f(-100,-100,100);
      glVertex3f(-100, 100,100);
      glEnd();

    };
  } scene;
  runScene(scene);
};

/*
  //Now every scene got a camera.
*/
TEST(OGL, Camera){

  struct: public Drawable{
    void Draw(){
      DrawSphere( V3f(0,0,0), 30.0f);

      glBegin(GL_LINES); // Crosshair.
      glColor3f(1.0f, 0.0f, 0.0f); // X
      glVertex3f(0.0f,0.0f,0.0f); glVertex3f(100.0f,0.0f,0.0f);
      glColor3f(0.0f, 1.0f, 0.0f); // Y
      glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,100.0f,0.0f);
      glColor3f(0.0f, 0.0f, 1.0f); // Z
      glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,100.0f);
      glEnd();

      glBegin(GL_LINES);
      glColor3f(0,0,0);
      for(int i = -100; i <= 100; i+=20){
	glVertex3f(i, -100, 0);	glVertex3f(i, 100, 0);
	glVertex3f(-100, i, 0);	glVertex3f(100, i, 0);
      };
      glEnd();

      glPushMatrix();
      glTranslatef(50,50,50);
      //test point
      glBegin(GL_LINES);
      glVertex3f(-3.0f,0.0f,0.0f); glVertex3f(3.0f,0.0f,0.0f);
      glVertex3f(0.0f,-3.0f,0.0f); glVertex3f(0.0f,3.0f,0.0f);
      glVertex3f(0.0f,0.0f,-3.0f); glVertex3f(0.0f,0.0f,3.0f);
      glEnd();
      glPopMatrix();
    };
  } scene; 

  runScene(scene);
};

TEST(OGL, RotationControl){

  struct: public Drawable{
    
    void Draw(){
      for(float fi = 0; fi < 360; fi += 12){
	glPushMatrix();
	glRotatef(fi, 1, 0, 0);
	glTranslatef(0,0,100);
	glBegin(GL_LINES);
	glColor3f(0,0,0);
	glVertex3f(-5,-5,0); glVertex3f(0,0,0);
	glVertex3f(0,0,0); glVertex3f(5,-5,0);
	glVertex3f(-5,-5,0); glVertex3f(5,-5,0);
	glEnd();
	//Internals
	glTranslatef(0,0,-0.1);
	glBegin(GL_TRIANGLES);
	glColor3f(0,0,1);
	glVertex3f(-5,-5,0); glVertex3f(0,0,0);
	glVertex3f(5,-5,0);
	glEnd();

	glPopMatrix();
      };
      
      //grid
      glBegin(GL_LINES);
      glColor3f(0,0,0);
      for(int i = -100; i <= 100; i+=20){
	glVertex3f(i, -100, 0);	glVertex3f(i, 100, 0);
	glVertex3f(-100, i, 0);	glVertex3f(100, i, 0);
      };
      glEnd();

    };
  } scene; 
  runScene(scene);
};

#define EPSILON 0.0001

TEST(Intersection, Sphere){
   Ray ray(V3f(1,1,1), V3f(0,1,0));
   Ray travel_test(ray);
   V3f res;
   Intersection pnt;
   travel_test.Travel(4, res);
   EXPECT_LT((V3f(1,5,1) - res).length(), EPSILON);
   EXPECT_FALSE(IntersectRaySphere(ray, V3f(0,4,0), 1.0f, pnt).hit);
   EXPECT_TRUE(IntersectRaySphere(ray, V3f(0,4,0), 2.0f, pnt).hit);
   EXPECT_FLOAT_EQ(2.0f, (ray.Travel(pnt.distance, res)-V3f(0,4,0)).length()); //actually equal to the radius. 
};

TEST(Intersection, Plane){
   Ray ray(V3f(1,1,1), V3f(0,1,0));
   Ray plane(V3f(2, 99, 8), V3f(0,1,0));
   Ray random_plane(V3f(2, 99, 8), V3f(3.4,1,-2.1));
   Intersection pnt;
   V3f res;
   
   IntersectRayPlane(ray, plane, pnt);
   EXPECT_EQ(pnt.hit, true);
   EXPECT_FLOAT_EQ(99.f, ray.Travel(pnt.distance, res).y);
   
   IntersectRayPlane(ray, random_plane, pnt);
   ray.Travel(pnt.distance, res);
   EXPECT_GT(0.0001f, (res-random_plane.O).dot(random_plane.D));
};
