#version 410 core

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
uniform mat4 rotation;
uniform float slide = 0.0f;

uniform vec3 volumeOffset;

// Vertex Shader
void main(void)
{
  vec4 pos = vec4(0,0,0,0);
    

  pos = rotation*vec4(inputPosition.xyz, 1.0);
  pos = translate*pos;

  vScreenPosition = pos.xyz+vec3(0.5,0.5,0.5)-volumeOffset; 

  pos = scale*pos;
  pos = view*pos;

  switch(axis){
  case 0 : vScreenPosition.x = vScreenPosition.x-volumeOffset.x;break;
  case 1 : vScreenPosition.y = vScreenPosition.y-volumeOffset.y;break;
  case 2 : vScreenPosition.z = vScreenPosition.z-volumeOffset.z; break;
  }

  /*switch(axis){
  	case 0 : vScreenPosition = vec3( slide, inputPosition.y+0.5f , inputPosition.z+0.5f) ; break; //x
  	case 1 : vScreenPosition = vec3( inputPosition.x+0.5f ,slide , inputPosition.z+0.5f); break; //y
  	case 2 : vScreenPosition = vec3( inputPosition.x+0.5f, inputPosition.y+0.5f, slide); break;  //z
  }*/
  
  gl_Position = projection*pos;
  vViewPos = pos.xyz;
 
}
