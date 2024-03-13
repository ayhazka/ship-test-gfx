/*====================================================================
 * static.c
 *
 * Synopsis:
 *
 * SGI Logo data translated from Inventor format.
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
        
        $RCSfile: gfxstatic.c,v $
        $Revision: 1.1.1.1 $
        $Date: 2002/05/02 03:27:21 $
 *---------------------------------------------------------------------*/

#include <ultra64.h>
#include "gfx.h"
#ifdef	FOG
#include "fog_mode.h"
#endif
#include "tex_func.h"

// test textures

#include "images/nin64_half_1.h"
#include "images/nin64_half_1_alpha.h"
#include "images/nin64_half_2.h"
#include "images/nin64_half_2_alpha.h"
#include "images/nin64.h"
#include "images/nin64_alpha.h"
#include "images/nin64ortho.h"
#include "images/text_sprites.h"
#include "images/text_sprites_alpha.h"
#include "images/alpha_pattern.h"
#include "images/alpha_pattern_alpha.h"

TiTexData tex_test1[] = {
    {nin64_half_1_texs, nin64_half_1_info},
    {nin64_half_1_alpha_texs, nin64_half_1_alpha_info},
    {nin64_half_2_texs, nin64_half_2_info},
    {nin64_half_2_alpha_texs, nin64_half_2_alpha_info},
    {nin64_texs, nin64_info},
    {nin64_alpha_texs, nin64_alpha_info},
    {nin64ortho_texs, nin64ortho_info},
    {text_sprites_texs, text_sprites_info},
    {text_sprites_alpha_texs, text_sprites_alpha_info},
    {alpha_pattern_texs, alpha_pattern_info},
    {alpha_pattern_alpha_texs, alpha_pattern_alpha_info},
};

__attribute__((aligned(16))) char circular_half_0_tex[] = {
#include "images/circular_half_0.h"
};

__attribute__((aligned(16))) char circular_half_1_tex[] = {
#include "images/circular_half_1.h"
};

/*
 * To get the most use out of the vertex cache, we have broken
 * up the vertices into groups of loads, and machine-generated
 * the gfx_tri.h file which does the loads and triangle draws
 * for that load below.
 */
#include "gfx_vtx.h"	/* all the vertices */

/*
 * This is the SGI logo display list. It depends on the initialization
 * state elsewhere for some graphics pipeline state assumptions.
 */
Gfx logo_dl[] = {
    gsDPPipeSync(),
#ifdef	FOG
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER | G_CULL_BACK | G_FOG ),
#else
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_ZBUFFER | G_CULL_BACK),
#endif
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode (G_CC_SHADE, G_CC_SHADE),
#ifdef	FOG
    gsDPSetRenderMode(G_RM_ZB_1C_FOG_SHADE_A, G_RM_ZB_1C_FOG_SHADE_A2),
    gsDPSetFogColor(0xff, 0x00, 0x00, 0xff),
    gsSPFogPosition(700, 800),
#else
    gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
#endif

#include "gfx_tri.h"	/* all the vtx loads and triangles */

    gsDPPipeSync(),
    gsSPEndDisplayList(),
};


/*
 * The background is two big triangles.
 */
static Vtx bg_vtx[4] =  {
        {        -30,  30, 0, 0, 0, 0, 0, 0, 0xff, 0xff		},
        {         30,  30, 0, 0, 0, 0, 0, 0, 0xff, 0xff		},
        {         30, -30, 0, 0, 0, 0, 0, 0, 0x10, 0xff		},
        {        -30, -30, 0, 0, 0, 0, 0, 0, 0x10, 0xff		},
};

Gfx bg_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER),
    gsSPSetGeometryMode(G_SHADE),
    gsDPSetCombineMode (G_CC_SHADE, G_CC_SHADE),
    gsDPSetColorDither(G_CD_BAYER),	/* HW 1.0 should use 'G_CD_ENABLE' */
    gsSPVertex(&bg_vtx, 4, 0),
    gsSP1Triangle(0, 2, 1, 0),
    gsSP1Triangle(0, 3, 2, 0),
    gsSPEndDisplayList(),
};

#include "images/ncube/model.inc.c"
