#version 330 core

//uniform variables
uniform sampler2D rayStartPoint;
uniform sampler3D CTVolume;
uniform sampler3D MRVolume;

uniform mat4    worldFragmentMatrix;
uniform mat4    worldMRinverseFragmentMatrix[12];
uniform int     MRmatricies = 13;

uniform float   CTScaling;
uniform float   MRScaling;

uniform float   stepSize = 2500.0f;

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor1;
layout(location=1) out vec4 outputColor2;
layout(location=2) out vec4 outputColor3;
layout(location=3) out vec4 outputColor4;

// Pixel Shader
void main(void)
{
  // read ray parameters and start color
  ivec2 screenpos = ivec2(gl_FragCoord.xy);

  // fetch ray entry from texture and get ray exit point from vs-shader
  vec3 rayStart = texelFetch(rayStartPoint, screenpos,0).xyz;
  vec3 rayDir = normalview-rayStart;
  rayDir = rayDir/length(rayDir);

  outputColor1 = vec4(0,0,0,0);
  outputColor2 = vec4(0,0,0,0);
  outputColor3 = vec4(0,0,0,0);
  outputColor4 = vec4(0,0,0,0);
  
  float outputValue = 0.0f;

  vec3 texturePos = rayStart;
  
  vec3  CTWorldPosition = vec3(0,0,0);
  vec3  MRposition = vec3(0,0,0);
  
  float MRValue = 0;
  float CTValue = 0;

  float VollumeDiff;

  for(int i = 0; i < 600;++i){
    CTValue = texture(CTVolume, texturePos).x;
    CTValue *= CTScaling;

    if(CTValue > 0.4f){
        CTValue = 0;
    }

    CTWorldPosition = (worldFragmentMatrix*vec4(texturePos-vec3(0.5,0.5,0.5),1)).xyz;
    for(int k = 0; k < MRmatricies;++k){
        MRValue = 0;
        MRposition = (worldMRinverseFragmentMatrix[k]*vec4(CTWorldPosition,1)).xyz;
        MRposition = MRposition + vec3(0.5,0.5,0.5);

        if( MRposition.x >= 0.0 && MRposition.x <= 1.0f && 
            MRposition.y >= 0.0 && MRposition.y <= 1.0f &&
            MRposition.z >= 0.0 && MRposition.z <= 1.0f){

            MRValue = texture(MRVolume, MRposition).x;
            MRValue *= MRScaling;
        }

        VollumeDiff = (CTValue-MRValue)*(CTValue-MRValue);

        switch(k){
          case 0 : outputColor1.x += VollumeDiff; break;
          case 1 : outputColor1.y += VollumeDiff; break;
          case 2 : outputColor1.z += VollumeDiff; break;
          case 3 : outputColor2.x += VollumeDiff; break;
          case 4 : outputColor2.y += VollumeDiff; break;
          case 5 : outputColor2.z += VollumeDiff; break;
          case 6 : outputColor3.x += VollumeDiff; break;
          case 7 : outputColor3.y += VollumeDiff; break;
          case 8 : outputColor3.z += VollumeDiff; break;
          case 9 : outputColor4.x += VollumeDiff; break;
          case 10 : outputColor4.y += VollumeDiff; break;
          case 11 : outputColor4.z += VollumeDiff; break;
        }
    }
    
    texturePos += rayDir*stepSize;

    //early ray termination
    if(texturePos.x > 1.0f || texturePos.y > 1.0f || texturePos.z > 1.0f ||
      texturePos.x < 0.0f || texturePos.y < 0.0f || texturePos.z < 0.0f){
       i = 20000;
      break;
    }
  }
  //outputColor = vec4(outputValue,outputValue,outputValue,1);
}
