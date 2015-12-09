#version 330 core

uniform sampler2D rayStartPoint;
uniform sampler3D volume;
uniform sampler1D transferfunction;

uniform mat4    viewFragmentMatrix;
uniform mat4    worldFragmentMatrix;
uniform float   tfScaling;
uniform vec3    eyePos;
uniform vec3    focusWorldPos = vec3(7.97907,18.9604,-24.2198);
uniform vec3    cutPlaneNormal = vec3(1,0,0);
uniform int     cutMode = 1;

uniform float   stepSize = 2500.0f;
uniform float   isCTImage = 0.0f;

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;
layout(location=1) out vec4 outputPosition;


bool isTwoDCut(vec3 curPos, vec3 focusPos, vec3 axis){
  if(axis.x == 1.0 || axis.x == -1.0){
      if(abs(focusPos.x - curPos.x) < 0.7){
        return false;
      }
  }else if(axis.y == 1.0 || axis.y == -1.0){
      if(abs(focusPos.y - curPos.y) < 0.7){
        return false;
      }
  }else if(axis.z == 1.0 || axis.z == -1.0){
      if(abs(focusPos.z - curPos.z) < 0.7){
        return false;
      }
  }
  return true;
}

// Pixel Shader
void main(void)
{
  // read ray parameters and start color
  ivec2 screenpos = ivec2(gl_FragCoord.xy);

  // fetch ray entry from texture and get ray exit point from vs-shader
  vec3 rayStart = texelFetch(rayStartPoint, screenpos,0).xyz;
  vec3 rayDir = normalview-rayStart;
  rayDir = rayDir/length(rayDir);

  outputColor = vec4(0,0,0,-1000);
  outputPosition = vec4(rayStart,-1000);

  vec3 texturePos = rayStart;
  vec4 finalColor = vec4(0,0,0,0);
  float value = 0;
  vec4 viewPos = vec4(0,0,0,0);
  bool isCut = false;
  
  for(int i = 0; i < 4000;++i){
    viewPos= worldFragmentMatrix*vec4(texturePos-vec3(0.5,0.5,0.5),1);

    switch(cutMode){
      case 3:  isCut = isTwoDCut( viewPos.xyz,     //2d cut
                                  focusWorldPos,
                                  cutPlaneNormal);
                                  break;
      default : i = 4001; outputColor = vec4(0,0,0,0); isCut = true; break;
    }

    if(!isCut){

          value = texture(volume, texturePos).x;
          value *= tfScaling;

          viewPos = viewFragmentMatrix*viewPos;
            
          outputColor = vec4(finalColor.xyz,viewPos.z);
          outputColor = vec4(value,value,value,viewPos.z);

          if(isCTImage > 0.5f && value > 0.4f){
            outputColor = vec4(0,0,0,viewPos.z);
          }
            
          outputPosition = vec4(texturePos,viewPos.z);

          i = 4000;
          break;
    }
    texturePos += rayDir/stepSize;

    //early ray termination
    if(texturePos.x > 1.0f || texturePos.y > 1.0f || texturePos.z > 1.0f ||
      texturePos.x < 0.0f || texturePos.y < 0.0f || texturePos.z < 0.0f){
    i = 1000;
      break;
    }
  }
  
}
