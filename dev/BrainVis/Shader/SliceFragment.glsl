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

float calcR(vec3 X, vec3 d, vec3 A){
  float r = (X.x-A.x)*d.x + (X.y-A.y)*d.y + (X.z-A.z)*d.z;
  r = -r / (pow(d.x,2) + pow(d.y,2) + pow(d.z,2));
  return r;
}

// Pixel Shader
void main(void)
{
  // read ray parameters and start color
  ivec2 screenpos = ivec2(gl_FragCoord.xy);

  // fetch ray entry from texture and get ray exit point from vs-shader
  vec3 rayStart = texelFetch(rayStartPoint, screenpos,0).xyz;

  outputColor = vec4(0,0,0,-1000);
  outputPosition = vec4(rayStart,-1000);

  vec3 texturePos = rayStart;
  float value = 0;
  vec4 viewPos = vec4(0,0,0,0);

  //calculate the volumespace position of the clipping plane
  vec4 worldstart = worldFragmentMatrix*vec4(rayStart-vec3(0.5,0.5,0.5),1);
  vec4 worldend = worldFragmentMatrix*vec4(normalview-vec3(0.5,0.5,0.5),1);
  vec3 worlddir = worldend.xyz - worldstart.xyz;
  worlddir = normalize(worlddir);

  float distance = calcR(worldstart.xyz,worlddir,focusWorldPos);

  vec3 worldTarget = worldstart.xyz + distance * worlddir;

  mat4 invWorld = inverse(worldFragmentMatrix);

  texturePos = (invWorld* vec4(worldTarget,1)).xyz;
  texturePos = texturePos+ vec3(0.5,0.5,0.5);
  
  if( texturePos.x >= 0.0f && texturePos.x <= 1.0f &&
      texturePos.y >= 0.0f && texturePos.y <= 1.0f &&
      texturePos.z >= 0.0f && texturePos.z <= 1.0f){

      viewPos = worldFragmentMatrix*vec4(texturePos-vec3(0.5,0.5,0.5),1);

      value = texture(volume, texturePos).x;
      value *= tfScaling;

      viewPos = viewFragmentMatrix*viewPos;
                
      outputColor = vec4(value,value,value,viewPos.z);
      outputPosition = vec4(viewPos.xyz,viewPos.z);
  }
}
