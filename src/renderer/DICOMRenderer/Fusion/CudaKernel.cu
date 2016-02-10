#include <stdio.h>
#include <iostream>
#include <vector>

#include <cuda_runtime.h>
#include "cublas_v2.h"

#include "CudaKernel.h"


#define cudaCheckErrors(msg) \
    do { \
        cudaError_t __err = cudaGetLastError(); \
        if (__err != cudaSuccess) { \
            fprintf(stderr, "Fatal error: %s (%s at %s:%d)\n", \
                msg, cudaGetErrorString(__err), \
                __FILE__, __LINE__); \
            fprintf(stderr, "*** FAILED - ABORTING\n"); \
        } \
    } while (0)

texture<float, 3,cudaReadModeElementType>      CTtex;
cudaArray                       *d_volumeArray = 0;
texture<float, 3,cudaReadModeElementType>      MRtex;
cudaArray                       *d_volumeArrayMR = 0;
float*                          device_matrix_ptr = 0;
int                             device_matrix_count = 0;
float*                          device_result = 0;
std::vector<float>              host_result;

void initCuda(){
    cudaFree(0);
}

struct vec{
    float x;
    float y;
    float z;
    float w;
};

void generateCudaTexture(unsigned short* hostdata, int x, int y, int z, bool CT){
    const cudaExtent extend = make_cudaExtent(x, y, z);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();

    std::vector<float> dataasFloat;
    for(int i = 0; i < x*y*z;++i){
        dataasFloat.push_back((float)hostdata[i]);
    }

    if(CT){
        cudaMalloc3DArray(&d_volumeArray, &channelDesc, extend);
        cudaCheckErrors("cudaMalloc3D error");

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloat[0]), extend.width*sizeof(float), extend.width, extend.height);
        copyParams.dstArray = d_volumeArray;
        copyParams.extent   = extend;
        copyParams.kind     = cudaMemcpyHostToDevice;
        cudaMemcpy3D(&copyParams);
        cudaCheckErrors("cudaMemcpy3D fail");

        CTtex.normalized = false;
        CTtex.filterMode = cudaFilterModeLinear;
        CTtex.addressMode[0] = cudaAddressModeClamp;
        CTtex.addressMode[1] = cudaAddressModeClamp;
        CTtex.addressMode[2] = cudaAddressModeClamp;
        CTtex.normalized = true;

        cudaBindTextureToArray(CTtex, d_volumeArray, channelDesc);
        cudaCheckErrors("bind fail");
    }else{
        cudaMalloc3DArray(&d_volumeArrayMR, &channelDesc, extend);
        cudaCheckErrors("cudaMalloc3D error");

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloat[0]), extend.width*sizeof(float), extend.width, extend.height);
        copyParams.dstArray = d_volumeArrayMR;
        copyParams.extent   = extend;
        copyParams.kind     = cudaMemcpyHostToDevice;
        cudaMemcpy3D(&copyParams);
        cudaCheckErrors("cudaMemcpy3D fail");

        MRtex.normalized = false;
        MRtex.filterMode = cudaFilterModeLinear;
        MRtex.addressMode[0] = cudaAddressModeClamp;
        MRtex.addressMode[1] = cudaAddressModeClamp;
        MRtex.addressMode[2] = cudaAddressModeClamp;
        MRtex.normalized = true;

        cudaBindTextureToArray(MRtex, d_volumeArrayMR, channelDesc);
        cudaCheckErrors("bind fail");
    }
}

inline __device__ float3 mul(float3* p, float* m){
    float3 v;
    v.x = p->x*m[0]+p->y*m[4]+p->z*m[8]+1*m[12]+0.5f;
    v.y = p->x*m[1]+p->y*m[5]+p->z*m[9]+1*m[13]+0.5f;
    v.z = p->x*m[2]+p->y*m[6]+p->z*m[10]+1*m[14]+0.5f;
    return v;
}


__global__ void test(float* result, float* matrixptr){
    float3 ctPos;
    ctPos.x = (float)blockIdx.x/511.0f*2.0f;
    ctPos.y = (float)threadIdx.x/511.0f*2.0f;
    ctPos.z = (float)blockIdx.z/167.0f*2.0f;

    float valueCT = tex3D(CTtex,ctPos.x,ctPos.y,ctPos.z);
    valueCT= valueCT/4095.0f;
    if(valueCT >= 0.6f){
       valueCT = 0;
    }
    float valueMR = 0;

    ctPos.x -= 0.5f;
    ctPos.y -= 0.5f;
    ctPos.z -= 0.5f;

    float3 mrPos = mul(&ctPos, &matrixptr[blockIdx.y*16]);
    if(mrPos.x >= 0.0f && mrPos.x <= 1.0f &&
       mrPos.y >= 0.0f && mrPos.y <= 1.0f &&
       mrPos.z >= 0.0f && mrPos.z <= 1.0f){
       valueMR = tex3D(MRtex,mrPos.x,mrPos.y,mrPos.z);
       valueMR = valueMR/1019.0f;
    }

    float dif = ((valueCT-valueMR)*(valueCT-valueMR));
    int index = blockIdx.x + 512*blockIdx.y;
    atomicAdd(&result[index],dif);
}


void setMatrixVector(float* matrix, int matrixcount){
    if(device_matrix_ptr == 0 || device_matrix_count != matrixcount){
        cudaFree(device_matrix_ptr);
        cudaMalloc((void**) &device_matrix_ptr, sizeof(float)*16*matrixcount);
    }
    cudaMemcpy(device_matrix_ptr,matrix,sizeof(float)*16*matrixcount,cudaMemcpyHostToDevice);
    device_matrix_count = matrixcount;
}

void setEmptyResultVector(int size){
    if(device_result == 0 || size != host_result.size())
        cudaMalloc((void**) &device_result, sizeof(float)*size);

    host_result.clear();
    host_result.resize(size);

    cudaMemcpy(device_result, &host_result[0], sizeof(float)*size, cudaMemcpyHostToDevice);
}

const std::vector<double>  subtractVolume(int x, int y, int z){
    dim3 grid(x/2,device_matrix_count,z/2);
    dim3 threadBlock(y/2,1,1);

    float* host_sumresult = new float[x*device_matrix_count];
    float* device_sumresult;
    memset(host_sumresult,0,sizeof(float)*x*device_matrix_count);
    cudaMalloc((void**) &device_sumresult, sizeof(float)*x*device_matrix_count);
    cudaMemcpy(device_sumresult,host_sumresult,sizeof(float)*x*device_matrix_count, cudaMemcpyHostToDevice);

    test<<<grid,threadBlock>>>(device_sumresult,device_matrix_ptr);
    cudaDeviceSynchronize();
    cudaCheckErrors("kernel fail");

    cudaMemcpy(host_sumresult, device_sumresult, sizeof(float)*x*device_matrix_count, cudaMemcpyDeviceToHost);

    host_result.clear();

    std::vector<double> maxValues;
    maxValues.resize(device_matrix_count);
    for(int i = 0; i < x*device_matrix_count;++i){
        maxValues[i/x] += host_sumresult[i];
    }

   /* for(double d : maxValues){
        std::cout << " val : "<< d << std::endl;
    }*/

    return maxValues;
}
