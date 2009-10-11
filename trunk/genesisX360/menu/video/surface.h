/*
 * surface.h
 *
 *  Created on: 9 oct. 2009
 *      Author: ced
 */

#ifndef SURFACE_H_
#define SURFACE_H_

//Image charger sur la stick ou en mémoire
struct XenosSurface *XeBackground;
struct XenosSurface *XeFileIcon;
struct XenosSurface *XeFolderIcon;
struct XenosSurface *XeCursor;

//Image permanente
struct XenosSurface *XeFont;
struct XenosSurface *XeBgInfo;
struct XenosSurface *XeLoadingForm;

// en mémoire
#define u8 unsigned char;
#define u32 int;

//global
#include "bg_png.h"
#include "file_png.h"
#include "folder_png.h"
#include "star_png.h"
#include "loading_png.h"
#include "verdana_0_png.h"
#include "loading_form_png.h"

//igm
#include "igm_bg_png.h"
#include "igm_ok_png.h"
#include "igm_cancel_png.h"
#include "igm_menu_restart_png.h"
#include "igm_popup_png.h"
#include "manette_png.h"
#include "igm_menu_info_png.h"

#undef u8
#undef u32

#endif /* SURFACE_H_ */
