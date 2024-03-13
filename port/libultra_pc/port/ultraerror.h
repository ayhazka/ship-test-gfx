#ifndef __ULTRAERROR_H__
#define __ULTRAERROR_H__

#include <libultra/types.h>

/* OS error codes (native port only) */

#define ERR_ALSEQP_NO_SOUND             0
#define ERR_ALSEQP_NO_VOICE             1
#define ERR_ALSEQP_MAP_VOICE            2
#define ERR_ALSEQP_OFF_VOICE            3
#define ERR_ALSEQP_POLY_VOICE           4
#define ERR_ALSNDP_NO_VOICE             5
#define ERR_ALSYN_NO_UPDATE             6
#define ERR_ALSNDPDEALLOCATE            7
#define ERR_ALSNDPDELETE                8
#define ERR_ALSNDPPLAY                  9
#define ERR_ALSNDPSETSOUND              10
#define ERR_ALSNDPSETPRIORITY           11
#define ERR_ALSNDPSETPAR                12
#define ERR_ALBNKFNEW                   13
#define ERR_ALSEQNOTMIDI                14
#define ERR_ALSEQNOTMIDI0               15
#define ERR_ALSEQNUMTRACKS              16
#define ERR_ALSEQTIME                   17
#define ERR_ALSEQTRACKHDR               18
#define ERR_ALSEQSYSEX                  19
#define ERR_ALSEQMETA                   20
#define ERR_ALSEQPINVALIDPROG           21
#define ERR_ALSEQPUNKNOWNMIDI           22
#define ERR_ALSEQPUNMAP                 23
#define ERR_ALEVENTNOFREE               24
#define ERR_ALHEAPNOFREE                25
#define ERR_ALHEAPCORRUPT               26
#define ERR_ALHEAPFIRSTBLOCK            27
#define ERR_ALCSEQZEROSTATUS            28
#define ERR_ALCSEQZEROVEL               29
#define ERR_ALCSPVNOTFREE               30
#define ERR_ALSEQOVERRUN                31
#define ERR_ALMODDELAYOVERFLOW          32

#define __osError(errcode, nargs, ...) __osErrorFile(__FILE__, __LINE__, errcode, nargs, ##__VA_ARGS__)
extern void		__osErrorFile(const char*, s32, s16, s32, ...);

#endif
