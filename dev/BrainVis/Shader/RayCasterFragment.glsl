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

uniform float   MRIValue = 0.0f;
uniform vec3   MRIPosition = vec3(0,0,0);

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


bool isPlaneCut(vec3 curPos, vec3 focusPos, vec3 normal){
  vec3 dirPos = curPos-focusPos;
  dirPos = normalize(dirPos);
  normal = normalize(normal);
  float dotValue = dot(dirPos,normal);
  if(dotValue > 0.0){
    return true;
  }
  return false;
}

bool isCubicCut(vec3 curPos, vec3 focusPos, vec3 start){
  vec3 dirEye = start-focusPos;
  vec3 dirPos = curPos-focusPos;
  dirEye = normalize(dirEye);

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
  bool isCut = false;

  vec4 targetPos = worldFragmentMatrix*vec4(MRIPosition-vec3(0.5,0.5,0.5),1);
  
  for(int i = 0; i < 4000;++i){
    viewPos= worldFragmentMatrix*vec4(texturePos-vec3(0.5,0.5,0.5),1);

    switch(cutMode){
      case 0 : isCut = false;     //no cut
                       break;
      case 1 : isCut = isCubicCut( viewPos.xyz,     //cubic cut
                              focusWorldPos,
                              eyePos);
                        break;
      case 2 : isCut = isPlaneCut( viewPos.xyz,     //plane cut
                              focusWorldPos,
                              cutPlaneNormal);
                        break;
    }

    if(!isCut){

        	value = texture(volume, texturePos).x;
          value *= tfScaling;
          float mrivaluedelta = abs((texture(volume, texturePos).x * 65535.0f)-MRIValue);
          vec4 color =  texture(transferfunction, value);


          //work in progress
          /*if(length(abs(targetPos-viewPos)) < 3){
            if(mrivaluedelta < 20){
              finalColor.xyz = finalColor.xyz + vec3(value,value*1.5f,value)*(1.0-finalColor.w)*color.w;
              finalColor.w = finalColor.w+color.w;
            }else{
              finalColor.xyz = finalColor.xyz + vec3(value,value,value)*(1.0-finalColor.w)*color.w;
              finalColor.w = finalColor.w+color.w;
            }
          }
        	*/
          finalColor.xyz = finalColor.xyz + vec3(value,value,value)*(1.0-finalColor.w)*color.w;
          finalColor.w = finalColor.w+color.w;
          

          if(finalColor.w >= 1.0f){  
            viewPos = viewFragmentMatrix*viewPos;
            
            outputColor = vec4(finalColor.xyz,viewPos.z);

            outputPosition = vec4(texturePos,viewPos.z);

            i = 1000;
            break;
          }
    }
    texturePos += rayDir/(2*stepSize);

  	//early ray termination
  	if(texturePos.x > 1.0f || texturePos.y > 1.0f || texturePos.z > 1.0f ||
  		texturePos.x < 0.0f || texturePos.y < 0.0f || texturePos.z < 0.0f){
  	   i = 20000;
  		break;
  	}
  }
  
}
