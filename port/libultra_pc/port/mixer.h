#ifndef MIXER_H
#define MIXER_H

#include "libultra/abi.h"
#include <stdint.h>
#include "libultra.h"

#undef aSegment
#undef aClearBuffer
#undef aSetBuffer
#undef aLoadBuffer
#undef aSaveBuffer
#undef aDMEMMove
#undef aMix
#undef aEnvMixer
#undef aResample
#undef aInterleave
#undef aSetVolume
#undef aSetVolume32
#undef aSetLoop
#undef aLoadADPCM
#undef aADPCMdec
#undef aPoleFilter

void aClearBufferImpl(uint16_t addr, int nbytes);
void aLoadBufferImpl(const void *source_addr);
void aSaveBufferImpl(int16_t *dest_addr);
void aLoadADPCMImpl(int num_entries_times_16, const int16_t *book_source_addr);
void aSetBufferImpl(uint8_t flags, uint16_t in, uint16_t out, uint16_t nbytes);
void aSetVolumeImpl(uint8_t flags, int16_t v, int16_t t, int16_t r);
void aInterleaveImpl(uint16_t left, uint16_t right);
void aDMEMMoveImpl(uint16_t in_addr, uint16_t out_addr, int nbytes);
void aSetLoopImpl(ADPCM_STATE *adpcm_loop_state);
void aADPCMdecImpl(uint8_t flags, ADPCM_STATE state);
void aResampleImpl(uint8_t flags, uint16_t pitch, RESAMPLE_STATE state);
void aEnvMixerImpl(uint8_t flags, ENVMIX_STATE state);
void aMixImpl(int16_t gain, uint16_t in_addr, uint16_t out_addr);
void aPoleFilterImpl(uint8_t flags, int16_t gain, POLEF_STATE state);

#define aSegment(pkt, s, b) pkt
#define aClearBuffer(pkt, d, c) {aClearBufferImpl(d, c); pkt;}
#define aLoadBuffer(pkt, s) {aLoadBufferImpl((void*)(s)); pkt;}
#define aSaveBuffer(pkt, s) {aSaveBufferImpl((void*)(s)); pkt;}
#define aLoadADPCM(pkt, c, d) {aLoadADPCMImpl(c, (void*)(d)); pkt;}
#define aSetBuffer(pkt, f, i, o, c) {aSetBufferImpl(f, i, o, c); pkt;}
#define aSetVolume(pkt, f, v, t, r) {aSetVolumeImpl(f, v, t, r); pkt;}
#define aSetVolume32(pkt, f, v, tr) {aSetVolume(pkt, f, v, (int16_t)((tr) >> 16), (int16_t)(tr)); pkt;}
#define aInterleave(pkt, l, r) {aInterleaveImpl(l, r); pkt;}
#define aDMEMMove(pkt, i, o, c) {aDMEMMoveImpl(i, o, c); pkt;}
#define aSetLoop(pkt, a) {aSetLoopImpl(a); pkt;}
#define aADPCMdec(pkt, f, s) {aADPCMdecImpl(f, (short*)(s)); pkt;}
#define aResample(pkt, f, p, s) {aResampleImpl(f, p, (short*)(s)); pkt;}
#define aEnvMixer(pkt, f, s) {aEnvMixerImpl(f, (short*)(s)); pkt;}
#define aMix(pkt, f, g, i, o) {aMixImpl(g, i, o); pkt;}
#define aPoleFilter(pkt, f, g, s) {aPoleFilterImpl(f, g, (short*)(s)); pkt;}

#endif
