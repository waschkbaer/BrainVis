//
//  Camera.h
//  TNG
//
//  Created by Andre Waschk on 24/01/15.
//  Copyright (c) 2015 CoViDaG. All rights reserved.
//

#ifndef __TNG__Camera__
#define __TNG__Camera__

#include <core/Math/Vectors.h>

namespace Tuvok{
  namespace Renderer{

    class Camera{
    public:
      Camera();
      Camera(Core::Math::Vec3f position, Core::Math::Vec3f at, Core::Math::Vec3f up );
      ~Camera();

      void moveCamera(Core::Math::Vec3f direction);
      void rotateCamera(Core::Math::Vec3f rotation = Core::Math::Vec3f(0,0,0));

      Core::Math::Mat4f buildLookAt();

      void Zoom(float f);
      void setZoom(float f);
      float getZoom() const;

      void setCameraMode(bool mode);
      const bool getFirstPerson() const;

	  void roll(float);
	  void pivit(float);

	  void tilt(float);

	  Core::Math::Vec3f GetWorldPosition() const;
      Core::Math::Vec3f getLookAt() const{
          return m_vLookAt;
      }
      void setLookAt(Core::Math::Vec3f lookAt){
            m_vLookAt = lookAt;
      }

	  void ResetCamera();

    private:
      float m_fZoom;


      bool m_bFirstPerson;
      Core::Math::Vec3f m_vPosition;
      Core::Math::Vec3f m_vLookAt;

      Core::Math::Vec3f m_vDir;
      Core::Math::Vec3f m_vUp;
      Core::Math::Vec3f m_vRight;

      Core::Math::Vec3f m_vRotation;

    };
    typedef std::shared_ptr<Camera> CameraPtr;
  }
}

#endif /* defined(__TNG__Camera__) */
