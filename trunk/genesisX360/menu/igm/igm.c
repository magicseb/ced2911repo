/***
In Game Menu
***/
#include "libxenon.h"
#include "menu.h"

#include "igm.h"

#define u8 unsigned char;
#define u32 int;

#define USED_PS sh_text_ps
#define USED_VS sh_text_vs

#define ITEM_RETURN_TO_MENU 1
#define ITEM_DISPLAY_INFO 2

#define SELECT_TRUE	1
#define SELECT_FALSE	0

//megadrive.c
//extern displayInfo;
//extern exitRequested;
//extern emulationPaused;

static int maxX=0;
static int isel=0;//deplacement dans l'axe X
static int menuselected=0;//menu choisi

void initIGM()
{
	igmBg=loadPNGFromMemory(igm_bg_png);
	igmPopup=loadPNGFromMemory(igm_popup_png);
	igmPadIco=loadPNGFromMemory(manette_png);
	igmMenuRestart=loadPNGFromMemory(igm_menu_restart_png);
	igmOk=loadPNGFromMemory(igm_ok_png);
	igmCancel=loadPNGFromMemory(igm_cancel_png);
	igmMenuInfo=loadPNGFromMemory(igm_menu_info_png);

	maxX=0;
	isel=0;
	menuselected=0;
}

float nframe=0;

#define FAST_CMP_I_SCALE i++;scale=(isel==i)?1.2f:1.0f;
//Dessine le menu des icones
void drawMenuIcon()
{
	int i=0;
	float scale=1.0f;
	scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmMenuInfo,-0.8,-0.5,0.12*scale,1);//Exit
	i++;scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmMenuRestart,-0.2,-0.5,0.12*scale,1);//Exit
	i++;scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmMenuInfo,0.4,-0.5,0.12*scale,1);//Exit
	maxX=3;
}

void fixisel()
{
	if(isel<=0)
		isel=0;
	if(isel>=maxX)
		isel=maxX-1;
}

void igmDoAction()
{
	switch(menuselected)
	{
		case ITEM_RETURN_TO_MENU:
			if(isel)
			{
				//shutdownEmulation();
				exitRequested=1;
				emulationPaused=0;
				displayInfo=0;
			}
			else
			{
				//Cancel
				resetAction();
			}

			break;
		case ITEM_DISPLAY_INFO:
			if(!isel)
			{
				resetAction();
				emulationPaused=0;
				displayInfo=0;
			}
			break;
		case 0:
			menuselected=isel;
			break;

		default:
			break;

	}
	isel=0;
}

void resetAction()
{
		//Reset Action
		menuselected=0;
		isel=0;
}

struct controller_data_s lastinputc;
int igmInput()
{
	struct controller_data_s c;
	int ret=get_controller_data(&c, 0);

	//Que droite et gauche
	if(c.left&&!lastinputc.left)
	{
		isel--;
		nframe=0;
	}
	if(c.right&&!lastinputc.right)
	{
		isel++;
		nframe=0;
	}
	//Action
	if(c.a&&!lastinputc.a)
	{
		igmDoAction();
	}
	if(c.b&&!lastinputc.b)
	{
		resetAction();
	}
	if(c.logo&&!lastinputc.logo)
	{
		displayInfo=0;
		emulationPaused=0;
	}
	//fix le choix
	fixisel();
	lastinputc=c;
	//usb_do_poll();

	return ret;
}

void drawReturnToMenu()
{
	XeDrawSurface(igmPopup,-0.5,-0.5,1,1);//Popup

	int i=0;
	float scale=1.0f;
	scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmCancel,-0.4,-0.3,0.12*scale,1);//Cancel
	i++;scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmOk,0.2,-0.3,0.12*scale,1);//
	XePrintf(-0.3,0.15,"Exit ?",1,0xffffff);
	maxX=2;
}

void drawInformation()
{
	XeDrawSurface(igmPopup,-0.5,-0.5,1,1);//Popup
	int i=0;
	float scale=1.0f;
	scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmCancel,-0.4,-0.3,0.12*scale,1);//Cancel
	i++;scale=(isel==i)?1.3f:1.0f;
	XeDrawSurface(igmOk,0.2,-0.3,0.12*scale,1);//
	XePrintf(-0.3,0.15,"Some Information about games...",1,0xffffff);
	maxX=2;
}

void updateIGM(int debut, int fin)
{
	if(debut)
		Xe_InvalidateState(xe);

	//Dessine la scene
	XeDrawSurface2(igmBg, -1, -1, 2, 2, 1);
	//event sur la manette
	igmInput();
	switch(menuselected)
	{
		case 0:
			drawMenuIcon();
			break;

		case ITEM_RETURN_TO_MENU:
			drawReturnToMenu();
			break;
		case ITEM_DISPLAY_INFO:
			drawInformation();
			break;
		default:
			drawMenuIcon();
			break;

	}

	if(fin)
	{
		Xe_SetClearColor(xe, ~0);

		/* resolve (and clear) */
		Xe_Resolve(xe);

		/* wait for render finish */
		while (!Xe_IsVBlank(xe));
		Xe_Sync(xe);


	}
	nframe=1.0f;
}

