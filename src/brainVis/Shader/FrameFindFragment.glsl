#version 330 core

uniform sampler3D volume;

uniform int frameMode = 0;
uniform float tfScaling;

uniform vec2 frameRange = vec2(0.45f,0.6f);

// INPUT VARIABLES
in vec3 vScreenPosition;

// OUTPUT VARIABLES
out vec4 outputColor;

// Pixel Shader
void main(void)
{
  float value = 0.0f;
  vec3 coord = vec3(vScreenPosition.z, vScreenPosition.x, vScreenPosition.y);

  value = texture(volume,coord).x;
  value *= tfScaling;

	if(value < frameRange.y && value > frameRange.x){
	  	outputColor = vec4(coord,1); 
	}else{
	  	outputColor = vec4(0,1,1,1);
	}
}

