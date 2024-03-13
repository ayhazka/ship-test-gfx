#include <libultra.h>
#include <stdio.h>
#include <stdarg.h>
#include <luslog.h>

#ifdef _DEBUG

static const char *error_strings[] = {
    "_handleMIDIMsg: no sound mapped",
    "_handleMIDIMsg: no free voices",
    "_handleMIDIMsg: couldn't map voice",
    "_handleMIDIMsg: note off - couldn't find voice",
    "_handleMIDIMsg: poly pressure - couldn't find voice",
    "_handleEvent: no free voices",
    "Synthesizer: no free updates",
    "alSndPDeallocate: attempt to deallocate a sound which is playing",
    "alSndpDelete: attempt to delete player with playing sounds",
    "alSndpPlay: attempt to play a sound which is playing",
    "alSndpSetSound: sound id (%d) out of range (0 - %d)",
    "alSndpSetPriority: sound id (%d) out of range (0 - %d)",
    "alSndpSet Parameter: target (%d) out of range (0 - %d)",
    "alBnkfNew: bank file out of date",
    "alSeqNew: 0x%x is not a midi file",
    "alSeqNew: 0x%x is not a type 0 midi file",
    "alSeqNew: 0x%x has more than 1 track",
    "alSeqNew: SMPTE delta times not supported",
    "alSeqNew: Error parsing file 0x%x (no track header)",
    "alSeqNextEvent: Unsupported system exclusive",
    "alSeqNextEvent: Unsupported midi meta event 0x%x",
    "_handleMIDIMsg: Invalid program change to %d, max instruments %d",
    "_handleMIDIMsg: Unknown midi message 0x%x",
    "_unmapVoice: Couldn't unmap voice 0x%x",
    "alEvtqPostEvent: Out of free events",
    "alHeapAlloc: Can't allocate %d bytes",
    "alHeapCheck: Heap corrupt",
    "alHeapCheck: Heap corrupt - first block is bad",
    "alCSeqGetTrackEvent: Running status of zero on track %d",
    "alCSeqGetTrackEvent: Note on velocity of zero on track %d",
    "alCSPVoiceHandler: Stopping sequence but voice not free chan %d, key %d",
    "alSeqNextEvent: Read past end of sequence",
    "_loadOutputBuffer: Modulated delay greater than total delay by %d samples",
};

void __osErrorFile(const char* file, s32 line, s16 errcode, s32 nargs, ...) {
    va_list ap;
    char buffer[4096];

    va_start(ap, nargs);
    vsnprintf(buffer, sizeof(buffer), error_strings[errcode], ap);
    luslog(file, line, LUSLOG_LEVEL_ERROR, buffer);
    va_end(ap);
}

#endif
