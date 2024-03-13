u8 ncube_nin64ortho_small_ci8[] = {
#include "nin64ortho_small.h"
};

u8 ncube_nin64ortho_small_ci8_pal_rgba16[] = {
#include "nin64ortho_small_pal.h"
};

u8 ncube_nin64ortho_small_rgba16[] = {
#include "nin64ortho_small_rgba16.h"
};

#define _M 2

Vtx ncube_Cube_mesh_vtx_0[16] = {
	{{ {100, 100, -100}, 0, {1008*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {-100, 100, 100}, 0, {-16*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {100, 100, 100}, 0, {1008*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {-100, 100, -100}, 0, {-16*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {-100, -100, -100}, 0, {-16*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {100, -100, -100}, 0, {1008*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {100, 100, 100}, 0, {-16*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {100, -100, 100}, 0, {-16*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {100, -100, 100}, 0, {1008*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {100, 100, 100}, 0, {1008*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {-100, 100, 100}, 0, {-16*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {-100, -100, 100}, 0, {-16*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {-100, 100, -100}, 0, {1008*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {-100, -100, -100}, 0, {1008*_M, 1008*_M}, {255, 255, 255, 255} }},
	{{ {-100, -100, -100}, 0, {-16*_M, -16*_M}, {255, 255, 255, 255} }},
	{{ {100, -100, -100}, 0, {1008*_M, -16*_M}, {255, 255, 255, 255} }},
};

Gfx ncube_Cube_mesh_tri_0[] = {
	gsSPVertex(ncube_Cube_mesh_vtx_0 + 0, 16, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 3, 1, 0),
	gsSP1Triangle(4, 3, 0, 0),
	gsSP1Triangle(4, 0, 5, 0),
	gsSP1Triangle(5, 0, 6, 0),
	gsSP1Triangle(5, 6, 7, 0),
	gsSP1Triangle(8, 9, 10, 0),
	gsSP1Triangle(8, 10, 11, 0),
	gsSP1Triangle(11, 10, 12, 0),
	gsSP1Triangle(11, 12, 13, 0),
	gsSP1Triangle(14, 8, 11, 0),
	gsSP1Triangle(14, 15, 8, 0),
	gsSPEndDisplayList(),
};

Gfx mat_ncube_f3dlite_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0),
	gsSPClearGeometryMode(G_LIGHTING),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetAlphaCompare(G_AC_THRESHOLD),
	gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
	gsDPSetBlendColor(0, 0, 0, 1),
	gsSPTexture(65535, 65535, 0, 0, 1),
#if 1
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPLoadTLUT_pal256(ncube_nin64ortho_small_ci8_pal_rgba16),
#if 1
	gsDPLoadTextureTile(ncube_nin64ortho_small_ci8, G_IM_FMT_CI, G_IM_SIZ_8b, 32, 32, 0, 0, 31, 31, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
#else
	gsDPLoadTextureBlock(ncube_nin64ortho_small_ci8, G_IM_FMT_CI, G_IM_SIZ_8b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
#endif

#else

#if 0
	gsDPLoadTextureTile(ncube_nin64ortho_small_rgba16, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, 0, 31, 31, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
#else
	gsDPLoadTextureBlock(ncube_nin64ortho_small_rgba16, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
#endif

#endif
	gsSPEndDisplayList(),
};

Gfx mat_revert_ncube_f3dlite_material[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPEndDisplayList(),
};

Gfx ncube_Cube_mesh[] = {
	gsSPDisplayList(mat_ncube_f3dlite_material),
	gsSPDisplayList(ncube_Cube_mesh_tri_0),
	gsSPDisplayList(mat_revert_ncube_f3dlite_material),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsSPEndDisplayList(),
};

