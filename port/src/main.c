#include <libultra.h>
#include "PcMain.h"
#include "os.h"
//#include "simple.h"

// sched.h

typedef struct {
    short type;
    char  misc[30];
} OSScMsg;

typedef struct OSScTask_s {
    struct OSScTask_s   *next;          /* note: this must be first */
    u32                 state;
    u32			flags;
    void		*framebuffer;	/* used by graphics tasks */

    OSTask              list;
    OSMesgQueue         *msgQ;
    OSMesg              msg;
#ifndef _FINALROM                       /* all #ifdef items should    */
    OSTime              startTime;      /* remain at the end!!, or    */
    OSTime              totalTime;      /* possible conflict if       */
#endif                                  /* FINALROM library used with */
} OSScTask;                             /* non FINALROM code          */

#define OS_SC_RETRACE_MSG       1
#define OS_SC_DONE_MSG          2
#define OS_SC_RDP_DONE_MSG      3
#define OS_SC_PRE_NMI_MSG       4
#define OS_SC_LAST_MSG          4	/* this should have highest number */
#define OS_SC_MAX_MESGS         8

// simple.h

/* define a message after the set used by the scheduler */
#define SIMPLE_CONTROLLER_MSG	    (OS_SC_LAST_MSG+1)

void initGame(void);
void gameloop(void);
extern OSMesgQueue *sched_cmdQ;
extern OSMesgQueue gfxFrameMsgQ;

// port

static OSMesgQueue s_sched_cmdQ;
static OSMesg s_sched_msg;

void port_mainloop(void) {
    short msg;
    OSScTask *t;

    msg = OS_SC_RETRACE_MSG;
    osSendMesg(&gfxFrameMsgQ, OS_MESG_PTR(&msg), OS_MESG_NOBLOCK);

    StartFrame();
    gameloop();
    osRecvMesg(sched_cmdQ, (OSMesg*)&t, OS_MESG_NOBLOCK);
    ProcessGfxCommands((Gfx*) t->list.t.data_ptr);

    msg = OS_SC_DONE_MSG;
    osSendMesg(&gfxFrameMsgQ, OS_MESG_PTR(&msg), OS_MESG_NOBLOCK);
    gameloop();

    msg = SIMPLE_CONTROLLER_MSG;
    osSendMesg(&gfxFrameMsgQ, OS_MESG_PTR(&msg), OS_MESG_NOBLOCK);
    gameloop();

    EndFrame();
}

int main(int argc, char *argv[]) {
    InitDM64();

    osCreateMesgQueue(&s_sched_cmdQ, &s_sched_msg, 1);
    sched_cmdQ = &s_sched_cmdQ;
    initGame();

    GameLoop(port_mainloop);
    DeinitDM64();
    return 0;
}
