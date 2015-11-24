//
//  GLBoundingBox.h
//  TNG
//
//  Created by Andre Waschk on 30/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#ifndef __TNG__GLBasicShapes__
#define __TNG__GLBasicShapes__

#include "GLModel.h"

namespace Tuvok{
  namespace Renderer{
    namespace OpenGL{
      namespace GLCore{
        
      class GLBox: public GLModel{
      public:
        GLBox(float width, float height, float depth, Core::Math::Vec3f center);
        virtual ~GLBox();

      private:
      };
      typedef std::shared_ptr<GLBox> GLBoxPtr;

      class GLCylinder: public GLModel{
      public:
        GLCylinder(float radius, float segments);
        virtual ~GLCylinder();

      private:
      };
      typedef std::shared_ptr<GLCylinder> GLCylinderPtr;

      };
    };
  };
};

#endif /* defined(__TNG__GLBasicShapes__) */
