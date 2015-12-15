//
//  GLBoundingBox.cpp
//  TNG
//
//  Created by Andre Waschk on 30/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#include "GLBoundingBox.h"

using namespace Tuvok::Renderer::OpenGL::GLCore;



static float CubeData[72] =
{
  0.5f, 0.5f, 0.5f,
  0.5f,-0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  -0.5f,-0.5f, 0.5f,
  0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  0.5f,-0.5f, 0.5f,
  -0.5f,-0.5f, 0.5f,

  0.5f, 0.5f, -0.5f,
  0.5f,-0.5f, -0.5f,
  -0.5f, 0.5f, -0.5f,
  -0.5f,-0.5f, -0.5f,
  0.5f, 0.5f, -0.5f,
  -0.5f, 0.5f, -0.5f,
  0.5f,-0.5f, -0.5f,
  -0.5f,-0.5f, -0.5f,

  0.5f, 0.5f, 0.5f,
  0.5f, 0.5f, -0.5f,
  -0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, -0.5f,
  0.5f, -0.5f, 0.5f,
  0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
};

static int CubeIndicies[24]{
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

static float xAxis[24] =
{
    0.0f, -0.5f, -0.5f,
    0.0f, 0.5f, -0.5f,

    0.0f, 0.5f, -0.5f,
    0.0f, 0.5f, 0.5f,

    0.0f, 0.5f, 0.5f,
    0.0f, -0.5f, 0.5f,

    0.0f, -0.5f, 0.5f,
    0.0f, -0.5f, -0.5f,
};


static float yAxis[24] =
{
     -0.5f,0.0f, -0.5f,
     0.5f,0.0f, -0.5f,

     0.5f,0.0f, -0.5f,
     0.5f,0.0f, 0.5f,

     0.5f,0.0f, 0.5f,
     -0.5f,0.0f, 0.5f,

     -0.5f,0.0f, 0.5f,
     -0.5f,0.0f, -0.5f,
};


static float zAxis[24] =
{
     -0.5f, -0.5f,0.0f,
     0.5f, -0.5f,0.0f,

     0.5f, -0.5f,0.0f,
     0.5f, 0.5f,0.0f,

     0.5f, 0.5f, 0.0f,
     -0.5f, 0.5f,0.0f,

     -0.5f, 0.5f,0.0f,
     -0.5f, -0.5f,0.0f,
};

static int xAxisIndicies[8]{
  0,1,
  2,3,
  4,5,
  6,7,
};



GLBoundingBox::GLBoundingBox(){
  Initialize(CubeData, CubeIndicies, 24, 24);
}

GLBoundingBox::GLBoundingBox(Core::Math::Vec3f min, Core::Math::Vec3f max){

    std::vector<Core::Math::Vec3f> Cube;

    Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));
    Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));

    Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));
    Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));

    Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));
    Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));

    Initialize(&(Cube[0].x), CubeIndicies, 24, 24);
}


GLBoundingBox::~GLBoundingBox(){}

GLBoundingQuad::GLBoundingQuad(float* data){
  Initialize(data, xAxisIndicies, 8, 8);
}
GLBoundingQuad::~GLBoundingQuad(){}

GLBoundingQuadX::GLBoundingQuadX():
GLBoundingQuad(xAxis){
}
GLBoundingQuadX::~GLBoundingQuadX(){}

GLBoundingQuadY::GLBoundingQuadY():
    GLBoundingQuad(yAxis){
}
GLBoundingQuadY::~GLBoundingQuadY(){}

GLBoundingQuadZ::GLBoundingQuadZ():
    GLBoundingQuad(zAxis){
}
GLBoundingQuadZ::~GLBoundingQuadZ(){}
