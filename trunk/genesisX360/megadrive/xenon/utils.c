#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <xenos/xe.h>
#include <input/input.h>
#include <time/time.h>
#include <ppc/timebase.h>
#include "shared.h"

/**
 * Information
 * http://www.zophar.net/fileuploads/2/10614uauyw/Genesis_ROM_Format.txt
 */

int getWidth()
{
	return (reg[12] & 1) ? 320 : 256;//0x0C (width)
}

int getHeight()
{
	return (reg[1]  & 8) ? 240 : 224;//1 Height
}

/*
E = Europe
J = Japan
U = USA
A = Asia
B = Brazil
4 = Brazil
F = France
8 = Hong Kong
*/
unsigned char getRegion()
{
	return READ_BYTE(cart_rom, 0x0001F0);
}

int getFPS()
{
	unsigned char region=getRegion();
	switch(region)
	{
		case 'E':
		case 'F':
		{
			return 50;
			break;
		}
		case 'J':
		case 'U':
		case 'A':
		case '4':
		case '8':
		{
			return 60;
			break;
		}
		default:
		{
			return 60;
			break;
		}
	}
}

unsigned char *getGameName()
{
	char gname[48];
	int i=0,end=0;
	while(i<48)
	{
		gname[i]=READ_BYTE(cart_rom, 0x000150+i);
		i++;
	}

	//simplifie le nom
	while((!end)&&(i>0))
	{
		if(gname[i]!=0x20)
		{
			gname[i++]='\0';
			end=1;
		}
	}
	return gname;
}
