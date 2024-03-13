#pragma once
#include "libultra.h"
#include <string.h>
#include <stdarg.h>

void __osPiCreateAccessQueue(void);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
int32_t osSendMesg(OSMesgQueue* mq, OSMesg mesg, int32_t flag);
void osStopThread(OSThread* thread);
void osViExtendVStart(u32 arg0);
int32_t osRecvMesg(OSMesgQueue* mq, OSMesg* msg, int32_t flag);
int32_t osContStartReadData(OSMesgQueue* mesg);
void osContGetReadData(OSContPad* pad);
s32 osContSetCh(u8 ch);
int32_t osContInit(OSMesgQueue* mq, uint8_t* controllerBits, OSContStatus* status);
void __osInitialize_common(void);
void __osInitialize_autodetect(void);
void __osExceptionPreamble();
void __osEnqueueAndYield(OSThread**);
void __osEnqueueThread(OSThread**, OSThread*);
OSThread* __osPopThread(OSThread**);
void __osDispatchThread();
void __osCleanupThread(void);
void __osDequeueThread(OSThread** queue, OSThread* thread);
void osDestroyThread(OSThread* thread);
void osCreateThread(OSThread* thread, OSId id, void (*entry)(void*), void* arg, void* sp, OSPri pri);
void __osSetSR(u32);
u32 __osGetSR();
void osWritebackDCache(void* vaddr, s32 nbytes);
void* osViGetNextFramebuffer(void);
void osCreatePiManager(OSPri pri, OSMesgQueue* cmdQ, OSMesg* cmdBuf, s32 cmdMsgCnt);
void __osDevMgrMain(void* arg);
s32 __osPiRawStartDma(s32 dir, u32 cartAddr, void* dramAddr, size_t size);
//u32 osVirtualToPhysical(void* vaddr);
#define osVirtualToPhysical(vaddr) ((uintptr_t)(void*)vaddr)
void osViBlack(u8 active);
s32 __osSiRawReadIo(void* devAddr, u32* dst);
OSId osGetThreadId(OSThread* thread);
void osViSetMode(OSViMode* mode);
u32 __osProbeTLB(void*);
u32 osGetMemSize(void);
void osSetEventMesg(OSEvent e, OSMesgQueue* mq, OSMesg msg);
s32 _Printf(PrintCallback, void* arg, const char* fmt, va_list ap);
void osUnmapTLBAll(void);
s32 osEPiStartDma(OSPiHandle* handle, OSIoMesg* mb, s32 direction);
void osInvalICache(void* vaddr, s32 nbytes);
s32 osContStartQuery(OSMesgQueue* mq);
void osContGetQuery(OSContStatus* data);
void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msg, s32 count);
void osInvalDCache(void* vaddr, s32 nbytes);
s32 __osSiDeviceBusy(void);
s32 osJamMesg(OSMesgQueue* mq, OSMesg mesg, s32 flag);
void osSetThreadPri(OSThread* thread, OSPri pri);
OSPri osGetThreadPri(OSThread* thread);
s32 __osEPiRawReadIo(OSPiHandle* handle, u32 devAddr, u32* data);
void osViSwapBuffer(void* vaddr);
s32 __osEPiRawStartDma(OSPiHandle* handle, s32 direction, u32 cartAddr, void* dramAddr, size_t size);
void __osTimerServicesInit(void);
void __osTimerInterrupt(void);
void __osSetTimerIntr(OSTime time);
OSTime __osInsertTimer(OSTimer* timer);
#ifndef __cplusplus
void __osSetGlobalIntMask(OSHWIntr mask);
#endif
void __osSetCompare(u32);
#ifndef __cplusplus
void __osResetGlobalIntMask(OSHWIntr mask);
#endif
s32 __osDisableInt(void);
void __osRestoreInt(s32);
void __osViInit(void);
void __osViSwapContext(void);
OSMesgQueue* osPiGetCmdQueue(void);
s32 osEPiReadIo(OSPiHandle* handle, u32 devAddr, u32* data);
void osViSetSpecialFeatures(u32 func);
OSPiHandle* osCartRomInit(void);
void __osSetFpcCsr(u32);
u32 __osGetFpcCsr();
s32 osEPiWriteIo(OSPiHandle* handle, u32 devAddr, u32 data);
void osMapTLBRdb(void);
void osYieldThread(void);
u32 __osGetCause();
s32 __osEPiRawWriteIo(OSPiHandle* handle, u32 devAddr, u32 data);
s32 __osSiRawWriteIo(void* devAddr, u32 val);
void osCreateViManager(OSPri pri);
OSViContext* __osViGetCurrentContext(void);
void osStartThread(OSThread* thread);
void osViSetYScale(f32 scale);
void osViSetXScale(f32 value);
void __osSetWatchLo(u32);
void osSpTaskLoad(OSTask* task);
void osSpTaskStartGo(OSTask* task);
void osSpTaskYield(void);
OSYieldResult osSpTaskYielded(OSTask* task);
void osWritebackDCacheAll(void);
s32 osAiSetNextBuffer(void* buf, size_t size);
s32 osAiSetFrequency(u32 frequency);
u32 osAiGetLength(void);

// GbiWrap.cpp
void gSPSegment(void* value, int segNum, uintptr_t target);
void gDPSetTextureImage(Gfx* pkt, u32 f, u32 s, u32 w, void* i);
void gSPDisplayList(Gfx* pkt, Gfx* dl);
void gSPDisplayListOffset(Gfx* pkt, Gfx* dl, int offset);
void gSPVertex(Gfx* pkt, void* v, int n, int v0);
void gSPInvalidateTexCache(Gfx* pkt, void* texAddr);

#define EEPROM_TYPE_4K    0x01
#define EEPROM_TYPE_16K   0x02
#define EEPROM_BLOCK_SIZE 8
#define EEPROM_MAXBLOCKS  64

extern u32 osResetType;
extern u32 osTvType;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeMpalLan1;
extern OSViMode osViModeFpalLan1;
extern OSViMode osViModePalLan1;
extern u8 osAppNmiBuffer[];


// os_message.h macros

/* Get count of valid messages in queue */
#define MQ_GET_COUNT(mq)        ((mq)->validCount)

/* Figure out if message queue is empty or full */
#define MQ_IS_EMPTY(mq)		(MQ_GET_COUNT(mq) == 0)
#define MQ_IS_FULL(mq)		(MQ_GET_COUNT(mq) >= (mq)->msgCount)


// some bcopy/bzero/bcmp macros

#define bcopy(s, d, n) memcpy(d, s, n)
#define bzero(s, n) memset(s, 0, n)
#define bcmp(s1, s2, n) memcmp(s1, s2, n)
