/*
    io.c
    I/O controller chip emulation
*/

#include "shared.h"

port_t port[3];
uint8 io_reg[0x10];


void io_reset(void)
{
    /* I/O register default settings */
    uint8 io_def[0x10] =
    {
        0xA0,
        0x7F, 0x7F, 0x7F,
        0x00, 0x00, 0x00, 
        0xFF, 0x00, 0x00,
        0xFF, 0x00, 0x00,  
        0xFB, 0x00, 0x00,  
    };

    /* Initialize I/O registers */
    memcpy(io_reg, io_def, 0x10);

    /*
        Port A : 3B pad
        Port B : Unused
        Port C : Unused
    */

//port0->1er manette
//port1->2eme manette
    port[0].data_w = device_6b_w1;   
    port[0].data_r = device_6b_r1;
    port[1].data_w = device_6b_w2;
    port[1].data_r = device_6b_r2;
    port[2].data_w = NULL;
    port[2].data_r = NULL;
}

/*--------------------------------------------------------------------------*/
/* I/O chip functions                                                       */
/*--------------------------------------------------------------------------*/

void gen_io_w(int offset, int value)
{
    switch(offset)
    {
        case 0x01: /* Port A Data */
            value = ((value & 0x80) | (value & io_reg[offset+3]));
            io_reg[offset] = value;
            if(port[0].data_w) port[0].data_w(value);
            return;

        case 0x02: /* Port B Data */
            value = ((value & 0x80) | (value & io_reg[offset+3]));
            io_reg[offset] = value;
            if(port[1].data_w) port[1].data_w(value);
            return;

        case 0x03: /* Port C Data */
            value = ((value & 0x80) | (value & io_reg[offset+3]));
            io_reg[offset] = value;
            if(port[2].data_w) port[2].data_w(value);
            return;  

        case 0x04: /* Port A Ctrl */
        case 0x05: /* Port B Ctrl */
        case 0x06: /* Port C Ctrl */
            io_reg[offset] = value & 0xFF;
            break;

        case 0x07: /* Port A TxData */
        case 0x0A: /* Port B TxData */
        case 0x0D: /* Port C TxData */
            io_reg[offset] = value;
            break;

        case 0x09: /* Port A S-Ctrl */
        case 0x0C: /* Port B S-Ctrl */
        case 0x0F: /* Port C S-Ctrl */
            io_reg[offset] = (value & 0xF8);
            break;
    }
}

int gen_io_r(int offset)
{
    uint8 temp;
    uint8 has_scd = 0x20; /* No Sega CD unit attached */
    uint8 gen_ver = 0x00; /* Version 0 hardware */

    switch(offset)
    {
        case 0x00: /* Version */
            switch(READ_BYTE(cart_rom, 0x0001F0))
            {
                case 'J':
                    temp = 0x00;
                    break;
                case 'U':
                    temp = 0x80;
                    break;
                case 'E':
                    temp = 0xC0;
                    break;
                case 'A':
                    temp = 0xC0;
                    break;
                case 'B':
                    temp = 0xC0;
                    break;
                case '4':
                    temp = 0x80;
                    break;
                default:
                    temp = 0x80;
                    break;
            }
            return (temp | has_scd | gen_ver);
            break;

        case 0x01: /* Port A Data */
            if(port[0].data_r) return ((io_reg[offset] & 0x80) | port[0].data_r());
            return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));

        case 0x02: /* Port B Data */
            if(port[1].data_r) return ((io_reg[offset] & 0x80) | port[1].data_r());
            return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));

        case 0x03: /* Port C Data */
            if(port[2].data_r) return ((io_reg[offset] & 0x80) | port[2].data_r());
            return (io_reg[offset] | ((~io_reg[offset+3]) & 0x7F));
    }

    return (io_reg[offset]);
}

/*--------------------------------------------------------------------------*/
/* Input callbacks                                                          */
/*--------------------------------------------------------------------------*/

uint8 pad_2b_r(void)
{
    uint8 temp = 0x3F;
    if(input.pad[0] & INPUT_UP)    temp &= ~0x01;
    if(input.pad[0] & INPUT_DOWN)  temp &= ~0x02;
    if(input.pad[0] & INPUT_LEFT)  temp &= ~0x04;
    if(input.pad[0] & INPUT_RIGHT) temp &= ~0x08;
    if(input.pad[0] & INPUT_B)     temp &= ~0x10;
    if(input.pad[0] & INPUT_C)     temp &= ~0x20;
    return (temp);
}

static int th = 0;

uint8 device_3b_r(void)
{
    uint8 temp = 0x3F;

    if(th)
    {
        temp = 0x3f;
        if(input.pad[0] & INPUT_UP)    temp &= ~0x01;
        if(input.pad[0] & INPUT_DOWN)  temp &= ~0x02;
        if(input.pad[0] & INPUT_LEFT)  temp &= ~0x04;
        if(input.pad[0] & INPUT_RIGHT) temp &= ~0x08;
        if(input.pad[0] & INPUT_B)     temp &= ~0x10;
        if(input.pad[0] & INPUT_C)     temp &= ~0x20;
        return (temp | 0x40);
    }
    else
    {
        temp = 0x33;
        if(input.pad[0] & INPUT_UP)    temp &= ~0x01;
        if(input.pad[0] & INPUT_DOWN)  temp &= ~0x02;
        if(input.pad[0] & INPUT_A)     temp &= ~0x10;
        if(input.pad[0] & INPUT_START) temp &= ~0x20;
        return (temp);
    }
}

void device_3b_w(uint8 data)
{
    th = (data & 0x40);
}


/**
Manette 6 Boutons :)


? = Whatever was last written to bit 7.
S = Start
A = Button A
B = Button B
C = Button C
U = Up
D = Down
L = Left
R = Right
M = Mode
X = Button X
Y = Button Y
Z = Button Z



3btn
TH = 0 : ?0SA00DU
TH = 1 : ?1CBRLDU
6btn
TH = 1 : ?1CBRLDU    3-button pad return value
TH = 0 : ?0SA00DU    3-button pad return value
TH = 1 : ?1CBRLDU    3-button pad return value
TH = 0 : ?0SA0000    D3-0 are forced to '0'
TH = 1 : ?1CBMXYZ    Extra buttons returned in D3-0
TH = 0 : ?0SA1111    D3-0 are forced to '1'

**/
struct device_6b_s
{
	short th;//th or tl
	short btnCount;//0>6
	int scanline;//0->25
};

struct device_6b_s pad6b_t[2];


uint32 device_6b_r_players(int pad)
{
	uint32 temp;

	if(pad6b_t[pad].btnCount>7)	
	{
		pad6b_t[pad].btnCount=0;
	}

	if(pad6b_t[pad].th)
	{
		temp = 0x7f;
		if(input.pad[pad] & INPUT_B)     temp &= ~0x10;
		if(input.pad[pad] & INPUT_C)     temp &= ~0x20;
		if(pad6b_t[pad].btnCount == 6)//?1CBMXYZ
		{
			if(input.pad[pad] & INPUT_Z)    temp &= ~0x01;
			if(input.pad[pad] & INPUT_Y)  	temp &= ~0x02;
			if(input.pad[pad] & INPUT_X)  	temp &= ~0x04;
			if(input.pad[pad] & INPUT_MODE) temp &= ~0x08;
		}
		else//1CBRLDU
		{
			if(input.pad[pad] & INPUT_UP)    temp &= ~0x01;
			if(input.pad[pad] & INPUT_DOWN)  temp &= ~0x02;
			if(input.pad[pad] & INPUT_LEFT)  temp &= ~0x04;
			if(input.pad[pad] & INPUT_RIGHT) temp &= ~0x08;
		}
		
	}
	else
	{
		temp = 0x33;
		if(input.pad[pad] & INPUT_A)     temp &= ~0x10;
		if(input.pad[pad] & INPUT_START) temp &= ~0x20;
		if(pad6b_t[pad].btnCount == 5)//?0SA0000
                {
                        temp &= 0xF0;
                }
                else if(pad6b_t[pad].btnCount == 7)//?0SA1111
                {
                        temp |= 0x0F;
                }
                else//?0SA00DU 
                {
                        if(input.pad[pad] & INPUT_UP)    temp &= ~0x01;
                        if(input.pad[pad] & INPUT_DOWN)  temp &= ~0x02;
                }
	}
	pad6b_t[pad].btnCount++;
	return temp;
}

uint32 device_6b_r1(void)
{
	return	device_6b_r_players(0);
}

uint32 device_6b_r2(void)
{
	return	device_6b_r_players(1);
}

void device_6b_w(uint8 data,int pad)
{
	uint8 temp = (data & 0x40);

	pad6b_t[pad].th=temp;
}

void device_6b_w1(uint8 data)
{
	device_6b_w(data,0);
}

void device_6b_w2(uint8 data)
{
	device_6b_w(data,1);
}
