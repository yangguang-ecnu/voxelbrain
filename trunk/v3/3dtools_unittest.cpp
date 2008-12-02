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

