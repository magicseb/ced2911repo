#include "libxenon.h"
#include "menu.h"

//utiliser pendant l'update
float xe_framecount=0.0f;

//Initilisation de l'ecran
void initScreen(int width,int height)
{
	xe_framecount=0;

	xe = &_xe;

	/* initialize the GPU */
	Xe_Init(xe);

	/* create a render target (the framebuffer) */
	fb = Xe_GetFramebufferSurface(xe);
	Xe_SetRenderTarget(xe, fb);//le buffer est xe


	//gfxplane = Xe_CreateTexture(xe, width,height, 1, XE_FMT_8888 | XE_FMT_ARGB , 0);//init de la texture 24bit argb
	/* let's define a vertex buffer format */


	/* un rectangle */
	static const struct XenosVBFFormat vbf =
	{
		5, {
		  {XE_USAGE_POSITION, 0, XE_TYPE_FLOAT3},
		  {XE_USAGE_NORMAL,   0, XE_TYPE_FLOAT3},
		  {XE_USAGE_TANGENT,  0, XE_TYPE_FLOAT3},
		  {XE_USAGE_COLOR,    0, XE_TYPE_UBYTE4},
		  {XE_USAGE_TEXCOORD, 0, XE_TYPE_FLOAT2},
		}
	};

		/* a cube */
	float cube[] = {
		//       POSITION           |            NORMAL           |        TANGENT              |   COL   |    U        V    |
		-0.5000 , -0.5000 , -0.5000 , +0.0000 , +0.0000 , -1.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +1.0000,
		-0.5000 , +0.5000 , -0.5000 , +0.0000 , +0.0000 , -1.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000,
		+0.5000 , +0.5000 , -0.5000 , +0.0000 , +0.0000 , -1.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +2.0000 , +0.0000,
		+0.5000 , -0.5000 , -0.5000 , +0.0000 , +0.0000 , -1.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +2.0000 , +1.0000,
		-0.5000 , -0.5000 , +0.5000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +1.0000,
		-0.5000 , +0.5000 , +0.5000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000,
		-0.5000 , +0.5000 , -0.5000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , -1.0000 , +0.0000 , +1.0000 , +0.0000,
		-0.5000 , -0.5000 , -0.5000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , -1.0000 , +0.0000 , +1.0000 , +1.0000,
		+0.5000 , -0.5000 , +0.5000 , +0.0000 , +0.0000 , +1.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +3.0000 , +1.0000,
		+0.5000 , +0.5000 , +0.5000 , +0.0000 , +0.0000 , +1.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +3.0000 , +0.0000,
		-0.5000 , +0.5000 , +0.5000 , +0.0000 , +0.0000 , +1.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +4.0000 , +0.0000,
		-0.5000 , -0.5000 , +0.5000 , +0.0000 , +0.0000 , +1.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +4.0000 , +1.0000,
		+0.5000 , -0.5000 , -0.5000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000 , +2.0000 , +1.0000,
		+0.5000 , +0.5000 , -0.5000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000 , +2.0000 , +0.0000,
		+0.5000 , +0.5000 , +0.5000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000 , +3.0000 , +0.0000,
		+0.5000 , -0.5000 , +0.5000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000 , +3.0000 , +1.0000,
		-0.5000 , +0.5000 , -0.5000 , +0.0000 , +1.0000 , +0.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000,
		-0.5000 , +0.5000 , +0.5000 , +0.0000 , +1.0000 , +0.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000,
		+0.5000 , +0.5000 , +0.5000 , +0.0000 , +1.0000 , +0.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000,
		+0.5000 , +0.5000 , -0.5000 , +0.0000 , +1.0000 , +0.0000 , +1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +1.0000,
		+0.5000 , -0.5000 , -0.5000 , +0.0000 , -1.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000,
		+0.5000 , -0.5000 , +0.5000 , +0.0000 , -1.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000 , +0.0000,
		-0.5000 , -0.5000 , +0.5000 , +0.0000 , -1.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +0.0000,
		-0.5000 , -0.5000 , -0.5000 , +0.0000 , -1.0000 , +0.0000 , -1.0000 , +0.0000 , +0.0000 , +0.0000 , +1.0000 , +1.0000,
	};
	unsigned short cube_indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

	printf("loading pixel shader...\n");
		/* load pixel shader */

	extern unsigned char content_datapspsu[];
	extern unsigned char content_datavsvsu[];

	sh_ps = Xe_LoadShaderFromMemory(xe,  shader_3d_ps);
	Xe_InstantiateShader(xe, sh_ps, 0);

	printf("loading vertex shader...\n");
		/* load vertex shader */

	sh_vs = Xe_LoadShaderFromMemory(xe, shader_3d_vs);
	Xe_InstantiateShader(xe, sh_vs, 0);
	Xe_ShaderApplyVFetchPatches(xe, sh_vs, 0, &vbf);

	//M_BuildPersp(&g_proj, 45.0 / 180.0 * M_PI, 640.0/480.0, 1, 200.0);

	printf("create vb...\n");
	/* create and fill vertex buffer */
	vb = Xe_CreateVertexBuffer(xe, sizeof(cube));
	void *v = Xe_VB_Lock(xe, vb, 0, sizeof(cube), XE_LOCK_WRITE);
	memcpy(v, cube, sizeof(cube));
	Xe_VB_Unlock(xe, vb);

	printf("create ib...\n");
	/* create and fill index buffer */
	ib = Xe_CreateIndexBuffer(xe, sizeof(cube_indices), XE_FMT_INDEX16);
	unsigned short *i = Xe_IB_Lock(xe, ib, 0, sizeof(cube_indices), XE_LOCK_WRITE);
	memcpy(i, cube_indices, sizeof(cube_indices));
	Xe_IB_Unlock(xe, ib);



	//Initialise les shaders (font)
	static const struct XenosVBFFormat vbf_font =
	{
		3, {
			{XE_USAGE_POSITION, 0, XE_TYPE_FLOAT2},
			{XE_USAGE_TEXCOORD, 0, XE_TYPE_FLOAT2},
			{XE_USAGE_COLOR,	0, XE_TYPE_UBYTE4},
		}
	};

	sh_font_ps = Xe_LoadShaderFromMemory(xe, shader_font_ps);
	Xe_InstantiateShader(xe, sh_font_ps, 0);

	sh_font_vs = Xe_LoadShaderFromMemory(xe, shader_font_vs);
	Xe_InstantiateShader(xe, sh_font_vs, 0);
	Xe_ShaderApplyVFetchPatches(xe, sh_font_vs, 0, &vbf_font);


	/* un rectangle */
	static const struct XenosVBFFormat texture_vbf =
	{
		2, {
		  {XE_USAGE_POSITION, 0, XE_TYPE_FLOAT2},
		  {XE_USAGE_TEXCOORD, 0, XE_TYPE_FLOAT2},
		}
	};

	sh_text_ps = Xe_LoadShaderFromMemory(xe,  content_datapspsu);
	Xe_InstantiateShader(xe, sh_text_ps, 0);

	printf("loading vertex shader...\n");
		/* load vertex shader */

	sh_text_vs = Xe_LoadShaderFromMemory(xe, content_datavsvsu);
	Xe_InstantiateShader(xe, sh_text_vs, 0);
	Xe_ShaderApplyVFetchPatches(xe, sh_text_vs, 0, &texture_vbf);

	/* stats */
	printf("render..\n");

	do_edram_foo(xe, 1);

	max_vertices = sizeof(cube)/(sizeof(*cube)*12);
	nr_primitives = sizeof(cube_indices)/sizeof(*cube_indices) / 3;

//Fonds noir jusqu'au prochain affichage
	Xe_InvalidateState(xe);
	Xe_SetClearColor(xe, ~0);
		Xe_Resolve(xe);
	while (!Xe_IsVBlank(xe));
		Xe_Sync(xe);

}
