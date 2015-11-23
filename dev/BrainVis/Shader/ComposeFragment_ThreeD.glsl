#version 410 core

uniform sampler2D raycastCT;
uniform sampler2D raycastMR;
uniform sampler2D boundingBox;
uniform sampler2D fontTexture;

uniform float slide = 0.0f;
uniform int axis = 0;
uniform float mrctblend = 0.5f;

uniform vec3 bottomBG = vec3(0.1f,0.2f,0.5f);
uniform vec3 topBG = vec3(0.4f,1.0f,1.0f);

// INPUT VARIABLES
in vec2 vScreenPosition;

// OUTPUT VARIABLES
out vec4 outputColor;

// Pixel Shader
void main(void)
{
  vec4 raycastColorCT = texture(raycastCT,vScreenPosition);
  vec4 raycastColorMR = texture(raycastMR,vScreenPosition);
  vec4 boundingColor = texture(boundingBox,vScreenPosition);
  vec3 font = texture(fontTexture,vec2(vScreenPosition.x,1.0-vScreenPosition.y)).xyz;

  vec3 finalColor = raycastColorCT.xyz;
  finalColor = raycastColorCT.xyz*mrctblend + (1.0f-mrctblend)* raycastColorMR.xyz;

  float depth = -8000.0f;

  if(raycastColorCT.w < raycastColorMR.w){
    depth = raycastColorCT.w;
  }else{
    depth = raycastColorMR.w;
  }

  if(depth < boundingColor.w &&
	(boundingColor.x != 0.0f ||
	boundingColor.y != 0.0f ||
	boundingColor.z != 0.0f)
  ){
  	finalColor = boundingColor.xyz;
  }

  if( finalColor.x == 0.0 &&
      finalColor.y == 0.0 &&
      finalColor.z == 0.0){
    finalColor.xyz = topBG*vScreenPosition.y + (1.0f -vScreenPosition.y)*bottomBG;
  }

  if(font.x == 0 && font.y == 0 && font.z == 0){
    outputColor = vec4(finalColor,1);
  }else{
    outputColor = vec4(font,1);
  }
  
}

