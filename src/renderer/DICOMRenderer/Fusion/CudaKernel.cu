#include <stdio.h>
#include <iostream>
#include <vector>

#include <cuda_runtime.h>
#include "cublas_v2.h"

#include "CudaKernel.h"

//GLOBAL Varaibles , jeah there is no class here deal with it right now :(
texture<float, 3,cudaReadModeElementType>       CTtex;
cudaArray                                       *d_volumeArray = 0;
float                                           CTmaxValue = 0;

texture<float, 3,cudaReadModeElementType>       MRtex;
cudaArray                                       *d_volumeArrayMR = 0;
float                                           MRmaxValue = 0;
float*                                          device_matrix_ptr = 0;
int                                             device_matrix_count = 0;

float*                                          host_result = NULL;
float*                                          device_result = NULL;

static float                                    sizefaktor = 2.0f;

void initCuda(){
    cudaFree(0);
}

void generateCudaTexture(unsigned short* hostdata, int x, int y, int z, bool CT){
    const cudaExtent extend = make_cudaExtent(x, y, z);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();

    std::vector<float> dataasFloat;
    float max = 0;
    for(int i = 0; i < x*y*z;++i){
        dataasFloat.push_back((float)hostdata[i]);
        if(max < dataasFloat[i]){
            max = dataasFloat[i];
        }
    }

    if(CT){
        for(int i = 0; i < x*y*z;++i){
            dataasFloat[i] = dataasFloat[i]/max;
            if(dataasFloat[i] > 0.5f){
                dataasFloat[i] = 0;
            }
        }


        cudaMalloc3DArray(&d_volumeArray, &channelDesc, extend);

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloat[0]), extend.width*sizeof(float), extend.width, extend.height);
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
    }else{
        for(int i = 0; i < x*y*z;++i){
            dataasFloat[i] = dataasFloat[i]/max;
        }

        cudaMalloc3DArray(&d_volumeArrayMR, &channelDesc, extend);

        cudaMemcpy3DParms copyParams = {0};
        copyParams.srcPtr   = make_cudaPitchedPtr((void*)&(dataasFloat[0]), extend.width*sizeof(float), extend.width, extend.height);
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
    }
}

inline __device__ float3 mul(float3* p, float* m){
    float3 v;
    v.x = p->x*m[0]+p->y*m[4]+p->z*m[8]+1*m[12]+0.5f;
    v.y = p->x*m[1]+p->y*m[5]+p->z*m[9]+1*m[13]+0.5f;
    v.z = p->x*m[2]+p->y*m[6]+p->z*m[10]+1*m[14]+0.5f;
    return v;
}


__global__ void subVolumes(float* result,
                           float* matrixptr,
                           float sizefaktor,
                           float xVolDim,
                           float yVolDim,
                           float zVolDim){
    float3 ctPos;
    ctPos.x = (float)blockIdx.x/(xVolDim-1.0f)*sizefaktor;
    ctPos.y = (float)threadIdx.x/(yVolDim-1.0f)*sizefaktor;
    ctPos.z = (float)blockIdx.z/(zVolDim-1.0f)*sizefaktor;

    float valueCT = tex3D(CTtex,ctPos.x,ctPos.y,ctPos.z);
    float valueMR = 0;

    ctPos.x -= 0.5f;
    ctPos.y -= 0.5f;
    ctPos.z -= 0.5f;

    float3 mrPos = mul(&ctPos, &matrixptr[blockIdx.y*16]);
    if(mrPos.x >= 0.0f && mrPos.x <= 1.0f &&
       mrPos.y >= 0.0f && mrPos.y <= 1.0f &&
       mrPos.z >= 0.0f && mrPos.z <= 1.0f){
       valueMR = tex3D(MRtex,mrPos.x,mrPos.y,mrPos.z);
    }

    float dif = (valueCT-valueMR);
    dif = dif*dif;
    int index = blockIdx.x + xVolDim*yVolDim*blockIdx.y + threadIdx.x*xVolDim;
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

void setSizeFaktor(float sf){
    sizefaktor = sf;
}

const std::vector<float>&  subtractVolume(int x, int y, int z){
    dim3 grid(x/sizefaktor,device_matrix_count,z*sizefaktor);
    dim3 threadBlock(y/sizefaktor,1,1);

    if(host_result == NULL)
        host_result = new float[x*y*device_matrix_count];

    if(device_result == NULL)
        cudaMalloc((void**) &device_result, sizeof(float)*x*y*device_matrix_count);

    memset(host_result,0,sizeof(float)*x*y*device_matrix_count);
    cudaMemcpy(device_result,host_result,sizeof(float)*x*y*device_matrix_count, cudaMemcpyHostToDevice);

    subVolumes<<<grid,threadBlock>>>(device_result,device_matrix_ptr,sizefaktor,(float)x,(float)y,(float)z);
    cudaDeviceSynchronize();

    cudaMemcpy(host_result, device_result, sizeof(float)*x*y*device_matrix_count, cudaMemcpyDeviceToHost);

    std::vector<float> maxValues;
    maxValues.resize(device_matrix_count);
    int matsel = 0;
    for(int i = 0; i < x*y*device_matrix_count;++i){
        matsel = i/(x*y);
        maxValues[matsel] += host_result[i];
    }

    return maxValues;
}
