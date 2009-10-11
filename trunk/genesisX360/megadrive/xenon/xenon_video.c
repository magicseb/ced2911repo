#include <stdio.h>
#include <xenos/xe.h>
#include <xenos/xenos.h>
#include "shared.h"
#include "xenon_video.h"
#include "xee.h"
#include "video.h"
#include "scalebit.h"


//utiliser pendant l'update


int f=0;
/**
main.c from cube example
**/
struct XenosSurface surface;

void updateVideo(int starting,int finishing)//starting =1 si c la premiere function qui update l'ecran, finishing =1 si elle fini le rafraisshement de l'ecran
{
	f++;
	int xres=getWidth();
	int yres=getHeight();
	//do_edram_foo(xe, 1);

	/**
	copy bitmap in surface (from snes9x)
	**/
	/* flush cache */
	Xe_Surface_LockRect(xe, gfxplane, 0, 0, 0, 0, XE_LOCK_WRITE);
	Xe_Surface_Unlock(xe, gfxplane);


	/* create new surface with right size */
	//struct XenosSurface surface = *gfxplane;
	struct XenosSurface surface_emul = *gfxplane;
	surface_emul.width = xres;
	surface_emul.height = yres;

	//scale(2,surface.base, surface.pitch, gfxplane->base, (xres * gfxplane->bypp + 127) &~127, gfxplane->bypp, xres, yres);
	dofilterscale2x(xres, yres, gfxplane->pitch, surface.pitch, gfxplane->base, surface.base, gfxplane->bypp);

	/* begin a new frame, i.e. reset all renderstates to the default */
	if(starting)
		Xe_InvalidateState(xe);

	glLoadIdentity();
	glPushMatrix();

	glTranslate(0, 0, -3);//bouge de 3 en arriere
	//glRotate(10.0, cubAnglex, cubAngley, cubAnglez);

	//glRotate(f / 100.0, .5, .1, 1);

	glScale(cubAnglex, cubAngley, 1);

//	glScale(0.9, 0.9, 1);

	M_LoadMV(xe, 0); // load model view matrix to VS constant 0
	M_LoadMW(xe, 4); // load (fake) model world matrix to VS constant 4

	/* set the light direction for the pixel shader */
	float lightDirection[] = {0, 0, -1, 0};
	Xe_SetPixelShaderConstantF(xe, 0, lightDirection, 1);



	/* draw cube */
	Xe_SetShader(xe, SHADER_TYPE_PIXEL, sh_ps, 0);
	Xe_SetShader(xe, SHADER_TYPE_VERTEX, sh_vs, 0);
	Xe_SetStreamSource(xe, 0, vb, 0, 12); /* using this vertex buffer */
	Xe_SetIndices(xe, ib); /* ... this index buffer... */
	Xe_SetTexture(xe, 0, &surface_emul); /* ... and this texture */
	Xe_DrawIndexedPrimitive(xe, XE_PRIMTYPE_TRIANGLELIST, 0, 0, max_vertices, 0, nr_primitives);

	if(finishing)
	{
		/* clear to white */
		Xe_SetClearColor(xe, ~0);

		/* resolve (and clear) */
		Xe_Resolve(xe);

		/* wait for render finish */
		while (!Xe_IsVBlank(xe));
		Xe_Sync(xe);
	}
	glPopMatrix();
}

int initVideo()
{
	gfxplane = Xe_CreateTexture(xe, 256 + 0x40, 224, 1, 4 | 0x40, 0);//32bits

	cubAnglez,cubPosx,cubPosx,cubPosz=1.0;

	cubAnglex=2.7;
	cubAngley=1.96;

	M_BuildPersp(&g_proj, 45.0 / 180.0 * M_PI, 640.0/480.0, 1, 200.0);



	surface=*Xe_CreateTexture(xe, 512+0x80, 448, 1, 4 | 0x40, 0);//32bits
}

