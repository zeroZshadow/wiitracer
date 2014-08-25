#include "gxutils.h"

/* SDK libs */
#include <string.h>
#include <malloc.h>


#define DEFAULT_FIFO_SIZE	(256*1024)
void *xfb[2] = { NULL, NULL };
void *screenBuffer = NULL;
GXTexObj* screenTexObject = NULL;
u16 screenWidth, screenHeight;
u32 fbi = 0;
GXRModeObj *rmode = NULL;
BOOL first_frame = FALSE;
void *gpfifo = NULL;

void GXU_init() {
	VIDEO_Init();

	/* Get render mode */
	rmode = VIDEO_GetPreferredMode(NULL);

	/* Allocate the fifo buffer */
	gpfifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gpfifo, 0, DEFAULT_FIFO_SIZE);

	/* Allocate frame buffers */
	xfb[0] = SYS_AllocateFramebuffer(rmode);
	xfb[1] = SYS_AllocateFramebuffer(rmode);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb[fbi]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	/* Enable USBGecko debugging */
	//CON_EnableGecko(1, FALSE);

	/* Swap frames */
	fbi ^= 1;

	/* Init flipper */
	GX_Init(gpfifo, DEFAULT_FIFO_SIZE);

	/* Clear the background to black and clear the Z buf */
	GXColor background = { 0xa0, 0xe0, 0xf0, 0xff };
	GX_SetCopyClear(background, 0x00ffffff);

	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

	GX_SetPixelFmt(rmode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfb[fbi], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

	Mtx44 perspective;
	//guOrtho(perspective, 0, rmode->viHeight, 0, rmode->viWidth, 0, 300);
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

	first_frame = TRUE;
}

void GXU_createPixelBuffer(u16 width, u16 height) {
	screenWidth = width;
	screenHeight = height;

	screenTexObject = malloc(sizeof(GXTexObj));
	if (screenTexObject == -1) {
		printf("failed to alloc screenTexObject");
		return;
	}

	u32 buffersize = GX_GetTexBufferSize(width, height, GX_TF_RGBA8, GX_FALSE, 0);
	screenBuffer = memalign(32, buffersize);
	if (screenBuffer == -1) {
		printf("failed to alloc screenBuffer");
		return;
	}
	
	GX_InitTexObj(screenTexObject, screenBuffer, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
}

void GXU_clearPixelBuffer(u32 color) {
	u32 x, y, ix, iy;
	u16* colorBuffer = screenBuffer;
	u32 index = 0;

	GX_InvalidateTexAll();

	for (y = 0; y<screenHeight; y += TILESIZE) {
		for (x = 0; x<screenWidth; x += TILESIZE) {
			for (iy = 0; iy<TILESIZE; ++iy) {
				for (ix = 0; ix<TILESIZE; ++ix) {
					u16 arcolor = (color) >> 16; // Alpha | Red
					colorBuffer[index++] = arcolor;
				}
			}
			for (iy = 0; iy<TILESIZE; ++iy) {
				for (ix = 0; ix<TILESIZE; ++ix) {
					u16 gbcolor = (color & 0x0000FFFF); //Green | Blue
					colorBuffer[index++] = gbcolor;
				}
			}
		}
	}
}

u32 GXU_copyTilePixelBuffer(u32* tileData, u32 tilex, u32 tiley) {
	u16 ix, iy;
	u16* colorBuffer = screenBuffer;

	const u32 tilewidth = screenWidth / TILESIZE;
	u32 index = (tilex + (tiley * tilewidth)) * (TILESIZE*TILESIZE) * 2;

	for (iy = 0; iy<TILESIZE; ++iy) {
		for (ix = 0; ix<TILESIZE; ++ix) {
			u32 color = tileData[ix + (iy * TILESIZE)];
			u16 arcolor = (color) >> 16; // Alpha | Red
			colorBuffer[index++] = arcolor;
		}
	}
	for (iy = 0; iy<TILESIZE; ++iy) {
		for (ix = 0; ix<TILESIZE; ++ix) {
			u32 color = tileData[ix + (iy * TILESIZE)];
			u16 gbcolor = (color & 0x0000FFFF); //Green | Blue
			colorBuffer[index++] = gbcolor;
		}
	}

	GX_InvalidateTexAll();

	return index;
}

void GXU_renderPixelBuffer() {

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
	GX_CopyDisp(xfb[fbi], GX_TRUE);

	GX_DrawDone();

	/* Flush and swap buffers */
	VIDEO_SetNextFramebuffer(xfb[fbi]);
	if (first_frame) {
		first_frame = 0;
		VIDEO_SetBlack(FALSE);
	}
	fbi ^= 1;
	VIDEO_Flush();
	VIDEO_WaitVSync();
}

GXRModeObj* GXU_getMode() {
	return rmode;
}

guVector GXU_blendColors(guVector c1, guVector c2, f32 blend) {
	guVector res, delta;
	guVecSub(&c2, &c1, &delta);
	guVecScale(&delta, &delta, blend);
	guVecAdd(&c1, &delta, &res);
	return res;
}

GXColor GXU_vectorToColorData(guVector color) {
	GXColor res = {
		0xFF,
		color.x * 0xFF,
		color.y * 0xFF,
		color.z * 0xFF,
	};
	return res;
}