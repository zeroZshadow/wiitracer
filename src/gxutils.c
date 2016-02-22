#include "gxutils.h"

/* SDK libs */
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include "mathutils.h"
#include "mu.h"

#define DEFAULT_FIFO_SIZE	(256*1024)
void *xfb[2] = { NULL, NULL };
void *screenBuffer = NULL;
GXTexObj* screenTexObject = NULL;
u16 screenWidth, screenHeight;
u32 fbi = 0;
GXRModeObj *vmode = NULL;
BOOL first_frame = FALSE;
void *gpfifo = NULL;

void GXU_init() {
	VIDEO_Init();

	/* Get render mode AND configure */
	vmode = VIDEO_GetPreferredMode(&TVPal528Int);// &TVPal528Int;// VIDEO_GetPreferredMode(NULL);
	VIDEO_Configure(vmode);

	/* Allocate frame buffers */
	xfb[0] = (u32 *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));
	xfb[1] = (u32 *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));

	/* Clean buffers */
	VIDEO_ClearFrameBuffer(vmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);
	VIDEO_SetNextFramebuffer(xfb[fbi]);

	/* Flush, and vsync once*/
	VIDEO_Flush();
	VIDEO_WaitVSync();

	/* If not interlaced, VSync again*/
	if (vmode->viTVMode & VI_NON_INTERLACE) {
		VIDEO_WaitVSync();
	} else while (VIDEO_GetNextField()) {
		VIDEO_WaitVSync();
	}

	/* Swap frames */
	fbi ^= 1;

	/* Init flipper */
	gpfifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gpfifo, 0, DEFAULT_FIFO_SIZE);
	GX_Init(gpfifo, DEFAULT_FIFO_SIZE);

	/* Clear the background to black and clear the Z buf */
	GXColor background = { 0xa0, 0xe0, 0xf0, 0xff };
	GX_SetCopyClear(background, GX_MAX_Z24);

	/* Culling and copy gamma settings*/
	GX_SetCullMode(GX_CULL_NONE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	/* Display render settings */
	f32 yscale = GX_GetYScaleFactor(vmode->efbHeight, vmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopySrc(0, 0, vmode->fbWidth, vmode->efbHeight);
	GX_SetDispCopyDst(vmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(vmode->aa, vmode->sample_pattern, GX_TRUE, vmode->vfilter);
	GX_SetFieldMode(vmode->field_rendering, ((vmode->viHeight == 2 * vmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

	/* Pixel format */
	GX_SetPixelFmt(vmode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	/* Set viewporty */
	GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);

	/* Create perspective matrix*/
	Mtx44 perspective;
	guOrtho(perspective, 0, 1, 0, 1, 0, 300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

	Mtx GXmodelView2D;
	guMtxIdentity(GXmodelView2D);
	guMtxTransApply(GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
	GX_LoadPosMtxImm(GXmodelView2D, GX_PNMTX0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_InvalidateTexAll();

	first_frame = GX_TRUE;
}

void GXU_createPixelBuffer(u16 width, u16 height) {
	screenWidth = width;
	screenHeight = height;

	screenTexObject = malloc(sizeof(GXTexObj));
	if (screenTexObject == NULL) {
		printf("failed to alloc screenTexObject");
		return;
	}

	u32 buffersize = GX_GetTexBufferSize(width, height, GX_TF_RGBA8, GX_FALSE, 0);
	printf("allocating screenBuffer [%u]", buffersize);
	screenBuffer = memalign(32, buffersize);
	if (screenBuffer == NULL) {
		printf("failed to alloc screenBuffer");
		return;
	}

	GX_InitTexObj(screenTexObject, screenBuffer, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObjFilterMode(screenTexObject, GX_LINEAR, GX_LINEAR);
}

void GXU_clearPixelBuffer(u32 color) {
	u32 x, y, ix, iy;
	u16* colorBuffer = screenBuffer;
	u32 index = 0;
	const u32 looplength = TILESIZE * TILESIZE;

	GX_InvalidateTexAll();

	for (y = 0; y < screenHeight; y += TILESIZE) {
		for (x = 0; x < screenWidth; x += TILESIZE) {
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					u16 arcolor = color >> 16;	// Alpha | Red
					u16 gbcolor = color;		// Green | Blue
					colorBuffer[index] = arcolor;
					colorBuffer[index + looplength] = gbcolor;
					index++;
				}
			}
			index += looplength;
		}
	}
}

void GXU_copyTilePixelBuffer(GXColor* tileData, u32 tilex, u32 tiley) {
	u16 ix, iy;
	u16* colorBuffer = screenBuffer;
	u32* data = (u32*)tileData;

	const u32 widthtiles = screenWidth / TILESIZE;
	const u32 looplength = TILESIZE * TILESIZE;
	u32 index = (tilex + (tiley * widthtiles)) * (looplength) << 1;

	for (iy = 0; iy < TILESIZE; ++iy) {
		for (ix = 0; ix < TILESIZE; ++ix) {
			u32 color = data[ix + (iy * TILESIZE)];
			u16 arcolor = (color) >> 16; // Alpha | Red
			u16 gbcolor = color; //Green | Blue
			colorBuffer[index] = arcolor;
			colorBuffer[index + looplength] = gbcolor;
			index++;
		}
	}

	GX_InvalidateTexAll();
}

void GXU_renderPixelBuffer() {

	GX_Flush();
	GX_LoadTexObj(screenTexObject, GX_TEXMAP0);

	GX_InvVtxCache();
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);	// Draw A Quad
	GX_Position2f32(0, 0);	// Top Left
	GX_TexCoord2f32(0, 0);

	GX_Position2f32(1, 0);	// Top Right
	GX_TexCoord2f32(1, 0);

	GX_Position2f32(1, 1);	// Bottom Right
	GX_TexCoord2f32(1, 1);

	GX_Position2f32(0, 1);	// Bottom Left
	GX_TexCoord2f32(0, 1);
	GX_End();
}

void GXU_done() {
	/* Finish up rendering */
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[fbi], first_frame);

	GX_DrawDone();

	/* Flush and swap buffers */
	VIDEO_SetNextFramebuffer(xfb[fbi]);
	if (first_frame == GX_TRUE) {
		first_frame = GX_FALSE;
		VIDEO_SetBlack(FALSE);
	}
	fbi ^= 1;
	VIDEO_Flush();
	VIDEO_WaitVSync();
}

GXRModeObj* GXU_getMode() {
	return vmode;
}

void GXU_blendColors(guVector* c1, guVector* c2, guVector* result, f32 blend) {
	//Abuse the result vector for temp storage of the delta c2-c1
	guVecSub(c2, c1, result);
	guVecScale(result, result, blend);
	guVecAdd(c1, result, result);
}

GXColor GXU_vectorToColorData(guVector* color) {
	guVector mul;
	muVecScale(color, &mul, 255.0f);
	GXColor res = { 0xFF, mul.x, mul.y, mul.z };
	return res;
}