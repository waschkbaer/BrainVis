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
uniform mat4 world;
uniform mat4 rotation;
uniform float slide = 0.0f;

// Vertex Shader
void main(void)
{
  vec4 pos = world*vec4(inputPosition.xyz, 1.0);

  pos = view*pos;

  //vScreenPosition = vec3( (inputPosition.xy+ vec2(1.0,1.0)) / vec2(2.0,2.0) ,slide);

  
  vec3 volumeSpace = (rotation*vec4(inputPosition.xyz, 1.0)).xyz;
  vec3 test = (volumeSpace.xyz + vec3(1.0,1.0,1.0)) / vec3(2.0,2.0,2.0);
  volumeSpace = (inputPosition.xyz + vec3(1.0,1.0,1.0)) / vec3(2.0,2.0,2.0);

  switch(axis){
  	case 0 : vScreenPosition = vec3( slide, inputPosition.y+0.5f , inputPosition.z+0.5f) ; break; //x
  	case 1 : vScreenPosition = vec3( inputPosition.x+0.5f ,slide , inputPosition.z+0.5f); break; //y
  	case 2 : vScreenPosition = vec3( inputPosition.x+0.5f, inputPosition.y+0.5f, slide); break;  //z
  }
  
  gl_Position = projection*pos;
  vViewPos = pos.xyz;
 
}
