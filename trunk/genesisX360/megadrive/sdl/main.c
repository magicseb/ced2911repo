
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include "shared.h"

#define FULLSCREEN_WIDTH	320
#define FULLSCREEN_HEIGHT	240

#define SOUND

int update_input(void);
unsigned char *keystate;

#ifdef SOUND

#define SAMPLE_RATE 44100
#define BUFFER_LENGTH 1024

char *snd_buffer;
char *snd_cpos;
int snd_esamples;

extern DECLSPEC SDL_Joystick * SDLCALL SDL_JoystickOpen(int device_index);

SDL_Joystick *sdlstick;

static void snd_callback(void *userdata, Uint8 *stream, int len)
{
	if(snd_esamples < len)
	{
		memset(stream, 0, len);
	}
	else
	{
		memcpy(stream, snd_buffer, len);
		do
		{
			snd_esamples -= len;
		} while(snd_esamples > 2 * len);
		memcpy(snd_buffer, snd_cpos - snd_esamples, snd_esamples);
		snd_cpos = snd_buffer + snd_esamples;
	}
}

static void snd_init()
{
	int n;
	SDL_AudioSpec desired, obtained;

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		printf("Can't initialise SDL Audio: %s\n", SDL_GetError());
		exit(0);
	}

	desired.freq = SAMPLE_RATE;
	desired.format = AUDIO_S16LSB;
	desired.channels = 2;
	desired.samples = BUFFER_LENGTH;
	desired.callback = snd_callback;

	if(SDL_OpenAudio(&desired, &obtained) == -1)
	{
		printf("Can't initialise SDL Audio: %s\n", SDL_GetError());
		exit(0);
	}

	snd_esamples = 0;
	n = BUFFER_LENGTH * 2 * sizeof(short) * 11;
	snd_buffer = (char *)malloc(n);
	if(!snd_buffer)
	{
		printf("Can't allocate sound buffer\n");
		exit(0);
	}

	memset(snd_buffer, 0, n);
	snd_cpos = snd_buffer;
}

static void snd_update()
{
	int i, out_L, out_R;;
	short *p;

	p = (short *)snd_cpos;
	for(i = 0; i < snd.buffer_size; i++)
	{
		out_L = snd.buffer[0][i];
		out_R = snd.buffer[1][i];
		*p++ = out_L;
		*p++ = out_R;
	}

	snd_cpos = (char *)p;
	snd_esamples += snd.buffer_size * 2 * sizeof(short);

//	int i, out_L, out_R;;
//	short *p;

	SDL_LockAudio();
/*	p = (short *)snd_cpos;
	for(i = 0; i < snd.buffer_size; i++)
	{
		out_L = snd.mixbuf[0][i];
		snd.mixbuf[0][i] = 0;

		if (out_L < -0x7FFF) *p++ = -0x7FFF;
		else if (out_L > 0x7FFF) *p++ = 0x7FFF;
		else *p++ = (short) out_L;

		out_R = snd.mixbuf[1][i];
		snd.mixbuf[1][i] = 0;

		if (out_R < -0x7FFF) *p++ = -0x7FFF;
		else if (out_R > 0x7FFF) *p++ = 0x7FFF;
		else *p++ = (short) out_R;
	}
	snd_cpos = (char *)p;
	snd_esamples += snd.buffer_size * 2 * sizeof(short);*/
	SDL_UnlockAudio();
}

static void snd_close()
{
	SDL_PauseAudio(1);
	SDL_CloseAudio();
	free(snd_buffer);
}

#endif // SOUND

static int is_fullscreen = 0;
static int vm_fullscreen = 0;
static SDL_Rect viewport, srcrect;
static SDL_Surface *screen = NULL;

static void update_window()
{
	int cstate;

	/* Set up source rectangle */
	srcrect.x = 32;
	srcrect.y = 0;
	srcrect.w = bitmap.viewport.w;
	srcrect.h = bitmap.viewport.h;

	cstate = SDL_ShowCursor(0);

	/* Destination rectangle */
	if(is_fullscreen)
	{
		/* Clear buffer (gets rid of junk at sides on resolution change */
		if(screen)
			SDL_FillRect(screen, &viewport, SDL_MapRGB(screen->format, 0, 0, 0));

		/* Reset destination rectangle */
		viewport.x = (FULLSCREEN_WIDTH / 2) - (bitmap.viewport.w / 2);
		viewport.y = (FULLSCREEN_HEIGHT / 2) - (bitmap.viewport.h / 2);
		viewport.w = bitmap.viewport.w;
		viewport.h = bitmap.viewport.h;

		/* Switch to full screen if not already */
		if(!vm_fullscreen)
			screen = SDL_SetVideoMode(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
		vm_fullscreen = 1;

		/* Hide cursor */
		if(!cstate)
			SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		/* Reset destination rectangle */
		viewport.x = 0;
		viewport.y = 0;
		viewport.w = bitmap.viewport.w;
		viewport.h = bitmap.viewport.h;

		/* Resize the window */
		screen = SDL_SetVideoMode(bitmap.viewport.w, bitmap.viewport.h, 16, SDL_SWSURFACE);
		vm_fullscreen = 0;

		/* Hide cursor */
		if(cstate)
			SDL_ShowCursor(SDL_ENABLE);
	}
}

char sramFilename[256];

void findSramFilename(char *romFilename)
{
	int length = strlen(romFilename);
	int i, lastDot = length;

	for(i = 0; i < length; i++)
	{
		if(romFilename[i] == '.')
			lastDot = i;
	}

	memset(sramFilename, 0, sizeof(sramFilename));
	strncpy(sramFilename, romFilename, lastDot);
	strncat(sramFilename, ".gpz", sizeof(sramFilename));
}

int main (int argc, char **argv)
{
	unsigned int next_time = 0;
	unsigned int clock;
	int nstick;

	int cstep = 0;
#ifdef MODE_PAL
	int steps[3] = {20,20,20};
#else
	int steps[3] = {17,16,17};
#endif

	int running = 1;

	SDL_Surface *bmp;
	SDL_Event event;

	error_init();

	/* Print help if no game specified */
	if(argc < 2)
	{
		printf("Genesis Plus\nby Charles MacDonald\nWWW: http://cgfm2.emuviews.com\nusage: %s gamename\n", argv[0]);
		exit(1);
	}

	/* Load game */
	findSramFilename(argv[1]);
	if(!load_rom(argv[1]))
	{
		printf("Error loading file `%s'.\n", argv[1]);
		exit(1);
	}
	
	printf("loading file `%s'.\n", argv[1]);

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) < 0)
	{
		exit(1);
	}
	SDL_WM_SetCaption("Genesis Plus / SDL", NULL);

	printf("Genesis Plus / SDL\n", NULL);

	
	printf("bitmap\n");

	/* Set the viewport (needs to be done before video init) */
	bitmap.viewport.w = 256;
	bitmap.viewport.h = 224;
	bitmap.viewport.x = 0x20;
	bitmap.viewport.y = 0x00;

	/* Initialise video mode */
	update_window();
	bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 512, 16, 0xF800, 0x07E0, 0x001F, 0x0000);

	printf("memset\n");

	memset(&bitmap, 0, sizeof(t_bitmap));
	bitmap.width  = 1024;
	bitmap.height = 512;
	bitmap.depth  = 16;
	bitmap.granularity = 2;
	bitmap.pitch = (bitmap.width * bitmap.granularity);
	bitmap.data   = (unsigned char *)bmp->pixels;
	bitmap.remap = 1;

	printf("system_init\n");
	system_init();

	printf("BUTTON\n");
	input.dev[0] = DEVICE_6BUTTON;
	input.dev[1] = DEVICE_6BUTTON;


	printf("SOUND\n");

#ifdef SOUND
	audio_init(SAMPLE_RATE);
#endif
	system_reset();

#ifdef SOUND
	snd_init();
	SDL_PauseAudio(0);
#endif
	printf("running");
	while(running)
	{
		running = update_input();

		while (SDL_PollEvent(&event))
		{
			switch(event.type) 
			{
				case SDL_QUIT: /* Windows was closed */
					running = 0;
					break;

				default:
					break;
			}
		}

			SDL_PauseAudio(0);		
		
			if(!system_frame(0))
			{
				printf("reset!\n");
				system_reset();
			}
	
			if(bitmap.viewport.changed)
			{
				update_window();
				bitmap.viewport.changed = 0;
			}

			SDL_BlitSurface(bmp, &srcrect, screen, &viewport);
			SDL_UpdateRect(screen, viewport.x, viewport.y, viewport.w, viewport.h);
	
	#ifdef SOUND
			snd_update();
	#endif
	
			/* Framerate limiter */
			clock = SDL_GetTicks();
			if(!keystate[SDLK_q])
			{
				while(clock < next_time)
					clock = SDL_GetTicks();
			}
			next_time = clock + steps[cstep++];
			if(cstep > 2)
				cstep = 0;
	}

	system_shutdown();

	SDL_Quit();
	error_shutdown();

	return 0;
}


/* Check if a key is pressed */
int check_key(int code)
{
	static char lastbuf[0x100] = {0};

	if((!keystate[code]) && (lastbuf[code] == 1))
		lastbuf[code] = 0;

	if((keystate[code]) && (lastbuf[code] == 0))
	{
		lastbuf[code] = 1;
		return (1);
	}

	return (0);
}

int update_input(void)
{
	int running = 1;

	int key_player = 0;
	int joy_player = 1;

	keystate = SDL_GetKeyState(NULL);

	memset(&input, 0, sizeof(t_input));
	if(keystate[SDLK_UP])     input.pad[key_player] |= INPUT_UP;
	else if(keystate[SDLK_DOWN])   input.pad[key_player] |= INPUT_DOWN;
	if(keystate[SDLK_LEFT])   input.pad[key_player] |= INPUT_LEFT;
	else if(keystate[SDLK_RIGHT])  input.pad[key_player] |= INPUT_RIGHT;
	if(keystate[SDLK_a])      input.pad[key_player] |= INPUT_A;
	if(keystate[SDLK_s])      input.pad[key_player] |= INPUT_B;
	if(keystate[SDLK_d])      input.pad[key_player] |= INPUT_C;
	if(keystate[SDLK_f])      input.pad[key_player] |= INPUT_START;
	if(keystate[SDLK_SPACE])      input.pad[key_player] |= INPUT_START;
	if(keystate[SDLK_KP7])      input.pad[key_player] |= INPUT_A;
	if(keystate[SDLK_KP8])      input.pad[key_player] |= INPUT_B;
	if(keystate[SDLK_KP9])      input.pad[key_player] |= INPUT_C;
	if(keystate[SDLK_KP0])      input.pad[key_player] |= INPUT_START;
	if(keystate[SDLK_z])      input.pad[key_player] |= INPUT_X;
	if(keystate[SDLK_x])      input.pad[key_player] |= INPUT_Y;
	if(keystate[SDLK_c])      input.pad[key_player] |= INPUT_Z;
	if(keystate[SDLK_v])      input.pad[key_player] |= INPUT_MODE;

	// Player 2 Controls
	if(sdlstick)
	{
		int dax;
		float ax, ay, alen, aangle;
		SDL_JoystickUpdate();

		// Digital pad
		dax = SDL_JoystickGetAxis(sdlstick, 4);
		if(dax < 0)
			input.pad[joy_player] |= INPUT_LEFT;
		else if(dax > 0)
			input.pad[joy_player] |= INPUT_RIGHT;
		dax = SDL_JoystickGetAxis(sdlstick, 5);
		if(dax < 0)
			input.pad[joy_player] |= INPUT_UP;
		else if(dax > 0)
			input.pad[joy_player] |= INPUT_DOWN;

		// Analog pad (digital emulation)
		ax = SDL_JoystickGetAxis(sdlstick, 0) / 32768.0f;
		ay = SDL_JoystickGetAxis(sdlstick, 1) / 32768.0f;
		alen = sqrt(ax * ax + ay * ay);
		aangle = (atan2(ay, ax) / M_PI) * 180;
		if(alen > 0.5)
		{
			if( (aangle > -67.5) && (aangle < 67.5) )
				input.pad[joy_player] |= INPUT_RIGHT;
			else if( (aangle < -112.5) || (aangle > 112.5) )
				input.pad[joy_player] |= INPUT_LEFT;
			if( (aangle > -157.5) && (aangle < - 22.5) )
				input.pad[joy_player] |= INPUT_UP;
			else if( (aangle < 157.5) && (aangle > 22.5) )
				input.pad[joy_player] |= INPUT_DOWN;
		}

		if(SDL_JoystickGetButton(sdlstick, 3))
			input.pad[joy_player] |= INPUT_A;
		if(SDL_JoystickGetButton(sdlstick, 2))
			input.pad[joy_player] |= INPUT_B;
		if(SDL_JoystickGetButton(sdlstick, 1))
			input.pad[joy_player] |= INPUT_C;
		if(SDL_JoystickGetButton(sdlstick, 8))
			input.pad[joy_player] |= INPUT_START;
		if(SDL_JoystickGetButton(sdlstick, 6))
			input.pad[joy_player] |= INPUT_X;
		if(SDL_JoystickGetButton(sdlstick, 7))
			input.pad[joy_player] |= INPUT_Y;
		if(SDL_JoystickGetButton(sdlstick, 0))
			input.pad[joy_player] |= INPUT_Z;
		if(SDL_JoystickGetButton(sdlstick, 9))
			input.pad[joy_player] |= INPUT_MODE;
	}

	if(keystate[SDLK_TAB])    system_reset();

	/* Fullscreen toggle */
	if(check_key(SDLK_p))
	{
		is_fullscreen ^= 1;
		update_window();
	}

	if(keystate[SDLK_ESCAPE]) running = 0;
	return (running);
}


