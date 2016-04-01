#ifndef DICOMENUMS
#define DICOMENUMS
    enum DICOMClipMode{
        none = 0,
        CubicCut,
        PlaneAuto,
        PlaneX,
        PlaneY,
        PlaneZ,
        PlaneXn,
        PlaneYn,
        PlaneZn,
        PlaneManual
    };

    enum RenderMode{
        ThreeDMode = 0,
        XAxis,
        YAxis,
        ZAxis
    };

    enum FusionMode{
        CPU = 0,
        OpenGL,
        CUDA
    };
#endif
