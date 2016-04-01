#ifndef CUDAKernel
#define CUDAKernel

void initCuda();

void generateCudaTexture(unsigned short* hostdata, int x, int y, int z, bool CT);

void initDevice(int x, int y, int z);

const std::vector<float>& step(int x, int y, int z,float* matrix);

float sumReduce(int x, int y, int z);

#endif CUDAKernel
