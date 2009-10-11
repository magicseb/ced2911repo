#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <input/input.h>
#include <xenos/xe.h>
#include "play_sound.h"

static int isSoundHasBeenPlayed=1;

static inline uint32_t bswap_32(uint32_t t)
{
        return ((t & 0xFF) << 24) | ((t & 0xFF00) << 8) | ((t & 0xFF0000) >> 8) | ((t & 0xFF000000) >> 24);
}

void playPcm(unsigned char*data,int length)
{
	if(xenon_sound_get_unplayed()>0)
	{
		//attends que ce soit libre
	}
	else
	{
		xenon_sound_submit(data, length);
	}
}

void sound_over()
{
	playPcm(button_over_pcm,button_over_pcm_size);
}

void sound_select()
{
	playPcm(button_select_pcm,button_select_pcm_size);
}


