#include "libxenon.h"
#include "menu.h"

#define DBG_CC 0

//les textures
dirInfo_s dirInfo;

//int iSelectedItem;
/***
 ETAT DE L'APPLICATION
 ***/

/**
 Lance l'emulateur ....
 **/
void doEmulation()
{
	char newDir[256];
	sprintf(newDir, "%s/%s", dirInfo.currentDirname,
			dirInfo.node[iSelectedItem].name);
	megadrivemain((newDir));
}

/**
 Une action est a realisé
 **/
void HandleAction()
{
	if (dirInfo.node[iSelectedItem].type == 1)//Dir
	{

		char newDir[256];
		sprintf(newDir, "%s/%s", dirInfo.currentDirname,
				dirInfo.node[iSelectedItem].name);
		//updateCurDir(newDir);
		dirInfo = updateCurDir(newDir);
		//listFile(1,newDir);//mets a jour ....
	}
	else if (dirInfo.node[iSelectedItem].type == 0)//A tester si c une rom !!
	{
		emuLoaded = 1;
	}
}

//Liste un dossier
void listFile(int refresh, char *newdirname)
{

	if (refresh)
	{
		dirInfo = updateCurDir(newdirname);
	}
	dirInfo_s directory = dirInfo;
#if !DBG_CC
	if (dirInfo.count > 0)
	{
		int i;

		int iStart;//=0;
		int iEnd;//=nItemOnScreen;
		int NBPage = ceil(dirInfo.count / nItemOnScreen);//Nombre de pages
		int selPage = iSelectedItem / nItemOnScreen;//Page selectionner

		if (selPage > NBPage)
		{
			selPage = NBPage;
		}

		iStart = selPage * nItemOnScreen;
		iEnd = iStart + (nItemOnScreen - 1);
		if (iEnd >= dirInfo.count)
		{
			if (dirInfo.count > 0)
				iEnd = dirInfo.count - 1;
			else
				iEnd = 0;
		}

		int z = 0;
		//printf("Affiche la page:%d/%d\niSelectedItem=%d\t selPage=%d\t iStart=%d\tiEnd=%d\tCount=%d\n",selPage , NBPage, iSelectedItem ,selPage, iStart, iEnd,FatLS.count );
		for (i = iStart; i <= iEnd; i++)
		{
			float scale = 1;
			int color = 0xaaaaaa;
			if (i == iSelectedItem)
			{
				scale = bound(xe_framecount) + 1.2f;
				color = 0xffffff;
			}
			const char dname[256];

			sprintf(dname, "%s", directory.node[i].name);
			//printf("%s\n",dname);
			//Affiche le nom de fichier
			XePrintf(-.4, ((ROWHEIGHT * (z) + DECALAGEROWS) + 0.05), dname,
					scale * 0.3, color);

			if (directory.node[i].type == 0)
			{
				XeDrawSurface(XeFileIcon, -.6, (ROWHEIGHT * z) + DECALAGEROWS,
						.06* scale , 1);
			}
			else
			{
				XeDrawSurface(XeFolderIcon, -.6,
						(ROWHEIGHT * z) + DECALAGEROWS, .06* scale , 1);
			}
			z++;

		}
	}
	else
	{
		//Aucun fichier trouver
		XePrintf(0.1, (0.5), "No file Found");
	}
	//Libere les ressources
#endif
}

/**
 **/
void DrawMenu()
{
#if !DBG_CC
	//Dessine le background
	//XeDrawSurface(XeBackground,0,0,
	Xe_InvalidateState(xe);

	XeDrawSurface2(XeBackground, -1, -1, 2, 2, 0);

	//updateAnim();

	listFile(0, "");//Liste les fichiers

	xe_framecount += 1.0f;

	//Blanc ....
	Xe_SetClearColor(xe, ~0);
	Xe_Resolve(xe);

	while (!Xe_IsVBlank(xe));
		Xe_Sync(xe);
#endif
}

//eteint la console
void shutdown()
{
	xenon_smc_power_shutdown();
}

//Créer une valeur de rebonds a partir d'une valeur (entre 0 et 2)
float bound(int f)
{
	float time = f / 300.0f;//obtient une valeur reel
	float ampl = 0.2f; // amplitude en pixel
	int freq = 3; // oscillations par seconde
	int decroi = 1; // facteur décroissance
	float u = ampl * sin(freq * time * 2 * BOUND_PI) * exp(-decroi * time);
	return u;
}

/***
 Frameskipping
 ***/

static unsigned long long prev, now;

static unsigned long long gettime(void)
{
	return mftb();
}

static int diff_usec(unsigned long long prev, unsigned long long now)
{
	return (now - prev) / (PPC_TIMEBASE_FREQ / 1000000);
}

//Attends jusqu'a la prochaine frame
static int menuframeSkip()
{
	int fpshz = (1) ? 16667 : 20000;//16667
	now = gettime();

	unsigned long diff = diff_usec(prev, now);

	while (diff_usec(prev, now) < fpshz)
	{
		now = gettime();
		udelay(50);
	}

	prev = now;
}

void mainInit()
{
	//Init Var
	iSelectedItem = 0;//Item Selectionner
	emuLoaded = 0;

	//init
	xenos_init();
	//console_init();
	xenon_sound_init();

	kmem_init();
	usb_init();
	usb_do_poll();

	//
	initScreen(1280, 720);
	//Loading screen ....
	initTextureFromMemory();
	doloading();
	//Init texture
	initTextureFromFile();

	//updateScreen(1280,720);
	usb_do_poll();
	//while(1);
}

//A ne faire qu'une seul fois
void firstInit()
{
	xenon_smc_set_led(0x01, 0x10);
	xenon_thread_startup();
	xenon_make_it_faster(1);
	xenon_sleep_thread(1);
	xenon_sleep_thread(3);
	xenon_sleep_thread(4);
	xenon_sleep_thread(5);
}

void menu()
{
	//Se place à la racine de la premiere clef usb
	printf("Debut\n");
	listFile(1, "uda:");
	emuLoaded = 0;//Emu pas lancer car on est dans le menu
	printf("FIni\n");

	//On affiche une premiere fois
	DrawMenu();//Dessine le menu (list des fichiers)

	//Boucle du menu
	while (emuLoaded == 0)
	{
		struct controller_data_s c;
		if (get_controller_data(&c, 0))
		{
			pushBtn(c);
		}
		DrawMenu();//Dessine le menu (list des fichiers)
		menuframeSkip();
		usb_do_poll();
	}
	//Lance l'emulation et quitte cette boucle
	doEmulation();
	//Emulation quitter
	emuLoaded = 0;
}

void cclf()
{
	struct dirent *lecture;
	DIR *rep;
	rep = opendir("uda:");
	while ((lecture = readdir(rep)))
	{
		printf("0x%02x--%s\n", lecture->d_type, lecture->d_name);
	}
}

/**
 Le but principal est de jamais quitter le main
 **/
int main(void)
{
	mainInit();
	firstInit();
	//cclf();
	while (1)
	{
		//Affiche le menu
		menu();
		//Menu quitter on reinitialise le menu pour le prochain tour
		mainInit();
	}
	return 0;
}
