#version 330 core

// INPUT VARIABLES
layout (location=0) in vec3 inputPosition;

uniform int axis = 0;
//0 = x 1 = y 2 = z

// OUTPUT VARIABLES
out vec3 vScreenPosition;
out vec3 vViewPos;

// UNIFORM VARIABLES
uniform mat4 projection;
uniform mat4 view;
uniform mat4 scale;
uniform mat4 translate;
uniform mat4 rotationX;
uniform mat4 rotationY;
uniform mat4 rotationZ;
uniform float slide = 0.0f;

uniform vec3 volumeOffset;

// Vertex Shader
void main(void)
{
  mat4 scaleInv = inverse(scale);
  vec4 pos = vec4(0,0,0,0);
    
  pos = scale*vec4(inputPosition.xyz, 1.0);
  pos = rotationX*pos;
  pos = rotationY*pos;
  pos = rotationZ*pos;
  pos = scaleInv*pos;
  pos = translate*pos;
  

  vScreenPosition = pos.xyz+vec3(0.5,0.5,0.5)-volumeOffset; 

  pos = scale*pos;
  pos = view*pos;

  switch(axis){
    case 0 :  vScreenPosition.y = inputPosition.y+0.5f;
              vScreenPosition.z = inputPosition.z+0.5f;
              vScreenPosition.x = vScreenPosition.x-volumeOffset.x;
              break;
    case 1 :  vScreenPosition.x = inputPosition.x+0.5f;
              vScreenPosition.z = inputPosition.z+0.5f;
              vScreenPosition.y = vScreenPosition.y-volumeOffset.y;
              break;
    case 2 :  vScreenPosition.x = inputPosition.x+0.5f;
              vScreenPosition.y = inputPosition.y+0.5f;
              vScreenPosition.z = vScreenPosition.z-volumeOffset.z; 
              break;
  }
  gl_Position = projection*pos;
  vViewPos = pos.xyz;
 
}
