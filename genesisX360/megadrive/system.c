/*
    Copyright (C) 1999, 2000, 2001, 2002, 2003  Charles MacDonald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "shared.h"
#include "utils.h"

t_bitmap bitmap;
t_input input;
t_snd snd;
static int sound_tbl[262];

#define SND_SIZE (snd.buffer_size * sizeof(int16))


int audio_init(int rate)
{
    int i;

    /* 68000 and YM2612 clock */
    float vclk = 53693175.0 / 7;//Clock NTSC

    /* Z80 and SN76489 clock */
    float zclk = 3579545.0;

    /* Clear the sound data context */
    memset(&snd, 0, sizeof(snd));

    /* Make sure the requested sample rate is valid */
    if(!rate || ((rate < 8000) | (rate > 480000)))
    {
        return (0);
    }

    /* Calculate the sound buffer size */
    snd.buffer_size = (rate / genGetFPS());
    snd.sample_rate = rate;

    /* Allocate sound buffers */
    snd.buffer[0] = malloc(snd.buffer_size * sizeof(int16));
    snd.buffer[1] = malloc(snd.buffer_size * sizeof(int16));

    snd.fm.buffer[0] = malloc(snd.buffer_size * sizeof(int16));
    snd.fm.buffer[1] = malloc(snd.buffer_size * sizeof(int16));

    snd.psg.buffer = malloc(snd.buffer_size * sizeof(int16));
    snd.dac.buffer = malloc(snd.buffer_size * sizeof(int16));

    /* Make sure we could allocate everything */
    if(!snd.buffer[0] || !snd.buffer[1] || !snd.fm.buffer[0] || !snd.fm.buffer[1] || !snd.psg.buffer)
    {
        return (0);
    }

    /* Initialize sound chip emulation */
#if USE_SOUND_SMS
    SN76489_Init(1,(int)zclk,rate);
#else
    SN76496_sh_start(zclk, 100, rate);
#endif

    YM2612Init(1, vclk, rate, NULL, NULL);

    /* Set audio enable flag */
    snd.enabled = 1;

    /* Make sound table */
    for (i = 0; i < 262; i++)
    {
        float p = snd.buffer_size * i;
        p = p / 262;
    	sound_tbl[i] = p;
    }

    return (0);
}

void system_init(void)
{
	printf("gen_init");
    gen_init();
	printf("vdp_init");
    vdp_init();
	printf("render_init");
    render_init();
}

void system_reset(void)
{
    gen_reset();
    vdp_reset();
    render_reset();

    if(snd.enabled)
    {
        YM2612ResetChip(0);
        memset(snd.buffer[0], 0, SND_SIZE);
        memset(snd.buffer[1], 0, SND_SIZE);
        memset(snd.fm.buffer[0], 0, SND_SIZE);
        memset(snd.fm.buffer[1], 0, SND_SIZE);
        memset(snd.psg.buffer, 0, SND_SIZE);
        memset(snd.dac.buffer, 0, SND_SIZE);
    }
}

void system_shutdown(void)
{
    gen_shutdown();
    vdp_shutdown();
    render_shutdown();
}


int system_frame(int do_skip)
{
    int line;

    if(gen_running == 0)
        return 0;

    /* Clear V-Blank flag */
    status &= ~0x0008;

    /* Toggle even/odd flag (IM2 only) */
    if(im2_flag)
        status ^= 0x0010;

    /* Point to start of sound buffer */
    snd.fm.lastStage = snd.fm.curStage = 0;
    snd.psg.lastStage = snd.psg.curStage = 0;
    snd.dac.lastStage = snd.dac.curStage = 0;

    /* Parse sprites for line 0 (done on line 261) */
    parse_satb(0x80);

    for(line = 0; line < 262; line += 1)
    {
        /* Used by HV counter */
        v_counter = line;

        /* Run 68000 emulation */
        m68k_execute(487);
        if(!gen_running) break;

        /* Run Z80 emulation (if enabled) */
        if(zreset == 1 && zbusreq == 0)
        {
            z80_execute(228);
            if(!gen_running) break;
        }

        /* If a Z80 interrupt is still pending after a scanline, cancel it */
        if(zirq == 1)
        {
            zirq = 0;
            z80_set_irq_line(0, CLEAR_LINE);
        }

        /* Render a line of the display */
        if(do_skip == 0)
        {
            if(line <  frame_end   ) render_line(line);
            if(line <  frame_end-1 ) parse_satb(0x81 + line);
        }

        /* Do H interrupt processing */
        if(line <= frame_end)
        {
            counter -= 1;
            if(counter == -1)
            {
                counter = reg[10];
                hint_pending = 1;
                if(reg[0] & 0x10)
                {
                    m68k_set_irq(4);
                }
            }
        }
        else
        {
            counter = reg[10];
        }

        /* Do end of frame processing */
        if(line == frame_end)
        {
            status |= 0x0088;
            vint_pending = 1;

            /* Give enough time to read the interrupt pending flag before
               the interrupt actually occurs. */
            m68k_execute(16);
            if(!gen_running) break;

            if(reg[1] & 0x20)
            {
                m68k_set_irq(6);
            }

            if(zreset == 1 && zbusreq == 0)
            {
                z80_set_irq_line(0, ASSERT_LINE);
                zirq = 1;
            }
        }

        fm_update_timers();

        snd.fm.curStage = sound_tbl[line];
        snd.psg.curStage = sound_tbl[line];
        snd.dac.curStage = sound_tbl[line];

    }

    if(snd.enabled)
    {
        audio_update();
    }

    return gen_running;
}

#define CLIP(x)		if(x>32765){x=32765;}else if(x<-32765){x=-32765;}
#define PSG_VOL 3
#define FM_VOL 2
static int ll, rr;
void audio_update(void)
{
    int i;
    int16 tmp,acc;
    int16 *tempBuffer[2];

    tempBuffer[0] = snd.fm.buffer[0];
    tempBuffer[1] = snd.fm.buffer[1];

    //Sound update
    if (snd.buffer_size > 0)
    {
    	//Update FM Sound
    	YM2612UpdateSynth(0, (int16 **)tempBuffer, snd.buffer_size);
    	//Update PSG
#if USE_SOUND_SMS
    	SN76489_Update(0, snd.psg.buffer, snd.buffer_size);
#else
    	SN76496Update(0, snd.psg.buffer, snd.buffer_size);
#endif
		//Update Dac
    	YM2612UpdateDacAndTimers(0, snd.dac.buffer + snd.dac.lastStage, snd.buffer_size - snd.dac.lastStage);
    }

    for(i = 0; i < snd.buffer_size; i ++)
    {

        int16 psg = snd.psg.buffer[i];
        CLIP(psg)
        psg=  psg/ PSG_VOL;
        psg+= snd.dac.buffer[i];
    	//int16 psg = snd.psg.buffer[i] / 3;
        acc = snd.fm.buffer[0][i];
        CLIP(acc)
        acc = acc/FM_VOL;
        acc += psg;
        CLIP(acc)
        snd.buffer[0][i] = acc;

        acc = snd.fm.buffer[1][i];
		CLIP(acc)
		acc = acc/FM_VOL;
        acc += psg;
        CLIP(acc)
        snd.buffer[1][i] = acc;
    }
}
