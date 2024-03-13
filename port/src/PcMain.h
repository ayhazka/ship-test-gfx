#pragma once

#include <libultraship.h>

#ifdef __cplusplus
class DM64Globals
{
public:
	static DM64Globals* Instance;

	std::shared_ptr<LUS::Context> context;
	std::map<std::string, std::shared_ptr<Ship::File>> fileCache;
	u64 gamePool[0x10000000 / sizeof(u64)];

	DM64Globals();
	~DM64Globals();

private:
};

extern "C" {
#endif
	void InitDM64();
	void DeinitDM64();
	void StartFrame();
	void GameLoop(void (*run_one_game_iter)(void));
	void ProcessGfxCommands(Gfx* commands);
	void EndFrame();
	uint64_t GetFrequency();
	uint64_t GetPerfCounter();

	void bswapPtrArrays(void** ptrlist, int count);
	void bswap16BitsArrays(void* arr, int count);
	void bswap32BitsArrays(void* arr, int count);

	int OTRSigCheck(char* imgData);
	Gfx* LoadGfxByName(const char* path);
	char* LoadTexByName(const char* path);
	size_t GetFileRawSize(const char* resName);
	char* LoadFileRaw(const char* resName);
	void* GetGamePoolStart();
	uint32_t GetCurrentWidth();
	uint32_t GetCurrentHeight();
	float GetAspectRatio();
	float GetDimensionFromLeftEdge(float v);
	float GetDimensionFromRightEdge(float v);
	int16_t GetRectDimensionFromLeftEdge(float v);
	int16_t GetRectDimensionFromRightEdge(float v);
#ifdef __cplusplus
};
#endif
