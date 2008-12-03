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


struct QuadScene: public Drawable{
  void Draw(){
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(100,100,100); //random quad
    glVertex3f(100,-100,100);
    glVertex3f(-100,-100,100);
    glVertex3f(-100, 100,100);
    glEnd();

  };
};

struct TexturedScene: public Drawable{
  
  Texturizer & texturizer_;
  float test;

  TexturedScene(Texturizer & in): texturizer_(in){
  };

  void Draw(){
    test += 0.1;
    V3f off(10*sin(test), 10*cos(test), 0);
    V3f a(20.0f,20.0f,5.0f); //random quad
    V3f b(20.0f,-20.0f,-5.0f);
    V3f c(-20.0f,-20.0f,-5.0f);
    V3f d(-20.0f, 20.0f,5.0f);
    a+=off; b+=off; c+=off; d+=off;
    
    Range cur(c,a); ExpandRange(cur,b); ExpandRange(cur, d);
    CheckTexture(texturizer_, cur);
        
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);
    SetVertex(SetTexture(texturizer_, a)); 
    SetVertex(SetTexture(texturizer_, b)); 
    SetVertex(SetTexture(texturizer_, c)); 
    SetVertex(SetTexture(texturizer_, d)); 
    glEnd();

  };
};

struct SphereScene: public Drawable{
  void Draw(){
    drawSphere(6, 40.0f);
  };
};

TEST(OGL, Quad){
  QuadScene scene; runScene(scene);
};

TEST(OGL, Sphere){
  SphereScene scene; runScene(scene);
};

TEST(OGL, TexturedQuad){
  Texturizer tx;
  TexturedScene scene(tx); runScene(scene);
};

