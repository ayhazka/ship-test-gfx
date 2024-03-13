#include <stdint.h>
#include "libultra.h"
#include "PcMain.h"
#include "core/bridge/resourcebridge.h"

#if 0
extern "C" void gSPSegment(void* value, int segNum, uintptr_t target) {
    /*
    char* imgData = (char*)target;

    int res = OTRSigCheck(imgData);

    if (res)
        target = (uintptr_t)LoadTexOrDListByName(imgData);

    __gSPSegment(value, segNum, target);
    */
    gSPNoOp(value);
}

extern "C" void gDPSetTextureImage(Gfx* pkt, u32 format, u32 size, u32 width, void* i) {
    __gDPSetTextureImage(pkt, format, size, width, i);
}

extern "C" void gDPSetTextureImageFB(Gfx* pkt, u32 format, u32 size, u32 width, int fb)
{
    __gDPSetTextureImageFB(pkt, format, size, width, fb);
}

extern "C" void gSPDisplayList(Gfx* pkt, Gfx* dl) {
    char* imgData = (char*)dl;

    if (OTRSigCheck(imgData) == 1)
        dl = LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl);
}

extern "C" void gSPDisplayListOffset(Gfx* pkt, Gfx* dl, int offset) {
    char* imgData = (char*)dl;

    if (OTRSigCheck(imgData) == 1)
        dl = LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl + offset);
}

extern "C" void gSPVertex(Gfx* pkt, void* v, int n, int v0) {
    if (OTRSigCheck((char*)v) == 1)
        v = (Vtx*)GetResourceDataByName((char*)v,  false);

    __gSPVertex(pkt, v, n, v0);
}

extern "C" void gSPInvalidateTexCache(Gfx* pkt, void* texAddr)
{
    char* imgData = (char*)texAddr;
    
    if (texAddr != 0 && OTRSigCheck(imgData)) {
        texAddr = LoadTexByName(imgData); 
    }

    __gSPInvalidateTexCache(pkt, (uintptr_t)texAddr);
}
#endif
