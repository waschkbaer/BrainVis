#version 330 core

// INPUT VARIABLES
in vec3 inputPosition;
in vec3 inputNormal;
in vec2 inputTexcoord;

// OUTPUT VARIABLES
out vec3 normalview;
out vec3 entranceInViewSpace;

// UNIFORM VARIABLES
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;



// Vertex Shader
void main(void)
{
	// Calculate the position
    vec4 pos = vec4(inputPosition, 1.0);

    gl_Position = worldMatrix * pos;
    gl_Position = viewMatrix * gl_Position;
  
    entranceInViewSpace = gl_Position.xyz;
  
    gl_Position = projectionMatrix * gl_Position;
  
    normalview = inputPosition+ vec3(0.5,0.5,0.5);
  

}
