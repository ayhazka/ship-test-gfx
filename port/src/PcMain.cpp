// TODO: reorganize
#include "PcMain.h"
#include <libultraship.h>
#include <graphic/Fast3D/gfx_pc.h>
#include <resource/File.h>

DM64Globals* DM64Globals::Instance;

DM64Globals::DM64Globals() {
	std::vector<std::string> OTRFiles;
	std::string otrPath = LUS::Context::GetPathRelativeToAppDirectory("soh.otr");
	if (std::filesystem::exists(otrPath)) {
		OTRFiles.push_back(otrPath);
	}

	context = LUS::Context::CreateInstance("Simple Test Rendering", "onetri", "config.json", OTRFiles);
}

DM64Globals::~DM64Globals() {

}

extern "C" void InitDM64() {
	DM64Globals::Instance = new DM64Globals();
}

extern "C" void DeinitDM64() {
	
}

extern "C" void StartFrame() {
    gfx_start_frame();
}

extern "C" void GameLoop(void(*run_one_game_iter)(void))
{
    DM64Globals::Instance->context->GetWindow()->MainLoop(run_one_game_iter);
}


extern "C" void ProcessGfxCommands(Gfx* commands) {
	gfx_run(commands, {});
}

extern "C" void EndFrame() {
    gfx_end_frame();
}

#ifdef _WIN32
extern "C" uint64_t GetFrequency() {
    LARGE_INTEGER nFreq;

    QueryPerformanceFrequency(&nFreq);

    return nFreq.QuadPart;
}

extern "C" uint64_t GetPerfCounter() {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);

    return ticks.QuadPart;
}
#else
extern "C" uint64_t GetFrequency() {
    return 1000; // sec -> ms
}

extern "C" uint64_t GetPerfCounter() {
    struct timespec monotime;
    clock_gettime(CLOCK_MONOTONIC, &monotime);

    uint64_t remainingMs = (monotime.tv_nsec / 1000000);

    // in milliseconds
    return monotime.tv_sec * 1000 + remainingMs;
}
#endif


extern "C" void bswapPtrArrays(void * * ptrlist, int count) {
    for (int i = 0; i < count; i++) {
        ptrlist[i] = (void*)BE32SWAP((uintptr_t)ptrlist[i]);
    }
}

extern "C" void bswap16BitsArrays(void* arr, int count) {
    u16* s = (u16*)arr;
    for (int i = 0; i < count; i++) {
        s[i] = BSWAP16(s[i]);
    }
}

extern "C" void bswap32BitsArrays(void* arr, int count) {
    u32* s = (u32*)arr;
    for (int i = 0; i < count; i++) {
        s[i] = BSWAP32(s[i]);
    }
}


extern "C" s32 osEepromProbe(OSMesgQueue * mq) {
    return 1;
}

extern "C" s32 osEepromLongRead(OSMesgQueue * mq, u8 address, u8 * buffer, int nbytes) {
    u8 content[512];
    s32 ret = -1;

    FILE* fp = fopen("dm64_save_file.bin", "rb");
    if (fp == NULL) {
        return -1;
    }
    if (fread(content, 1, 512, fp) == 512) {
        memcpy(buffer, content + address * 8, nbytes);
        ret = 0;
    }
    fclose(fp);
    return ret;
}

extern "C" s32 osEepromLongWrite(OSMesgQueue * mq, u8 address, u8 * buffer, int nbytes) {
    u8 content[512] = { 0 };
    if (address != 0 || nbytes != 512) {
        osEepromLongRead(mq, 0, content, 512);
    }
    memcpy(content + address * 8, buffer, nbytes);

    FILE* fp = fopen("dm64_save_file.bin", "wb");
    if (fp == NULL) {
        return -1;
    }
    s32 ret = fwrite(content, 1, 512, fp) == 512 ? 0 : -1;
    fclose(fp);
    return ret;
}


/*
 *  Functions from SoH
 */

extern "C" int OTRSigCheck(char* imgData)
{
    uintptr_t i = (uintptr_t)(imgData);

    if ((i & 1) == 1)
        return 0;

    if (i != 0) {
        if (imgData[0] == '_' && imgData[1] == '_' && imgData[2] == 'O' && imgData[3] == 'T' && imgData[4] == 'R' &&
            imgData[5] == '_' && imgData[6] == '_')
            return 1;
    }

    return 0;
}

extern "C" Gfx* LoadGfxByName(const char* path)
{
    return (Gfx*)ResourceGetDataByName(path);
}

extern "C" char* LoadTexByName(const char* path)
{
    return (char*)ResourceGetDataByName(path);
}

/*
static std::shared_ptr<Ship::File> LoadFileAndCache(std::string path) {
    auto fileCacheFind = DM64Globals::Instance->fileCache.find(path);
    if (fileCacheFind == DM64Globals::Instance->fileCache.end()) {
        auto file = DM64Globals::Instance->context->GetResourceManager()->LoadFile(path);
        
        if (file == nullptr)
            return nullptr;

        DM64Globals::Instance->fileCache.emplace(path, file);
        return file;
    }
    return fileCacheFind->second;
}

extern "C" size_t GetFileRawSize(const char* resName) {
    return LoadFileAndCache(resName)->Buffer.size();
}

extern "C" char* LoadFileRaw(const char* resName) {
    return LoadFileAndCache(resName)->Buffer.data();
}
*/

void* GetGamePoolStart()
{
    return (void*)DM64Globals::Instance->gamePool;
}

extern "C" uint32_t GetCurrentWidth() {
    return DM64Globals::Instance->context->GetWindow()->GetWidth();
}

extern "C" uint32_t GetCurrentHeight() {
    return DM64Globals::Instance->context->GetWindow()->GetHeight();
}

/*
extern "C" void ControllerCallback(ControllerCallback * controller) {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    for (int i = 0; i < controlDeck->GetNumVirtualDevices(); ++i) {
        auto physicalDevice = controlDeck->GetPhysicalDeviceFromVirtualSlot(i);
        physicalDevice->WriteToSource(i, controller);
    }
}
*/

extern "C" float GetAspectRatio() {
    return gfx_current_dimensions.aspect_ratio;
}

extern "C" float GetDimensionFromLeftEdge(float v) {
    return (SCREEN_WIDTH / 2 - SCREEN_HEIGHT / 2 * GetAspectRatio() + (v));
}

extern "C" float GetDimensionFromRightEdge(float v) {
    return (SCREEN_WIDTH / 2 + SCREEN_HEIGHT / 2 * GetAspectRatio() - (SCREEN_WIDTH - v));
}

f32 floorf(f32 x);
f32 ceilf(f32 x);

extern "C" int16_t GetRectDimensionFromLeftEdge(float v) {
    return ((int)floorf(GetDimensionFromLeftEdge(v)));
}

extern "C" int16_t GetRectDimensionFromRightEdge(float v) {
    return ((int)ceilf(GetDimensionFromRightEdge(v)));
}


extern "C" s32 osAiSetFrequency(u32 freq)
{
    u32 a1;
    s32 a2;
    u32 D_8033491C;

#ifdef PAL_FRAMES
    D_8033491C = 0x02E6025C;
#else
    D_8033491C = 0x02E6D354;
#endif

    a1 = D_8033491C / (float)freq + .5f;

    if (a1 < 0x84) {
        return -1;
    }

    a2 = (a1 / 66) & 0xff;
    if (a2 > 16) {
        a2 = 16;
    }

    return D_8033491C / (s32)a1;
}
