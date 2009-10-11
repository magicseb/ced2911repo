/**
 Function Utiliser pour l'ecriture, creation de surface et l'initialisation des textures
 **/

#include "libxenon.h"
#include "menu.h"
#include "drawtexture.h"
#include "verdana.h"

struct XenosSurface * XeGetBitmapFromMemory(int width, int height, void *pixeldata)
{
	printf("\t\t\tXeGetBitmapFromMemory(int width,int height,void *pixeldata) ...!!\n");
	struct XenosSurface *surface = Xe_CreateTexture(xe, width, height, 1, XE_FMT_8888 | XE_FMT_ARGB, 0);
	Xe_Surface_LockRect(xe, surface, 0, 0, 0, 0, XE_LOCK_WRITE);
	unsigned long size = (surface->height) * (surface->pitch) + 1;
	printf("\t\t\tmemcpy ...!!\n");
	memcpy(surface->base, pixeldata, size);//Copie la texture en mémoire dans la surface xe
	Xe_Surface_Unlock(xe, surface);
	return surface;
}

struct XenosSurface * XeGetFont()
{
	/*
	 struct XenosSurface *font = Xe_CreateTexture(xe, 256, 256, 1, XE_FMT_8, 0);
	 uint32_t *data = (uint32_t*)Xe_Surface_LockRect(xe, font, 0, 0, 0, 0, XE_LOCK_WRITE);
	 memcpy(data, fontdata, sizeof(fontdata));
	 Xe_Surface_Unlock(xe, font);
	 return font;
	 */
	return loadPNGFromMemory(verdana_0_png);
}

void XeDrawSurface2(struct XenosSurface *txt, float dstx, float dsty, float width, float height, int withalpha)
{
	float uleft = 0, uright = 1, vtop = 0, vbottom = 1;

	float logo[] =
	{ 	dstx,
		dsty + height,
		uleft,
		vtop,
		dstx + width,
		dsty + height,
		uright,
		vtop,
		dstx,
		dsty,
		uleft,
		vbottom,
		dstx,
		dsty,
		uleft,
		vbottom,
		dstx + width,
		dsty + height,
		uright,
		vtop,
		dstx + width,
		dsty,
		uright,
		vbottom };

	Xe_VBBegin(xe, 4);
	Xe_VBPut(xe, logo, 6 * 4);

	struct XenosVertexBuffer *vb = Xe_VBEnd(xe);
	Xe_VBPoolAdd(xe, vb);

	if (withalpha)
	{
		Xe_SetSrcBlend(xe, XE_BLEND_SRCALPHA);
		Xe_SetDestBlend(xe, XE_BLEND_INVSRCALPHA);
		Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
	}
	else
	{
		Xe_SetSrcBlend(xe, XE_BLEND_ONE);
		Xe_SetDestBlend(xe, XE_BLEND_ZERO);
		Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
	}

	Xe_SetShader(xe, SHADER_TYPE_PIXEL, sh_text_ps, 0);
	Xe_SetShader(xe, SHADER_TYPE_VERTEX, sh_text_vs, 0);
	Xe_SetTexture(xe, 0, txt);
	Xe_Draw(xe, vb, 0);
}

//Garde l'aspect ratio :) en function de la texture et non du framebuffer
void XeDrawSurface(struct XenosSurface *txt, float dstx, float dsty, float scale, int withalpha)
{

	float scrratio = ((float) xe->tex_fb.width / (float) xe->tex_fb.height);
	//float txtratio=((float)txt->width/(float)txt->height);

	float height = scale * scrratio;

	float width = scale;

	float uleft = 0, uright = 1, vtop = 0, vbottom = 1;

	float logo[] =
	{ 	dstx,
		dsty + height,
		uleft,
		vtop,
		dstx + width,
		dsty + height,
		uright,
		vtop,
		dstx,
		dsty,
		uleft,
		vbottom,
		dstx,
		dsty,
		uleft,
		vbottom,
		dstx + width,
		dsty + height,
		uright,
		vtop,
		dstx + width,
		dsty,
		uright,
		vbottom };

	Xe_VBBegin(xe, 4);
	Xe_VBPut(xe, logo, 6 * 4);

	struct XenosVertexBuffer *vb = Xe_VBEnd(xe);
	Xe_VBPoolAdd(xe, vb);

	if (withalpha)
	{
		Xe_SetSrcBlend(xe, XE_BLEND_SRCALPHA);
		Xe_SetDestBlend(xe, XE_BLEND_INVSRCALPHA);
		Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
	}
	else
	{
		Xe_SetSrcBlend(xe, XE_BLEND_ONE);
		Xe_SetDestBlend(xe, XE_BLEND_ZERO);
		Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
	}

	Xe_SetShader(xe, SHADER_TYPE_PIXEL, sh_text_ps, 0);
	Xe_SetShader(xe, SHADER_TYPE_VERTEX, sh_text_vs, 0);
	Xe_SetTexture(xe, 0, txt);
	Xe_Draw(xe, vb, 0);
}

#define WH 512.0

void XePrintf(float dstx, float dsty, const char *text, float scale, uint32_t icolor)
{
	if (!*text)
		return 0;
	Xe_VBBegin(xe, 5);

	while (*text)
	{
		//struct fontentry *f = &fontentries[(unsigned char)*text++];
		struct fnt_s *f = &verdana_fnt[(unsigned char) *text++];
		if (!f->width)
			continue;

		float u = f->x / WH;
		float v = f->y / WH;

		float uw = f->width / WH;
		float vw = f->height / WH;

		float width = uw * scale, height = vw * scale;

		float xo = f->xoffset / WH * scale;
		float yo = f->yoffset / WH* scale;

		float left = dstx + xo, top = dsty - yo, right = dstx + width, bottom = dsty - yo - height;

		union
		{
			float color;
			int icolor;
		} un;

		un.icolor = icolor;

		float color = un.color;

		float letter[] =
		{ 	left,
			top,
			u,
			v,
			color,
			right,
			top,
			u + uw,
			v,
			color,
			left,
			bottom,
			u,
			v + vw,
			color,
			left,
			bottom,
			u,
			v + vw,
			color,
			right,
			top,
			u + uw,
			v,
			color,
			right,
			bottom,
			u + uw,
			v + vw,
			color, };

		Xe_VBPut(xe, letter, 6 * 5);

		dstx += f->xadvance / WH * scale;
	}

	struct XenosVertexBuffer *vb = Xe_VBEnd(xe);
	Xe_VBPoolAdd(xe, vb);

	Xe_SetSrcBlend(xe, XE_BLEND_SRCALPHA);
	Xe_SetDestBlend(xe, XE_BLEND_INVSRCALPHA);
	Xe_SetBlendOp(xe, XE_BLENDOP_ADD);

	Xe_SetShader(xe, SHADER_TYPE_PIXEL, sh_font_ps, 0);
	Xe_SetShader(xe, SHADER_TYPE_VERTEX, sh_font_vs, 0);
	Xe_SetTexture(xe, 0, XeFont);
	Xe_Draw(xe, vb, 0);

	return dstx;
}

/**
 Effectue un printf rapide avec effaement de couleur/ et rafraichissement

 **/
void fastPrintf(int x, int y, int color, char *str)
{
	Xe_InvalidateState(xe);
	XeDrawSurface2(XeBgInfo, -1, -1, 2, 2, 0);
	XePrintf(-0.5, 0, str, 0.6, 0xFF0000);//Bleu
#if 1
	//Blanc ....
	Xe_SetClearColor(xe, ~0);
	Xe_Resolve(xe);

	while (!Xe_IsVBlank(xe))
		;
	Xe_Sync(xe);
#endif
}
/**
 Effectue un printf rapide avec effaement de couleur/ et rafraichissement

 **/
void errorPrintf(int x, int y, int color, char *str)
{
	Xe_InvalidateState(xe);
	XePrintf(-0.5, 0, str, 0.6, 0x0000FF);//Red
#if 1
	//Blanc ....
	Xe_SetClearColor(xe, ~0);
	Xe_Resolve(xe);

	while (!Xe_IsVBlank(xe))
		;
	Xe_Sync(xe);
	//Attends qu'une touche soit presser
	while (1)
	{
		struct controller_data_s c;
		if (get_controller_data(&c, 0))
		{
			return;//Quit
		}
	}
#endif
}
