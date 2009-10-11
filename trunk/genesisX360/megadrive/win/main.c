
#include <windows.h>
#include <SDL.h>
#include "shared.h"

int timer_count = 0;
int old_timer_count = 0;
int paused = 0;
int frame_count = 0;

int update_input(void);
unsigned char *keystate;


Uint32 fps_callback(Uint32 interval)
{
	if(paused) return 1000/60;
	timer_count++;
	if(timer_count % 60 == 0)
	{
		int fps = frame_count;
		char caption[32];
        sprintf(caption, "Genesis Plus/SDL    FPS=%d", fps);
		SDL_WM_SetCaption(caption, NULL);
		frame_count = 0;
	}
	return 1000/60;
}


int main (int argc, char **argv)
{
	int running = 1;

    SDL_Rect viewport, src;
    SDL_Surface *bmp, *screen;
    SDL_Event event;

    error_init();

    /* Print help if no game specified */
	if(argc < 2)
	{
		char caption[256];
        sprintf(caption, "Genesis Plus\nby Charles MacDonald\nWWW: http://cgfm2.emuviews.com\nusage: %s gamename\n", argv[0]);
        MessageBox(NULL, caption, "Information", 0);
		exit(1);
	}

	/* Load game */
    if(!load_rom(argv[1]))
	{
		char caption[256];
		sprintf(caption, "Error loading file `%s'.", argv[1]);
		MessageBox(NULL, caption, "Error", 0);
		exit(1);
	}
        
	viewport.x = 0;
	viewport.y = 0;
    viewport.w = 256;
    viewport.h = 224;

    src.x = 32;
    src.y = 0;
	src.w = viewport.w;
	src.h = viewport.h;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        exit(1);
    }
    SDL_WM_SetCaption("Genesis Plus/SDL", NULL);

    screen = SDL_SetVideoMode(viewport.w, viewport.h, 16, SDL_SWSURFACE);
    viewport.x = 0;
    viewport.y = 0;

    bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 512, 16, 0xF800, 0x07E0, 0x001F, 0x0000);



    memset(&bitmap, 0, sizeof(t_bitmap));
    bitmap.width  = 1024;
    bitmap.height = 512;
    bitmap.depth  = 16;
    bitmap.granularity = 2;
    bitmap.pitch = (bitmap.width * bitmap.granularity);
    bitmap.data   = (unsigned char *)bmp->pixels;
    bitmap.viewport.w = 256;
    bitmap.viewport.h = 224;
    bitmap.viewport.x = 0x20;
    bitmap.viewport.y = 0x00;
    bitmap.remap = 1;

    system_init();
    system_reset();

    SDL_SetTimer(1000/60, fps_callback);

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

				case SDL_ACTIVEEVENT: /* Window focus changed or was minimized */
					if(event.active.state & (SDL_APPINPUTFOCUS | SDL_APPACTIVE))
					{
						paused = !event.active.gain;
					}
					break;

				default:
					break;
			}
		}

		if(!paused)
		{
			frame_count++;

            update_input();

            if(!system_frame(0))
                system_reset();

            if(bitmap.viewport.changed)
            {
                bitmap.viewport.changed = 0;
                src.w = bitmap.viewport.w;
                src.h = bitmap.viewport.h;
                viewport.w = bitmap.viewport.w;
                viewport.h = bitmap.viewport.h;
                screen = SDL_SetVideoMode(bitmap.viewport.w, bitmap.viewport.h, 16, SDL_SWSURFACE);
            }

            SDL_BlitSurface(bmp, &src, screen, &viewport);
            SDL_UpdateRect(screen, viewport.x, viewport.y, viewport.w, viewport.h);
        }
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



