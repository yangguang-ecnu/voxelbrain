#include <gtest/gtest.h>
#include "v3tools.h"

// Test loading of volume
TEST(V3tools, RangeBasic){
  Range one(V3f(0,0,0), V3f(1,1,1)); //start
  Range inside(V3f(0.1,0.1,0.1), V3f(0.9,0.9,0.9)); 
  Range intersecting(V3f(0.1,0.1,0.1), V3f(0.9,5,0.9)); 
  Range outside(V3f(1.1,-2,3), V3f(1.2,-1,3.8)); 

  EXPECT_TRUE(ContainsPoint(one, V3f(0.5, 0.5, 0.5)));  // Some common cases
  EXPECT_FALSE(ContainsPoint(one, V3f(1.1, 0.5, 0.5)));
  EXPECT_FALSE(ContainsPoint(one, V3f(-0.1, 0.5, 0.5)));
  EXPECT_FALSE(ContainsPoint(one, V3f(0.5, 1.1, 0.5)));
  EXPECT_FALSE(ContainsPoint(one, V3f(0.5, -0.1, 0.5)));
  EXPECT_FALSE(ContainsPoint(one, V3f(0.5, 0.5, 1.1)));
  EXPECT_FALSE(ContainsPoint(one, V3f(0.5, 0.5, -0.1)));
  EXPECT_TRUE(ContainsRange(one, inside));
  EXPECT_TRUE(IntersectsRange(one, intersecting));
  EXPECT_FALSE(ContainsRange(one, intersecting));
  EXPECT_FALSE(ContainsRange(one, outside));
};
