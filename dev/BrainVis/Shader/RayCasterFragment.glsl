#version 420 core

layout(binding=0) uniform sampler2D rayStartPoint;
layout(binding=1) uniform sampler3D volume;
layout(binding=2) uniform sampler1D transferfunction;

uniform mat4 viewFragmentMatrix;
uniform mat4 worldFragmentMatrix;
uniform float tfScaling;
uniform vec3 eyePos;
uniform vec3 focusWorldPos = vec3(7.97907,18.9604,-24.2198);

// INPUT VARIABLES
in vec3 normalview;
in vec3 entranceInViewSpace;

// OUTPUT VARIABLES
layout(location=0) out vec4 outputColor;
layout(location=1) out vec4 outputPosition;


bool isPositiv(float x){
  if(x >= 0)
    return true;
  else
    return false;
}

bool isCut(vec3 curPos, vec3 focusPos, vec3 start){
  vec3 dirEye = start-focusPos;
  vec3 dirPos = curPos-focusPos;
  dirEye = normalize(dirEye);
  start = normalize(start);

  bool xcut = false;
  bool ycut = false;
  bool zcut = false;

  if( ( (isPositiv(dirEye.x) && isPositiv(dirPos.x)) ||
      (!isPositiv(dirEye.x) && !isPositiv(dirPos.x)) ) &&

      ( (isPositiv(dirEye.y) && isPositiv(dirPos.y)) ||
      (!isPositiv(dirEye.y) && !isPositiv(dirPos.y)) ) &&

      ( (isPositiv(dirEye.z) && isPositiv(dirPos.z)) ||
      (!isPositiv(dirEye.z) && !isPositiv(dirPos.z)) )

      ){
    return true;
  }


  return false;
}

// Pixel Shader
void main(void)
{
  // read ray parameters and start color
  ivec2 screenpos = ivec2(gl_FragCoord.xy);

  // fetch ray entry from texture and get ray exit point from vs-shader
  vec3 rayStart = texelFetch(rayStartPoint, screenpos,0).xyz;
  vec3 rayDir = normalview-rayStart;
  rayDir = rayDir/length(rayDir);

  outputColor = vec4(0,0,0,-1000);
  outputPosition = vec4(rayStart,-1000);

  vec3 texturePos = rayStart;
  vec4 finalColor = vec4(0,0,0,0);
  float value = 0;
  vec4 viewPos = vec4(0,0,0,0);
  for(int i = 0; i < 4000;++i){
    viewPos= worldFragmentMatrix*vec4(texturePos-vec3(0.5,0.5,0.5),1);
    if(!isCut(viewPos.xyz,
              focusWorldPos,
              eyePos)){

  	value = texture(volume, texturePos).x;
    value *= tfScaling;
    vec4 color =  texture(transferfunction, value);
  	
    finalColor.xyz = finalColor.xyz + (1.0f-finalColor.w)*color.xyz;
    finalColor.w = finalColor.w+color.w;
	if(finalColor.w != 0.0){

	}
    if(finalColor.w >= 1.0f){  
      viewPos = viewFragmentMatrix*viewPos;
      outputColor = vec4(finalColor.xyz,viewPos.z);
      outputPosition = vec4(texturePos,viewPos.z);
      //outputColor = vec4(texturePos,viewPos.z);
      i = 1000;
      break;
    }
  }
    texturePos += rayDir/800.0f;

  	//early ray termination
  	if(texturePos.x > 1.0f || texturePos.y > 1.0f || texturePos.z > 1.0f ||
  		texturePos.x < 0.0f || texturePos.y < 0.0f || texturePos.z < 0.0f){
  	i = 1000;
  		break;
  	}
  }
  
}
