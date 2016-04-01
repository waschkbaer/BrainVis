#include <stdio.h>
#include <iostream>
#include <vector>

#include <cuda_runtime.h>
#include "cublas_v2.h"

#include "CudaKernel.h"

//jeah i should use some classes and stuff, but for first it was much simpler
//for me to just use some global variables
texture<float, 3,cudaReadModeElementType>       CTtex;
cudaArray                                       *d_volumeArray = 0;
float                                           CTmaxValue = 0;

texture<float, 3,cudaReadModeElementType>       MRtex;
cudaArray                                       *d_volumeArrayMR = 0;
float                                           MRmaxValue = 0;
float*                                          device_matrix_ptr = 0;
int                                             device_matrix_count = 0;

float*                                          device_result = NULL;
float*                                          device_output = NULL;
static float                                    sizefaktor = 2.0f;
static bool                                     volumeInitCT = false;
static bool                                     volumeInitMR = false;

void initCuda(){
    cudaFree(0);
}

void generateCudaTexture(unsigned short* hostdata, int x, int y, int z, bool CT){
    if(volumeInitMR && !CT) return;
    if(volumeInitCT && CT) return;
    //if i want to use the full size...
    //i have to use a float buffer since CUDA does not support
    //the linear filter on int/short textures :(

    std::vector<float> dataasFloat;
    float max = 0;
    for(int i = 0; i < x*y*z;++i){
        dataasFloat.push_back((float)hostdata[i]);
        if(max < dataasFloat[i]){
            max = dataasFloat[i];
        }
    }

    std::vector<float> dataasFloatHalf;
    float valu = 0;
    float val = 0;
    for(int pz = 0; pz < z-1; pz+=2){
        for(int py = 0; py < y-1; py+=2){
            for(int px = 0; px < x-1; px+=2){
                //std::cout << px << " "<< py << " "<< pz << std::endl;
                val = hostdata[px + py*x + pz * x* y];
                val += hostdata[px + py*x + (pz+1)* x* y];
                val += hostdata[px + (py+1)*x + pz * x* y];
                val += hostdata[px + (py+1)*x + (pz+1) * x* y];
                val += hostdata[(px+1) + py*x + pz* x* y];
                val += hostdata[(px+1) + py*x + (pz+1)* x* y];
                val += hostdata[(px+1) + (py+1)*x + pz* x* y];
                val += hostdata[(px+1) + (py+1)*x + (pz+1)* x* y];
                val /= 8.0f;
                dataasFloatHalf.push_back(val);
                valu += val;
            }
        }
    }


    //creating the texture, only have the size since i resampled the data
    const cudaExtent extend = make_cudaExtent(x/2, y/2, z/2);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();

    if(CT){
        cudaMalloc3DArray(&d_volumeArray, &channelDesc, extend);

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloatHalf[0]), extend.width*sizeof(float), extend.width, extend.height);
        copyParams.dstArray = d_volumeArray;
        copyParams.extent   = extend;
        copyParams.kind     = cudaMemcpyHostToDevice;
        cudaMemcpy3D(&copyParams);

        CTtex.normalized = false;
        CTtex.filterMode = cudaFilterModePoint;
        CTtex.addressMode[0] = cudaAddressModeClamp;
        CTtex.addressMode[1] = cudaAddressModeClamp;
        CTtex.addressMode[2] = cudaAddressModeClamp;
        CTtex.normalized = true;

        cudaBindTextureToArray(CTtex, d_volumeArray, channelDesc);
        CTmaxValue = max;
        std::cout << "CT TEXTURE VALUE "<< (int)valu << std::endl;
        volumeInitCT = true;
    }else{
        cudaMalloc3DArray(&d_volumeArrayMR, &channelDesc, extend);

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloatHalf[0]), extend.width*sizeof(float), extend.width, extend.height);
        copyParams.dstArray = d_volumeArrayMR;
        copyParams.extent   = extend;
        copyParams.kind     = cudaMemcpyHostToDevice;
        cudaMemcpy3D(&copyParams);

        MRtex.normalized = false;
        MRtex.filterMode = cudaFilterModePoint;
        MRtex.addressMode[0] = cudaAddressModeClamp;
        MRtex.addressMode[1] = cudaAddressModeClamp;
        MRtex.addressMode[2] = cudaAddressModeClamp;
        MRtex.normalized = true;

        cudaBindTextureToArray(MRtex, d_volumeArrayMR, channelDesc);
        MRmaxValue = max;
        std::cout << "MR TEXTURE VALUE "<< (int)valu << std::endl;
        volumeInitMR = true;
    }

}

//DEVICE CODE --------------------------------------------------------
__global__ void substractCTandMR(float*     result,
                                 const float      maxX,
                                 const float      maxY,
                                 const float      maxZ,
                                 const float      CTMax,
                                 const float      MRMax,
                                 const float      m11,
                                 const float      m12,
                                 const float      m13,
                                 const float      m14,
                                 const float      m21,
                                 const float      m22,
                                 const float      m23,
                                 const float      m24,
                                 const float      m31,
                                 const float      m32,
                                 const float      m33,
                                 const float      m34){
    __shared__ float sdata[512];

    unsigned int x = (blockIdx.x * blockDim.x) + threadIdx.x;
    unsigned int y = (blockIdx.y * blockDim.y) + threadIdx.y;
    unsigned int z = (blockIdx.z * blockDim.z) + threadIdx.z;


    float3 ctPosition;
    ctPosition.x = (float)(x)/maxX;
    ctPosition.y = (float)(y)/maxY;
    ctPosition.z = (float)(z)/maxZ;

    //read the value
    float valueCT = tex3D(CTtex,ctPosition.x,ctPosition.y,ctPosition.z);
    float valueMR = 0.0f;

    valueCT = valueCT/CTMax;
    if(valueCT > 0.5f )valueCT = 0;

    ctPosition.x -= 0.5f;
    ctPosition.y -= 0.5f;
    ctPosition.z -= 0.5f;

    unsigned long index = 0;
    float subresult = 0;
    float3 mrPosition;

    mrPosition.x = ctPosition.x*m11+ctPosition.y*m12+ctPosition.z*m13+1*m14+0.5f;
    mrPosition.y = ctPosition.x*m21+ctPosition.y*m22+ctPosition.z*m23+1*m24+0.5f;
    mrPosition.z = ctPosition.x*m31+ctPosition.y*m32+ctPosition.z*m33+1*m34+0.5f;

    if(mrPosition.x >= 0.0f && mrPosition.x <= 1.0f &&
       mrPosition.y >= 0.0f && mrPosition.y <= 1.0f &&
       mrPosition.z >= 0.0f && mrPosition.z <= 1.0f){

        //get value and normalize it
       valueMR = tex3D(MRtex,mrPosition.x,mrPosition.y,mrPosition.z);
       valueMR = valueMR/MRMax;
    }

    index = 0;
    index += z*(unsigned int)(maxX*maxY);
    index += y*(unsigned int)(maxX) + x;

    subresult = (valueMR-valueCT)*(valueMR-valueCT);

    //test use the thread reduction of the block to reduce the global memory acces
    int tid = threadIdx.z * blockDim.x *blockDim.y + threadIdx.y * blockDim.x + threadIdx.x; // calculate the 1d index
    sdata[tid] = subresult;

    __syncthreads();
    for (unsigned int s=blockDim.x*blockDim.y*blockDim.z/2; s>0; s>>=1) {
    if (tid < s) {
    sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
    }
    if(tid == 0){
        index = blockIdx.z * 32 *32 + blockIdx.y * 32+ blockIdx.x;
        result[index] =sdata[0];
    }
    //result[index] = subresult;
}


__global__ void reduce(float* g_idata, float* g_odata){
    __shared__ float sdata[512];

    // each thread loads one element from global to shared mem
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
    sdata[tid] = g_idata[i] + g_idata[i+blockDim.x];
    __syncthreads();
    // do reduction in shared mem
    for (unsigned int s=blockDim.x/2; s>0; s>>=1) {
    if (tid < s) {
    sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
    }
    if(tid == 0){
        g_odata[blockIdx.x] = sdata[0];
    }
}


// HOST CODE -----------------------------------------------------------------------


static int count = 13;
static std::vector<float> result_vector;
void initDevice(int x, int y, int z){
    if(device_result == NULL)
        cudaMalloc((void**) &device_result, sizeof(float)*x*y*z/512);

    if(device_output == NULL)
        cudaMalloc((void**) &device_output, sizeof(float)*x*y*z/512);

    result_vector.resize(count);
}

float sumReduce(int x, int y, int z){
    int arraySize = x*y*z;
    float* temp = NULL;
    const int threads = 512;

    while(arraySize >= threads){
        arraySize = arraySize/threads/2;
        reduce<<<arraySize,threads>>>(device_result,device_output);
        temp = device_output;
        device_output = device_result;
        device_result = temp;
    }
    float* result = new float[arraySize];
    cudaMemcpy(&(result[0]), temp, sizeof(float)*arraySize, cudaMemcpyDeviceToHost);

    float resultVal = 0;
    for(int i = 0; i < arraySize;++i){
        resultVal += result[i];
    }
    return resultVal;
}


const std::vector<float>& step(int x, int y, int z,float* matrix){


    dim3 threadsPerBlock(8, 8, 8);
    dim3 numBlocks( x/threadsPerBlock.x,
                    y/threadsPerBlock.y,
                    z/threadsPerBlock.z);

    int matInd = 0;
    for(int i = 0; i < count;++i){
        matInd = i*16;
        substractCTandMR<<<numBlocks,threadsPerBlock>>>(device_result,
                                                        (float)x,
                                                        (float)y,
                                                        (float)z,
                                                        CTmaxValue,
                                                        MRmaxValue,
                                                        matrix[matInd],matrix[matInd+4],matrix[matInd+8],matrix[matInd+12],
                                                        matrix[matInd+1],matrix[matInd+5],matrix[matInd+9],matrix[matInd+13],
                                                        matrix[matInd+2],matrix[matInd+6],matrix[matInd+10],matrix[matInd+14]);
        cudaDeviceSynchronize();
        result_vector[i] = sumReduce(x/8,y/8,z/8);
        cudaDeviceSynchronize();
    }

    return result_vector;
}

