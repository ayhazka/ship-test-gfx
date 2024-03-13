/*====================================================================
 * gfx.c
 *
 * Synopsis:
 *
 * This code implements the application graphics stuff
 * 
 * 
 * 
 * Copyright 1993, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics,
 * Inc.; the contents of this file may not be disclosed to third
 * parties, copied or duplicated in any form, in whole or in part,
 * without the prior written permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to
 * restrictions as set forth in subdivision (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS
 * 252.227-7013, and/or in similar or successor clauses in the FAR,
 * DOD or NASA FAR Supplement. Unpublished - rights reserved under the
 * Copyright Laws of the United States.
 *====================================================================*/

/*---------------------------------------------------------------------*
        Copyright (C) 1998 Nintendo. (Originated by SGI)
        
        $RCSfile: gfx.c,v $
        $Revision: 1.1.1.1 $
        $Date: 2002/05/02 03:27:21 $
 *---------------------------------------------------------------------*/

#include <ultra64.h>
#include <assert.h>
#include <PR/gu.h>
#include <string.h>

#include "gfx.h"
#include "simple.h"
#include "misc.h"
#include "tex_func.h"

/*
 * graphics globals
 */

extern OSSched         sc;
extern OSMesgQueue     *sched_cmdQ;
extern GFXInfo         gInfo[];

extern s8    logoPos_x;
extern s8    logoPos_y;
extern s8    logoPos_z;
extern f32   logoScale_x;
extern f32   logoScale_y;
extern f32   logoScale_z;
extern f32   logoVeloc;

u32	ucode_index = 0;

static u32          framecount;

static char         *staticSegment = 0;

#ifndef _FINALROM
extern OSTime      lastTime;
#endif

#ifdef TARGET_PC
#include <stdio.h>
#define PRINT(...) printf(__VA_ARGS__)
#else
#define PRINT(...)
#endif

void doLogo(Dynamic *dynamicp);
void doNcubeSpeeen(Dynamic *dynamicp);
void imgTest1(Dynamic *dynamicp);
void imgTest2(Dynamic *dynamicp);
void imgTest3(Dynamic *dynamicp);
void imgTest4(Dynamic *dynamicp);
void imgTest5(Dynamic *dynamicp);
void imgVertexedTest(Dynamic *dynamicp);
void imgVertexedTest2(Dynamic *dynamicp);
void fill_4_rect_alpha(void);

extern TiTexData tex_test1[]; // segmented
#ifndef TARGET_PC
static TiTexData* tex_test1_addr;
#else
#define tex_test1_addr tex_test1
#endif

void initGFX(void) 
{    
#ifndef TARGET_PC
extern char _gfxdlistsSegmentEnd[];
    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);

    assert (len < (_gfxdlistsSegmentEnd - _gfxdlistsSegmentStart));
    staticSegment = _gfxdlistsSegmentStart;
    romCopy(_staticSegmentRomStart, staticSegment, len);

    tex_test1_addr = (TiTexData*)(((u32)&tex_test1 & 0x00ffffff) + (u32)staticSegment);
    tiMappingAddr(tex_test1_addr, 20, (u32)staticSegment);
#endif

    gInfo[0].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[0].cfb = cfb_16_a;
    gInfo[1].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[1].cfb = cfb_16_b;

    /* The Vi manager was started by scheduler by this point in time */
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
}

void createGfxTask(GFXInfo *i) 
{
    static int firsttime = 1;
    Dynamic *dynamicp;
    OSScTask *t;
    
    /**** pointers to build the display list. ****/
    dynamicp = &i->dp;
    glistp   = i->dp.glist;
    mtxP = i->dp.mtxList;
    vtxP = i->dp.vtxList;

#ifndef TARGET_PC
    /**** Tell RCP where each segment is  ****/
    gSPSegment(glistp++, 0, 0);	/* physical addressing segment */
    gSPSegment(glistp++, STATIC_SEGMENT,  osVirtualToPhysical(staticSegment));
    gSPSegment(glistp++, DYNAMIC_SEGMENT, osVirtualToPhysical(dynamicp));
#endif

    /**** Graphics pipeline state initialization ****/
    gSPDisplayList(glistp++, setup_rspstate);
    if (firsttime) {
        gSPDisplayList(glistp++, rdpstateinit_dl);
	firsttime = 0;
    }

    gSPDisplayList(glistp++, setup_rdpstate);
    
    /**** clear zbuffer, z = max z, dz = 0  ****/
    gDPSetDepthImage(glistp++, osVirtualToPhysical(zbuffer));
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(zbuffer));
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ,0) << 16 |
			       GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
    /**** Clear framebuffer cvg = FULL or 1  ****/
    gDPPipeSync(glistp++);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(i->cfb));
    /* clearcolor is 32-bits (2 pixels wide) because fill mode
     * does two pixels at a time.
     */
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
			       GPACK_RGBA5551(0, 0, 0, 1)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_1CYCLE); 

    assert((void *)glistp < (void *)&i->msg);

    /**** Draw objects */
    //doLogo(dynamicp);
    //doNcubeSpeeen(dynamicp);
    imgTest1(dynamicp);
    //imgTest2(dynamicp);
    //imgTest3(dynamicp);
    //imgTest4(dynamicp);
    //imgTest5(dynamicp);
    //fill_4_rect_alpha();
    //imgVertexedTest(dynamicp);
    //imgVertexedTest2(dynamicp);

    
    /**** Put an end on the top-level display list  ****/
    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    /* Flush the dynamic segment */
    osWritebackDCache(&i->dp, (s32)glistp - (s32)&i->dp);

    /* build graphics task */

    t = &i->task;
    t->list.t.data_ptr = (u64 *) dynamicp->glist;
    t->list.t.data_size = (s32)(glistp - dynamicp->glist) * sizeof (Gfx);
    t->list.t.type = M_GFXTASK;
#ifdef	F3DEX_GBI
    t->list.t.flags = OS_TASK_LOADABLE;
#else
    t->list.t.flags = 0;
#endif
#ifndef TARGET_PC
    t->list.t.ucode_boot = (u64 *)rspbootTextStart;
    t->list.t.ucode_boot_size = ((s32) rspbootTextEnd - 
				 (s32) rspbootTextStart);
#ifdef	XBUS
    switch(ucode_index){
    case	0:
      t->list.t.ucode =      (u64 *) gspF3DEX2_xbusTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DEX2_xbusDataStart;
      break;
    case	1:
      t->list.t.ucode =      (u64 *) gspF3DEX2_Rej_xbusTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DEX2_Rej_xbusDataStart;
      break;
    case	2:
      t->list.t.ucode =      (u64 *) gspF3DLX2_Rej_xbusTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DLX2_Rej_xbusDataStart;
      break;
    }
#else
    switch(ucode_index){
    case	0:
      t->list.t.ucode =      (u64 *) gspF3DEX2_fifoTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DEX2_fifoDataStart;
      break;
    case	1:
      t->list.t.ucode =      (u64 *) gspF3DEX2_Rej_fifoTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DEX2_Rej_fifoDataStart;
      break;
    case	2:
      t->list.t.ucode =      (u64 *) gspF3DLX2_Rej_fifoTextStart;
      t->list.t.ucode_data = (u64 *) gspF3DLX2_Rej_fifoDataStart;
      break;
    }
#endif    
    t->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    t->list.t.dram_stack = (u64 *) dram_stack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    t->list.t.output_buff = (u64 *) rdp_output;
    t->list.t.output_buff_size = (u64 *) rdp_output + 0x4000/8;
    t->list.t.yield_data_ptr = (u64 *) gfxYieldBuf;
    t->list.t.yield_data_size = OS_YIELD_DATA_SIZE;
#endif

    t->next     = 0;                   /* paranoia */
    t->flags	= (OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK |
		   OS_SC_SWAPBUFFER);
    t->msgQ     = &gfxFrameMsgQ;       /* reply to when finished */
    t->msg      = (OSMesg)&i->msg;     /* reply with this message */
    t->framebuffer = (void *)i->cfb;
#ifndef _FINALROM
    t->totalTime = 0;
#endif
    osSendMesg(sched_cmdQ, (OSMesg) t, OS_MESG_BLOCK);
    
    framecount++;
}

#include "images/ncube/header.h"

void doNcubeSpeeen(Dynamic *dynamicp) 
{
    u16		   perspNorm;
    static float   logo_theta = 0;
#ifndef _FINALROM
    u32      timeLen;
#endif
    /*
     * You must make the call to gSPPerspNormalize() in addition to 
     * using the perspective projection matrix.
     */
    guPerspective(&dynamicp->projection, &perspNorm,
		  33, 320.0/240.0, 400, 2000, 1.0);
    gSPPerspNormalize(glistp++, perspNorm);

    guLookAt(&dynamicp->viewing,
	     0, 0, 1000,
	     0, 0, 0,
	     0, 1, 0);
    

    /* draw the background first */
    guScale(&dynamicp->bg_model, 12.0, 7.0, 1.0);
    gSPMatrix(glistp++, &dynamicp->projection, 
	       G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->viewing,
	       G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->bg_model, 
	       G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, bg_dl);

    /* Position the logo: */
    guTranslate(&dynamicp->logo_trans, logoPos_x*4, logoPos_y*2, logoPos_z);
    /* Scale the logo */
    guScale(&dynamicp->logo_scale, logoScale_x, logoScale_y, logoScale_z);
    /* Rotate the logo */
    guRotate(&dynamicp->logo_rotate, logo_theta, 0.0, 1.0, 0.0);

    /* Setup model matrix */
    gSPMatrix(glistp++, &dynamicp->projection,
	       G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->viewing,
	       G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_trans, 
	      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_scale, 
	      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_rotate, 
	      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    guRotate(mtxP, logo_theta / 5, 0.0, 0.0, 0.4);
    gSPMatrix(glistp++, mtxP, 
	      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;
    
    /* Draw the logo */
    gSPDisplayList(glistp++, ncube_Cube_mesh);

    /* calculate theta for next frame */
    logo_theta += logoVeloc;
}

/*
 * Draw the SGI Logo
 *
 * You can mix the gu and the gSP commands.  The important item to 
 * remember is that the cache must be flushed of any dynamic data
 * before the RSP starts reading the command list.
 */
void doLogo(Dynamic *dynamicp)
{
    u16		   perspNorm;
    static float   logo_theta = 0;
#ifndef _FINALROM
    u32      timeLen;
#endif
    /*
     * You must make the call to gSPPerspNormalize() in addition to 
     * using the perspective projection matrix.
     */
    guPerspective(&dynamicp->projection, &perspNorm,
		  33, 320.0/240.0, 400, 2000, 1.0);
    gSPPerspNormalize(glistp++, perspNorm);

    guLookAt(&dynamicp->viewing,
	     0, 0, 1000,
	     0, 0, 0,
	     0, 1, 0);
    

    /* draw the background first */
    guScale(&dynamicp->bg_model, 12.0, 7.0, 1.0);
    gSPMatrix(glistp++, &dynamicp->projection, 
	       G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->viewing,
	       G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->bg_model, 
	       G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, bg_dl);

    /* Position the logo: */
    guTranslate(&dynamicp->logo_trans, logoPos_x*4, logoPos_y*2, logoPos_z);
    /* Scale the logo */
    guScale(&dynamicp->logo_scale, logoScale_x, logoScale_y, logoScale_z);
    /* Rotate the logo */
    guRotate(&dynamicp->logo_rotate, logo_theta, 0.0, 1.0, 0.0);

    /* Setup model matrix */
    gSPMatrix(glistp++, &dynamicp->projection,
	       G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->viewing,
	       G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_trans, 
	      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_scale, 
	      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(glistp++, &dynamicp->logo_rotate, 
	      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    
    /* Draw the logo */
    gSPDisplayList(glistp++, logo_dl);

    /* calculate theta for next frame */
    logo_theta += logoVeloc;

#ifndef _FINALROM   /* draw the performance bar */

#define USECS_PER_FRAME    16666  /* assuming a frame rate of 60fps */
#define USECS_PER_PIXEL    150
#define PIXELS_PER_FRAME   USECS_PER_FRAME/USECS_PER_PIXEL
#define LEFT_OFFSET        40
#define MARK_TOP           210
#define BAR_TOP            215
#define BAR_BOTTOM         220
#define MARK_BOTTOM        225
#define MARK_WIDTH         2

    gDPPipeSync(glistp++);

    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetPrimColor(glistp++, 0, 0, 200, 0, 120, 255);
    gDPSetCombineMode(glistp++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

    timeLen =(u32) (OS_CYCLES_TO_USEC(lastTime) / USECS_PER_PIXEL);
    gDPFillRectangle(glistp++, LEFT_OFFSET, BAR_TOP, 
		     LEFT_OFFSET + timeLen, BAR_BOTTOM);
    gDPFillRectangle(glistp++, LEFT_OFFSET+PIXELS_PER_FRAME, MARK_TOP, 
		     LEFT_OFFSET+PIXELS_PER_FRAME+MARK_WIDTH, MARK_BOTTOM);
    gDPFillRectangle(glistp++, LEFT_OFFSET+(2*PIXELS_PER_FRAME), MARK_TOP, 
		     LEFT_OFFSET+(2*PIXELS_PER_FRAME)+MARK_WIDTH, MARK_BOTTOM);
#endif
}

void fill_4_rect_alpha(void) {
    FillRectRGBA(&glistp, 320/6,   240/5, 320/6, 240/5*3,  255,255,255,255);
    FillRectRGBA(&glistp, 320/6*2, 240/5, 320/6, 240/5*3,  255,255,255,180);
    FillRectRGBA(&glistp, 320/6*3, 240/5, 320/6, 240/5*3,  255,255,255,127);
    FillRectRGBA(&glistp, 320/6*4, 240/5, 320/6, 240/5*3,  255,255,255,63);
}

static void tiStretchAlphaTexItem_2(Gfx **gpp, TiTexData *tiC, TiTexData *tiA,
	int count, int index, float x, float y, float w, float h, int untiled)
{
	int width, height;
	void *tex[2];

	width = MIN(tiC->info[0], tiA->info[0]);
	height = tiC->info[1] / count;
	height = MIN(height, tiA->info[1]);

    tex[0] = (u8 *)tiC->texs[1] + tiC->info[0] * height * index * 2;
    tex[1] = (u8 *)tiA->texs[1] + tiA->info[0] * height * index / 2;

	if(untiled) {
		StretchAlphaTexBlock(gpp, width, height,
			tex[0], tiC->info[0], tex[1], tiA->info[0], x, y, w, h);
	}
	else {
		StretchAlphaTexTile(gpp, width, height,
			tex[0], tiC->info[0], tex[1], tiA->info[0],
			0, 0, width, height, x, y, w, h);
	}
}

void imgVertexedTest2(Dynamic *dynamicp) {
    static int theta = 0;

    // 2d sprite on 3d display
    
    guOrtho(mtxP, 0, SCREEN_WD, SCREEN_HT, 0, 1, 10, 1);
    gSPMatrix(glistp++, mtxP, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    mtxP++;

    guTranslate(mtxP, 0, 0, -9);
    gSPMatrix(glistp++, mtxP, G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;

    gSPClearGeometryMode(glistp++, 0|G_ZBUFFER|G_SHADE|G_SHADING_SMOOTH|G_CULL_FRONT|G_CULL_BACK|G_LIGHTING|G_FOG|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR|G_CLIPPING|G_LOD|G_TEXTURE_ENABLE);
    gDPSetTexturePersp(glistp++, G_TP_NONE);

    gSPDisplayList(glistp++, alpha_texture_init_dl);

    guTranslate(mtxP, 320*3/4, 240/2, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    mtxP++;

    //guRotateRPY(mp, 0, 0, theta);
    //gSPMatrix(glistp++, mp, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    //mp++;

    static const int rwidth = 80;
    static const int rheight = 104;

    guTranslate(mtxP, -rwidth*3/4, -rheight/2, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;

    PRINT("\n-- letters combined (vertexed)\n");
    RectAlphaTexTile(&glistp, &vtxP, tex_test1_addr[14].info[0], tex_test1_addr[14].info[1],
                     tex_test1_addr[14].texs[1], tex_test1_addr[14].info[0],
                     tex_test1_addr[15].texs[1], tex_test1_addr[15].info[0],
                     0, 0, tex_test1_addr[15].info[0], tex_test1_addr[15].info[1],
                     0, 0, rwidth, rheight);

    PRINT("\n-- letters combined\n");
    StretchAlphaTexTile(&glistp, tex_test1_addr[14].info[0], tex_test1_addr[14].info[1],
                        tex_test1_addr[14].texs[1], tex_test1_addr[14].info[0],
                        tex_test1_addr[15].texs[1], tex_test1_addr[15].info[0],
                        0, 0, tex_test1_addr[15].info[0], tex_test1_addr[15].info[1],
                        320*1/4-rwidth*1/4, 240/2-rheight/2, rwidth, rheight);

    theta++;
}

void imgVertexedTest(Dynamic *dynamicp) {
    static int theta = 0;

    // 2d sprite on 3d display
    
    guOrtho(mtxP, 0, SCREEN_WD, SCREEN_HT, 0, 1, 10, 1);
    gSPMatrix(glistp++, mtxP, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    mtxP++;

    guTranslate(mtxP, 0, 0, -9);
    gSPMatrix(glistp++, mtxP, G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;

    gSPClearGeometryMode(glistp++, 0|G_ZBUFFER|G_SHADE|G_SHADING_SMOOTH|G_CULL_FRONT|G_CULL_BACK|G_LIGHTING|G_FOG|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR|G_CLIPPING|G_LOD|G_TEXTURE_ENABLE);
    gDPSetTexturePersp(glistp++, G_TP_NONE);

#if 1
    gSPDisplayList(glistp++, alpha_texture_init_dl);
    
    guTranslate(mtxP, 320*1/4, 240/4, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    mtxP++;

    //guRotateRPY(mp, 0, 0, theta);
    //gSPMatrix(glistp++, mp, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    //mp++;

    guTranslate(mtxP, -64*1/4, -64/4, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;

    PRINT("\n-- big n64 cube (vertexed)\n");
    RectAlphaTexTile(&glistp, &vtxP, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                    tex_test1_addr[11].texs[1], 64,
                    tex_test1_addr[12].texs[1], 64,
                    0, 0, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                    //0, 0, 64+sinf(theta/16.f)*32, 64+cosf(theta/16.f)*32);
                    0, 0, 64, 64);

    PRINT("\n-- big n64 cube\n");
    StretchAlphaTexTile(&glistp, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                        tex_test1_addr[11].texs[1], 64,
                        tex_test1_addr[12].texs[1], 64,
                        0, 0, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                        320/2-64/2, 240/2-64/2, 64, 64);

#endif

    gSPDisplayList(glistp++, normal_texture_init_dl);

    guTranslate(mtxP, 320*3/4, 240*3/4, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    mtxP++;

    //guRotateRPY(mp, 0, 0, theta);
    //gSPMatrix(glistp++, mp, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    //mp++;

    guTranslate(mtxP, -32*3/4, -32*3/4, 0);
    gSPMatrix(glistp++, mtxP, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    mtxP++;

    PRINT("\n-- small n64 cube (vertexed)\n");
    RectTexTile8(&glistp, &vtxP, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                 tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                 0, 0, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                 0, 0, 32, 32);

    PRINT("\n-- small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         0, 0, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         320/2-32/2, 240/4-32/4, 32, 32);

    theta++;
}

void imgTest5(Dynamic *dynamicp) {
    gSPDisplayList(glistp++, normal_texture_init_dl);

    PRINT("\n-- small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         0, 0, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         320/2-32/2, 240/4-32/4, 32, 32);

    gSPDisplayList(glistp++, alpha_texture_init_dl);
    
    PRINT("\n-- big n64 cube\n");
    StretchAlphaTexTile(&glistp, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                        tex_test1_addr[11].texs[1], 64,
                        tex_test1_addr[12].texs[1], 64,
                        0, 0, tex_test1_addr[11].info[0], tex_test1_addr[11].info[1],
                        320/2-64/2, 240/2-64/2, 64, 64);
}

void imgTest4(Dynamic *dynamicp) {
    gDPPipeSync(glistp++);
    static int theta = 0;
    
    gSPDisplayList(glistp++, alpha_texture_init_dl);
    
#if 0
    PRINT("\n-- texture alignment (untiled)\n")
    tiStretchAlphaTexItem_2(&glistp, &tex_test1_addr[18], &tex_test1_addr[19], 3, 0,
                            320/6, 240/6, tex_test1_addr[19].info[0] * 4, tex_test1_addr[19].info[1] / 3 * 4, 1);

    {
        float ypos = MAX(0, cosf(theta / 16.f)) * 100;
        PRINT("\n-- texture alignment (tiled)\n")
        tiStretchAlphaTexItem_2(&glistp, &tex_test1_addr[18], &tex_test1_addr[19], 3, 0,
                                320/6, 240/6 + ypos, tex_test1_addr[19].info[0] * 4, tex_test1_addr[19].info[1] / 3 * 4, 0);
    }
#endif

#if 0
#define ABS(x) ((x) < 0 ? -(x) : (x))
    {
        PRINT("\n-- letters (AlphaTst, ...)\n");
        static const int itemSize = 4;
        for (int i = 0, j = itemSize - 1; i < itemSize*2 - 1; i++, j--) {
            static const int texIdx = 14;
            const int w = tex_test1_addr[texIdx+1].info[0];
            const int h = tex_test1_addr[texIdx+1].info[1];
            float xpos = sinf((theta + i * 17)/16.f) * 30;
            tiStretchAlphaTexItem(&glistp, &tex_test1_addr[texIdx], &tex_test1_addr[texIdx+1],
                                0, itemSize, ABS(j), 320/2-w/2+xpos, 240/6-h/6 + i*26,
                                w, h/itemSize);
        }
    }
#endif

#if 0
    gSPDisplayList(glistp++, normal_texture_init_dl);
    
    PRINT("\n-- [1] small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         0, 0, tex_test1_addr[13].info[0]/3, tex_test1_addr[13].info[1],
                         320/6-32/6, 240/4-32/4, 32, 32);

    PRINT("\n-- [2] small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         0, 0, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         320/6-32/6, 240*2/4-32*2/4, 32, 32);

    PRINT("\n-- [3] small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         0, 0, tex_test1_addr[13].info[0]/2, tex_test1_addr[13].info[1],
                         320/6-32/6, 240*3/4-32*3/4, 32, 32);

    PRINT("\n-- [4] small n64 cube\n");
    StretchTexTile8(&glistp, tex_test1_addr[13].info[0], tex_test1_addr[13].info[1],
                         tex_test1_addr[13].texs[0], tex_test1_addr[13].texs[1],
                         tex_test1_addr[13].info[0]/4, 0, tex_test1_addr[13].info[0]/2, tex_test1_addr[13].info[1],
                         320*4/6-32*4/6, 240/4-32/4, 32, 32);
#endif

    theta++;
}

void imgTest1(Dynamic *dynamicp) {
    static int theta = 0;
    int texDim1[] = { cosf(theta/16.f) * 128 + 320/2, sinf(theta/16.f) * 64 + 240/2, 75 + sinf(theta / 8.f) * 64, 75 + cosf(theta / 10.f) * 64 };
    int texDim2[] = { cosf(theta/40.f) * 128 + 320/2, sinf(theta/40.f) * 64 + 240/2, 75 + cosf((theta + 60) / 10.f) * 64, 75 + sinf((theta + 60) / 8.f) * 64 };

    gDPPipeSync(glistp++);
    gSPDisplayList(glistp++, normal_texture_init_dl);
    //tiStretchTexBlock(&glistp, &tex_test1_addr[1], 0, texDim1[0], texDim1[1], texDim1[2], texDim1[3]);
    //tiStretchTexBlock(&glistp, &tex_test1_addr[0], 0, 40, 40, 32, 32);
    gSPDisplayList(glistp++, alpha_texture_init_dl);
    //tiStretchTexBlock(&glistp, &tex_test1_addr[0], 0, 90, 40, 32, 32);
    //tiStretchTexBlock(&glistp, &tex_test1_addr[3], 0, texDim2[0], texDim2[1], texDim2[2], texDim2[3]);

    StretchAlphaTexBlock(&glistp, tex_test1_addr[5].info[0], tex_test1_addr[5].info[1],
                         tex_test1_addr[5].texs[1], 32,
                         tex_test1_addr[4].texs[1], 32,
                         texDim1[0], 240/4, 32, 64);
    StretchAlphaTexBlock(&glistp, tex_test1_addr[5].info[0], tex_test1_addr[5].info[1],
                         tex_test1_addr[5].texs[1], 32,
                         tex_test1_addr[4].texs[1], 32,
                         texDim2[0], 240/4, 32, 64);

    gSPDisplayList(glistp++, normal_texture_init_dl);

    StretchTexTile8(&glistp, tex_test1_addr[6].info[0], tex_test1_addr[6].info[1],
                    tex_test1_addr[6].texs[0], tex_test1_addr[6].texs[1],
                    0, 0, 256, 64, 0, 0, sinf(theta/40.0f) * 64 + 64+16, cosf(theta/40.0f) * 32 + 32+16);
    theta++;
}
