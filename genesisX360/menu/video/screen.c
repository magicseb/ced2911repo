#include "libxenon.h"
#include "menu.h"
#include "drawtexture.h"

void doloading()
{
//	XeBgInfo = loadPNGFromMemory(loading_png);

	Xe_InvalidateState(xe);

	XeDrawSurface2(XeBgInfo, -1, -1, 2, 2, 0);

	//Blanc ....
	Xe_SetClearColor(xe, ~0);
	Xe_Resolve(xe);

	while (!Xe_IsVBlank(xe));
	Xe_Sync(xe);
}

void initTextureFromMemory()
{
	XeFont = loadPNGFromMemory(verdana_0_png);
	XeBgInfo = loadPNGFromMemory(loading_png);
	XeLoadingForm = loadPNGFromMemory(loading_form_png);
}

/**
 Initialisation de l'ecran
 **/
void initTextureFromFile()
{
	printf("\t\t\tCréation des textures ...!!\n");
	//Texture frequement utilisé

	printf("\t\t\tXeGetBitmapFromMemory(); ...!!\n");
	//XeBackground=XeGetBitmapFromMemory(1280,720,background);
	//XeFileIcon=XeGetBitmapFromMemory(64,64,xenonFileData);
	//XeFolderIcon=XeGetBitmapFromMemory(64,64,xenonFolderData);
	//XeCursor=XeGetBitmapFromMemory(64,64,xenonCursorData);
	if (XeBackground = loadPNG("uda:/mdbg.png") == 0)
	{
		XeBackground = loadPNGFromMemory(bg_png);
	}
	if (XeFileIcon = loadPNG("uda:/mdfile.png") == 0)
	{
		XeFileIcon = loadPNGFromMemory(file_png);
	}
	if (XeFolderIcon = loadPNG("uda:/mddir.png") == 0)
	{
		XeFolderIcon=loadPNGFromMemory(folder_png);
	}
	if (XeCursor = loadPNG("uda:/mdcurs.png") == 0)
	{
		XeCursor = loadPNGFromMemory(star_png);
	}

	printf("\t\t\tCa marche !!\n");
}
