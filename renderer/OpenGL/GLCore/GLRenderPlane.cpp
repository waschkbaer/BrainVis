//
//  GLRenderPlane.cpp
//  TNG
//
//  Created by Andre Waschk on 18/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#include "GLRenderPlane.h"


using namespace Tuvok::Renderer::OpenGL::GLCore;
using namespace Core::Math;



static float PlaneData[32] =
{
  -1.0f, 1.0f, -0.5f,
  -1.0f, -1.0f, -0.5f,
  1.0f, -1.0f, -0.5f,
  1.0f, 1.0f, -0.5f,
};

static float PlaneDataZ[32] =
{
  -1.0f, 1.0f, 0.0f,
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  1.0f, 1.0f, 0.0f,
};

static float PlaneDataY[32] =
{
  -1.0f,  0.0f, 1.0f,
  -1.0f,  0.0f, -1.0f,
  1.0f,  0.0f, -1.0f,
  1.0f,  0.0f, 1.0f,
};


static float PlaneDataX[32] =
{
  0.0f,-1.0f,   1.0f,
  0.0f,-1.0f,   -1.0f,
  0.0f,1.0f,   -1.0f,
   0.0f,1.0f,  1.0f,
};

static int PlaneIndices[6]{
  0, 1, 2,
  2, 3, 0,
};

GLRenderPlane::GLRenderPlane(float* corners, const Core::Math::Vec2ui screenResolution,int axis){
    Initialize(corners, PlaneIndices, 4, 6);

    m_mWorld.Translation(0, 0, 0);
    m_mWorld.Scaling(Vec3f(1,1,1));
    m_mView.BuildLookAt(Vec3f(0,0,2), Vec3f(0,0,0), Vec3f(0,1,0));
    m_mProjection.Ortho(-0.5f, 0.5f, -0.5f, 0.5f, 0.1f, 100.0f);
}

GLRenderPlane::GLRenderPlane(const Core::Math::Vec2ui screenResolution,int axis){
   switch(axis){
   case 0 : Initialize(PlaneDataX, PlaneIndices, 4, 6); break;
   case 1 : Initialize(PlaneDataY, PlaneIndices, 4, 6);break;
   case 2 : Initialize(PlaneDataZ, PlaneIndices, 4, 6);break;
   default: Initialize(PlaneData, PlaneIndices, 4, 6);
   }

  
  m_mWorld.Translation(0, 0, 0);
  m_mWorld.Scaling(Vec3f(1,1,1));
  m_mView.BuildLookAt(Vec3f(0,0,2), Vec3f(0,0,0), Vec3f(0,1,0));
  m_mProjection.Ortho(-0.5f, 0.5f, -0.5f, 0.5f, 0.1f, 100.0f);
}
GLRenderPlane::~GLRenderPlane(){}

const Core::Math::Mat4f GLRenderPlane::getPlaneView() const{
  return m_mView;
}
const Core::Math::Mat4f GLRenderPlane::getPlaneProjection() const{
  return m_mProjection;
}
const Core::Math::Mat4f GLRenderPlane::getPlaneWorld() const{
  return m_mWorld;
}
