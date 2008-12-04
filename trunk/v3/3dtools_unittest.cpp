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

TEST(OGL, Sphere){
  struct: public Drawable{
    void Draw(){
      DrawSphere( V3f(0,0,0), 30.0f);
    };
  } scene; 
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
      DrawSphere(V3f(50*sin(0.05*frame_no_), 50*sin(0.07*frame_no_+34), 50*sin(0.03*frame_no_+65)), 30.0f, 60,  
		 this);
    };
  } scene;
  runScene(scene);
};

TEST(OGL, TexturedFastVolumeSphere){
  struct: public Drawable, public Textured{
    void Draw(){
      DrawSphere(V3f(50*sin(0.05*frame_no_), 50*sin(0.07*frame_no_+34), 50*sin(0.03*frame_no_+65)), 30.0f, 3,  
		 this);
    };
  } scene;

  FastVolume volume;
  MgzLoader mri(volume);
  EXPECT_TRUE(mri.Load("brainmask.mgz"));

  scene.texturing_fastvolume = & volume;  

  runScene(scene);
};

