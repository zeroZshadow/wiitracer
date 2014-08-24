/*! \file gxutils.h
 *  \brief Utility functions and wrappers for GX
 */

#ifndef _GXUTILS_H 
#define _GXUTILS_H

#include <gccore.h>

#define TILESIZE 4

/* GX vars */
extern void *screenBuffer;
extern GXTexObj* screenTexObject;
extern GXRModeObj *rmode;

/*! \brief Initialize the GX subsystem
 */
void GXU_init();

/*! \brief Set light color
 *  \param view View matrix
 *  \param lightColor Colors of Diffuse, Ambient and Material lights
 */
void GXU_setLight(Mtx view, GXColor lightColor[]);

/*! \brief Get video mode
 *  \return Currently preferred/used video mode
 */
GXRModeObj* GXU_getMode();

void GXU_createPixelBuffer(u16 width, u16 height);

void GXU_clearPixelBuffer(u32 color);

u32 GXU_copyTilePixelBuffer(u32* tileData, u32 tilex, u32 tiley);

void GXU_renderPixelBuffer();

/*! \brief Finish rendering and swap buffers
 */
void GXU_done();

#endif