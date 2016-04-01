#version 330 core

uniform vec3 color = vec3(1.0f,1.0f,1.0f);

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;

// Pixel Shader
void main(void)
{
  outputColor = vec4(color,entranceInViewSpace.z);
}
