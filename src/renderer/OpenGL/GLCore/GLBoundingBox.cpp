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

    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));

    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));

    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, max.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(max.x, min.y, min.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, max.z));
    _Cube.push_back(Core::Math::Vec3f(min.x, min.y, min.z));

    Initialize(&(_Cube[0].x), CubeIndicies, 24, 24);
}

void GLBoundingBox::update(Core::Math::Vec3f min, Core::Math::Vec3f max){

    _Cube[0] = Core::Math::Vec3f(max.x, max.y, max.z);
    _Cube[1] = Core::Math::Vec3f(max.x, min.y, max.z);
    _Cube[2] = Core::Math::Vec3f(min.x, max.y, max.z);
    _Cube[3] = Core::Math::Vec3f(min.x, min.y, max.z);
    _Cube[4] = Core::Math::Vec3f(max.x, max.y, max.z);
    _Cube[5] = Core::Math::Vec3f(min.x, max.y, max.z);
    _Cube[6] = Core::Math::Vec3f(max.x, min.y, max.z);
    _Cube[7] = Core::Math::Vec3f(min.x, min.y, max.z);

    _Cube[8] = Core::Math::Vec3f(max.x, max.y, min.z);
    _Cube[9] = Core::Math::Vec3f(max.x, min.y, min.z);
    _Cube[10] = Core::Math::Vec3f(min.x, max.y, min.z);
    _Cube[11] = Core::Math::Vec3f(min.x, min.y, min.z);
    _Cube[12] = Core::Math::Vec3f(max.x, max.y, min.z);
    _Cube[13] = Core::Math::Vec3f(min.x, max.y, min.z);
    _Cube[14] = Core::Math::Vec3f(max.x, min.y, min.z);
    _Cube[15] = Core::Math::Vec3f(min.x, min.y, min.z);

    _Cube[16] = Core::Math::Vec3f(max.x, max.y, max.z);
    _Cube[17] = Core::Math::Vec3f(max.x, max.y, min.z);
    _Cube[18] = Core::Math::Vec3f(min.x, max.y, max.z);
    _Cube[19] = Core::Math::Vec3f(min.x, max.y, min.z);
    _Cube[20] = Core::Math::Vec3f(max.x, min.y, max.z);
    _Cube[21] = Core::Math::Vec3f(max.x, min.y, min.z);
    _Cube[22] = Core::Math::Vec3f(min.x, min.y, max.z);
    _Cube[23] = Core::Math::Vec3f(min.x, min.y, min.z);

    updateVertexData(&(_Cube[0].x),24);
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
