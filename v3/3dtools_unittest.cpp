#include <gtest/gtest.h>

#include "3dtools.h"
#include "fastvolume.h"
#include "loader.h"
#include <GL/glfw.h>

//Volume data loading.
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

/*
//Simplest possile 3D scene.
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
*/

//DrawSphere function.
TEST(OGL, Sphere){
  struct: public Drawable{
    void Draw(){
      DrawSphere( V3f(0,0,0), 98.0f, 40);
    };
  } scene; 
  runScene(scene);
};

//Blending text.
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

//Textures.
TEST(OGL, TexturedQuad){
  
  struct: public Drawable  {
    Textured texture;

    void Draw(){

      V3f a(20.0f,20.0f,5.0f); //random quad
      V3f b(20.0f,-20.0f,-5.0f);
      V3f c(-20.0f,-20.0f,-5.0f);
      V3f d(-20.0f, 20.0f,5.0f);
          
      Range cur(c,a); ExpandRange(cur,b); ExpandRange(cur, d);
      texture.CheckTexture(cur);
        
      glBegin(GL_QUADS);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(texture.SetTexture(a)); 
      glVertex3f(texture.SetTexture(b)); 
      glVertex3f(texture.SetTexture(c)); 
      glVertex3f(texture.SetTexture(d)); 
      glEnd();

    };

  } scene; 

  runScene(scene);
};

//Explore detection visualization. 
TEST(OGL, SurfaceAnalyzer){
  
  struct: public Drawable{
    Surface surf;
    FastVolume vol;

    void Draw(){
      glDisable(GL_LIGHTING);
      DrawSurface( surf );
    };
  } scene; 
  /*
  //Perform surface modification if user asked.
  struct public EventSlider {
  Surface * surf;
  bool Do(int value){
  for(int i = 0; i < surf.v.size(); i++){
  V3f c;
  AnalyzePoint(surf.v[i]+surf.n[i]*sin(0.01*x)*sin(0.01*x)*1.5, surf.n[i], vol, c);
  surf.c[i] = c;
  };
  };
  };
  */  

  //Load the triangle data.
  EXPECT_TRUE(read_surface_binary(scene.surf, "lh.pial"));
  //Load volume.
  MgzLoader mri(scene.vol);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  AnalyzeSurface(scene.surf, scene.vol);
  
  runScene(scene);  
};

//Loading surface.
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

/** Use the class to store texture
    to be passed on to other objects during 
    rendering. */

TEST(OGL, TexturedSphere){
  struct: public Drawable{
    Textured * texture;

    void Draw(){
      glDisable(GL_BLEND);
      glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      DrawSphere(V3f(0, 0, 0), 30.0f, 60, texture);
    };
  } scene;

  Textured texture;
  scene.texture = & texture;

  runScene(scene);
};

TEST(OGL, TexturedFastVolumeSphere){
  struct: public Drawable {
    Textured * texture;

    void Draw(){
      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      DrawSphere(V3f(0,0,0), 30.0f, 5, texture);
    };
  } scene;

  FastVolume volume;
  MgzLoader mri(volume);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));
  EXPECT_NE((void *)0 , volume.vol);

  Textured texture; //This will keep track of its scope.
  scene.texture = &texture;
  texture.texturing_fastvolume = & volume;  
  

  runScene(scene);
};

//Basic interface test. (Probably redundant.)
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
    Textured texture;
    Surface surf;
    FastVolume vol;
    Ray in;

    void Draw(){

      int x, y;
      glfwGetMousePos(&x, &y);

      //DrawSurface( surf );
      DrawSurfaceLines( surf);

      glDisable(GL_LIGHTING);

      glPushMatrix();

      Intersection hit;
      
      if(glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS)in = mousePosition();

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

      DrawSphere(in.Travel(min_depth+1), 15.0, 10, & texture);

      glBegin(GL_LINES);
      //Draw normals
      glColor3f(1,0,0);
      if(glfwGetKey(GLFW_KEY_TAB) == GLFW_PRESS){
	for(int i = 0; i < surf.v.size(); i++){
	  V3f c;
	  AnalyzePoint(surf.v[i]+surf.n[i]*sin(0.01*x)*sin(0.01*x)*1.5, surf.n[i], vol, c);
	  surf.c[i] = c;
	};
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
  scene.texture.texturing_fastvolume = & scene.vol;

  AnalyzeSurface(scene.surf, scene.vol);
  
  runScene(scene);  
};

/*
//Now every scene got a camera.
*/
TEST(OGL, Camera){

  struct: public Drawable{
    void Draw(){
      DrawSphere( V3f (0,0,0), 30.0f);

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

TEST(Intersection, Mesh){
  
};

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
