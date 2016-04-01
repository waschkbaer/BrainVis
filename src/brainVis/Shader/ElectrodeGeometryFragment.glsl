#version 330 core

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

uniform vec3 color = vec3(0.8f,1.0f,0.2f);

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;

// Pixel Shader
void main(void)
{
  outputColor = vec4(color,entranceInViewSpace.z);
}
