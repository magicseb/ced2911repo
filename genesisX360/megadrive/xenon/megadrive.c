/**
 megadrive.c

 fichier principal pour la xbox 360

 affiche, gere les joystick, et joue le son

 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <xenos/xe.h>
#include <input/input.h>
#include <time/time.h>
#include <ppc/timebase.h>

#include "shared.h"
#include "drawtexture.h"

#include "megadrive.h"
#include "xenon_video.h"
#include "config.h"

#include "shared.h"

//Valeur utilisé lors de la boucle d'emulation


/** Initialisation de la console **/
void xb360Init()
{
	//xenos_init();
	//console_init();
	xenon_sound_init();
	kmem_init();

	usb_init();
	usb_do_poll();

	//On charge la configuration de l'application (touches vision etc ....)
	default_configuration();
	//on essaie de la lire ....
	//parseConfigFile("uda:/genesis.ini");
}

/***
 Frameskipping
 ***/

unsigned long long prev, now;

unsigned long long gettime(void)
{
	return mftb();
}

int diff_usec(unsigned long long prev, unsigned long long now)
{
	return (now - prev) / (PPC_TIMEBASE_FREQ / 1000000);
}

//Attends jusqu'a la prochaine frame
int frameSkip()
{
	int fpshz;
	if(genFps==50)
	{
		fpshz=20000;//16667
	}
	else
	{
		fpshz=16667;
	}

	now = gettime();

	unsigned long diff = diff_usec(prev, now);

	while (diff_usec(prev, now) < fpshz)
	{
		now = gettime();
		udelay(50);
	}

	prev = now;
}

/***
 INPUT HANDLING
 ***/

//Savoir si une touche est appuyer
#define TESTINPUT(x)	x&&toucheCfg.val[x]
#define TESTOLDINPUT(y)	y&&last_toucheCfg.val[y]
//Savoir si une touche a été appuyer, une sorte de click
#define TESTBTN(z)	(z&&toucheCfg.val[z])&&(!z&&last_toucheCfg.val[z])

#define TTBTN(i)	(i&&toucheCfg.val[i])	&& !(i&&last_toucheCfg.val[i])

struct controller_data_s lastinputc;

int update_input(void)
{
	memset(&input, 0, sizeof(input));
	struct controller_data_s c;
	int ijoy;
	for(ijoy=0;ijoy<4;ijoy++)
	{
		get_controller_data(&c, ijoy);

		//copie la manette dans une structeur plus facile a parcourir
		struct c_input_s toucheCfg = controllerFromConfig(c);
		struct c_input_s last_toucheCfg = controllerFromConfig(lastinputc);

		int iJoystick = ijoy;

		//on applique les boutons
		if (TESTINPUT(globalconfig.up))
		{
			input.pad[iJoystick] |= INPUT_UP;
		}
		if (TESTINPUT(globalconfig.down))
		{
			input.pad[iJoystick] |= INPUT_DOWN;
		}
		if (TESTINPUT(globalconfig.left))
		{
			input.pad[iJoystick] |= INPUT_LEFT;
		}
		if (TESTINPUT(globalconfig.right))
		{
			input.pad[iJoystick] |= INPUT_RIGHT;
		}
		if (TESTINPUT(globalconfig.start))
		{
			input.pad[iJoystick] |= INPUT_START;
		}
		if (TESTINPUT(globalconfig.mode))
		{
			input.pad[iJoystick] |= INPUT_MODE;
		}
		if (TESTINPUT(globalconfig.a))
		{
			input.pad[iJoystick] |= INPUT_A;
		}
		if (TESTINPUT(globalconfig.b))
		{
			input.pad[iJoystick] |= INPUT_B;
		}
		if (TESTINPUT(globalconfig.c))
		{
			input.pad[iJoystick] |= INPUT_C;
		}
		if (TESTINPUT(globalconfig.x))
		{
			input.pad[iJoystick] |= INPUT_X;
		}
		if (TESTINPUT(globalconfig.y))
		{
			input.pad[iJoystick] |= INPUT_Y;
		}
		if (TESTINPUT(globalconfig.z))
		{
			input.pad[iJoystick] |= INPUT_Z;
		}

		if(ijoy==0)
		{
			if (TTBTN(globalconfig.back))
			{
				//on retourne au menu
				//exitRequested=1;
				//emulationPaused=1;
				displayInfo = !displayInfo;
				emulationPaused = displayInfo;
			}

			if (TTBTN(globalconfig.doresize))
			{
				//emulationPaused!=emulationPaused;
				resizeScreen = !resizeScreen;
				emulationPaused = resizeScreen;
			}

			if (resizeScreen)
			{
				if (TESTINPUT(globalconfig.rdown))
				{
					cubAngley -= 0.01;
				}
				if (TESTINPUT(globalconfig.rup))
				{
					cubAngley += 0.01;
				}
				if (TESTINPUT(globalconfig.rleft))
				{
					cubAnglex += 0.01;
				}
				if (TESTINPUT(globalconfig.rright))
				{
					cubAnglex -= 0.01;
				}
			}
			if (displayInfo)
			{

			}
			lastinputc = c;
		}

	}

	//mets à jour les données de la manette
	usb_do_poll();
}

/***
 SOUND
 ***/

void updateSound()
{
	xenonUpdateSound(snd.buffer, snd.buffer_size);
}

static inline uint32_t bswap_32(uint32_t t)
{
	return ((t & 0xFF) << 24) | ((t & 0xFF00) << 8) | ((t & 0xFF0000) >> 8)
			| ((t & 0xFF000000) >> 24);
}

void xenonUpdateSound(int16 **output, int length)
{

	int samples[48000* 16 ];//Grandes valeurs ... ( à revoirs ...)

			//if (xenon_sound_get_unplayed() < samples_guard)
			if(1)
			{
				int i;
				for(i = 0; i < length; i++)
				{
					unsigned int left = (unsigned int*) ((output[0][i]));
					unsigned int right = (unsigned int*) ((output[1][i]));
					left &= 0xFFFF;//strips of the sign-extended bits
					right &= 0xFFFF;//strips of the sign-extended bits
					//on mix les 2 cannaux et on bswap
					//samples[i] = bswap32((left << 32));
					samples[i] = bswap32( (left<<16)|right);
				}

				long sample_size=length*4;// =3200 (48khz*2/60)
				//long sample_size=3200;// =3200 (48khz*2/60)
				if(xenon_sound_get_unplayed()>sample_size)
				{
					//attends que ce soit libre
				}
				else
				{
					//length*4 joue à la bonne vitesse
					xenon_sound_submit(samples,sample_size);//fait des craquements
				}
			}

		}

void _xenonUpdateSound(int16 **output, int length)
{
	int sample_rate = 48000 * 2;

	int samples_per_frame = 1 ? sample_rate / 60 : sample_rate / 50;

	int req_samples = samples_per_frame;

	int samples[48000 / 2];//Grandes valeurs ... ( à revoirs ...)
	int i;
	for (i = 0; i < length * 4; i++)
	{
		unsigned int left = (unsigned int*) output[0][i];
		unsigned int right = (unsigned int*) output[1][i];
		left &= 0xFFFF;//strips of the sign-extended bits
		right &= 0xFFFF;//strips of the sign-extended bits
		//on mix les 2 cannaux et on bswap
		//samples[i] = bswap32((left << 16) | right);
		samples[i] = bswap32((left << 16) | right);
	}
	//length*4 joue à la bonne vitesse
	xenon_sound_submit(samples, length * 4);
}

/**
 MAIN

 ***/

void shutdownEmulation()
{
	exitRequested = 1;
}

//int megadrivemain(char romName)
int megadrivemain(char *romName)
{
	//On attends de charger la rom avant de reinitialiser la video (video deja init par le menu et  elle permet d'afficher des messages ... a termes utiliser une seul fct)

	//init des variables
	exitRequested = 0;
	emulationPaused = 0;
	resizeScreen = 0;
	displayInfo = 0;
	//affichage on screen
	initIGM();

	error_init();

	printf("\t\t\tinit ....Ok\n");
	/* Load game */
	if (strlen(romName) > 2)
	{
		if (!load_rom(romName))
		{
			char caption[256];
			sprintf(caption, "Error loading file `%s'.", romName);
			//MessageBox(NULL, caption, "Error", 0);
			errorPrintf(320, 240, 0x00000000, caption);

			exitRequested = (1);
		}
	}
	else
	{
		char caption[256];
		sprintf(caption, "File `%s' not found .", romName);
		errorPrintf(320, 240, 0x00000000, caption);
		exitRequested = (1);
	}
	//Le jeux doit etre charger a ce moment
	//Initialisation de l'affichage et Initialisation de la console
	xb360Init();
	initVideo();

	printf("\t\t\tmemset\n");
	memset(&bitmap, 0, sizeof(t_bitmap));
	bitmap.width = 256 + 0x40;
	bitmap.height = 224;
	bitmap.depth = 16;
	bitmap.granularity = 2;

	printf("\t\t\tbitmap.pitch\n");

	bitmap.pitch = gfxplane->pitch;
	//bitmap.pitch = (bitmap.width * bitmap.granularity);
	bitmap.data = (uint16*) ((unsigned char*) gfxplane->base);
	bitmap.viewport.w = 256 + 0x40;
	bitmap.viewport.h = 224;
	bitmap.viewport.x = 0x20;
	bitmap.viewport.y = 00;
	bitmap.remap = 1;

	genFps=getFPS();

	printf("\t\t\tsystem_init\n");

	system_init();
	printf("\t\t\tsystem_reset\n");
	system_reset();

	printf("\t\t\trunning");

	audio_init(48000);

	while (exitRequested == 0)
	{

		if (emulationPaused == 0)
		{
			system_frame(0);//avance
			updateSound();
		}
		update_input();
		if (!displayInfo)
		{
			//Pour l'instant on est dans le cas d'un retaillage de l'ecran
			updateVideo(1, 1);
			frameSkip();
		}
		else
		{
			//Mets a jour le menu
			updateVideo(1, 0);//Ne fini pas la scene
			updateIGM(0, 1);
		}
		if (exitRequested == 1)
		{
			break;
		}
	}

	system_shutdown();
	error_shutdown();

	return 0;
}

