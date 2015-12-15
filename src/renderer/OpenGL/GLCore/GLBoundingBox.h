//
//  GLBoundingBox.h
//  TNG
//
//  Created by Andre Waschk on 30/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#ifndef __TNG__GLBoundingBox__
#define __TNG__GLBoundingBox__

#include "GLModel.h"

namespace Tuvok{
  namespace Renderer{
    namespace OpenGL{
      namespace GLCore{
        
        class GLBoundingBox: public GLModel{
        public:
          GLBoundingBox();
          GLBoundingBox(Core::Math::Vec3f min, Core::Math::Vec3f max);
          virtual ~GLBoundingBox();
          
        private:
        };
        typedef std::shared_ptr<GLBoundingBox> GLBoundingBoxPtr;

        class GLBoundingQuad: public GLModel{
        public:
          GLBoundingQuad(float* data);
          virtual ~GLBoundingQuad();

        private:
        };
        

        class GLBoundingQuadX: public GLBoundingQuad{
        public:
          GLBoundingQuadX();
          virtual ~GLBoundingQuadX();

        private:
        };



      class GLBoundingQuadY: public GLBoundingQuad{
      public:
        GLBoundingQuadY();
        virtual ~GLBoundingQuadY();

      private:
      };



      class GLBoundingQuadZ: public GLBoundingQuad{
      public:
        GLBoundingQuadZ();
        virtual ~GLBoundingQuadZ();

      private:
      };

      };
    };
  };
};

#endif /* defined(__TNG__GLBoundingBox__) */
