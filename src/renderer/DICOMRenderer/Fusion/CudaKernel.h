#ifndef CUDAKernel
#define CUDAKernel

void initCuda();

void generateCudaTexture(unsigned short* hostdata, int x, int y, int z, bool CT);

void setMatrixVector(float* matrix, int matrixcount);

void setSizeFaktor(float sf);

const std::vector<float>& subtractVolume(int x, int y, int z);


#endif CUDAKernel
