//
//  GLBoundingBox.cpp
//  TNG
//
//  Created by Andre Waschk on 30/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#include "GLBasicShapes.h"

using namespace Tuvok::Renderer::OpenGL::GLCore;


GLBox::GLBox(float width, float height, float depth, Core::Math::Vec3f center){
    float CubeData[72] =
   {
     0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     0.5f*width+center.x,-0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x,-0.5f*height+center.y, 0.5f*depth+center.z,
     0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     0.5f*width+center.x,-0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x,-0.5f*height+center.y, 0.5f*depth+center.z,

     0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     0.5f*width+center.x,-0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x,-0.5f*height+center.y, -0.5f*depth+center.z,
     0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     0.5f*width+center.x,-0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x,-0.5f*height+center.y, -0.5f*depth+center.z,

     0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x, 0.5f*height+center.y, -0.5f*depth+center.z,
     0.5f*width+center.x, -0.5f*height+center.y, 0.5f*depth+center.z,
     0.5f*width+center.x, -0.5f*height+center.y, -0.5f*depth+center.z,
     -0.5f*width+center.x, -0.5f*height+center.y, 0.5f*depth+center.z,
     -0.5f*width+center.x, -0.5f*height+center.y, -0.5f*depth+center.z,
   };

    int CubeIndicies[24]{
      0,1,
      2,3,
      4,5,
      6,7,
      8,9,
      10,11,
      12,13,
      14,15,
      16,17,
      18,19,
      20,21,
      22,23
    };


  Initialize(CubeData, CubeIndicies, 24, 24);
}
GLBox::~GLBox(){}

GLCylinder::GLCylinder(float radius, float segments){
    float x,z;
    float xn,zn;

    double steps = (2*3.14159265359)/segments;
    double cur = 0;
    int index = 0;

    std::vector<Core::Math::Vec3f> pos;
    std::vector<int> ind;

    for(int i = 0; i < segments; i++){
        x = (float)sin(cur)*radius;
        z = (float)cos(cur)*radius;
        cur+= steps;
        xn = (float)sin(cur)*radius;
        zn = (float)cos(cur)*radius;

        pos.push_back(Core::Math::Vec3f(x,0,z));
        pos.push_back(Core::Math::Vec3f(x,1,z));
        pos.push_back(Core::Math::Vec3f(xn,0,zn));

        pos.push_back(Core::Math::Vec3f(xn,0,zn));
        pos.push_back(Core::Math::Vec3f(xn,1,zn));
        pos.push_back(Core::Math::Vec3f(x,1,z));

        ind.push_back(index);index++;
        ind.push_back(index);index++;
        ind.push_back(index);index++;
        ind.push_back(index);index++;
        ind.push_back(index);index++;
        ind.push_back(index);index++;
    }

    Initialize(&(pos[0].x), &(ind[0]), pos.size(), ind.size());
}
GLCylinder::~GLCylinder(){}

