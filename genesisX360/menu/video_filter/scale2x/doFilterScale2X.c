/*
 * doFilterScale2X.c
 *
 *  Created on: 9 oct. 2009
 *      Author: ced
 */

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

#define READINT24(x)      ((x)[2]<<16 | (x)[1]<<8 | (x)[0])
#define WRITEINT24(x, i)  {(x)[2]=i>>16; (x)[1]=(i>>8)&0xff; x[0]=i&0xff; }

#define Uint8 unsigned char
#define Uint16 unsigned short
#define Uint32 unsigned int

void dofilterscale2x(int srcWidth,int srcHeight,int srcPitch,int dstPitch,void *XeSrc,void *XeDst,int bypp)
{
	//
	int looph,loopw;
	Uint16* src = (Uint16*)XeSrc;
	Uint16* dst = (Uint16*)XeDst;
	bypp=4;

	switch(bypp)
	{
		case 1:
		{
			unsigned char E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
			{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = *(Uint8*)(src + (MAX(0,looph-1)*srcPitch) + (1*loopw));
					D = *(Uint8*)(src + (looph*srcPitch) + (1*MAX(0,loopw-1)));
					E = *(Uint8*)(src + (looph*srcPitch) + (1*loopw));
					F = *(Uint8*)(src + (looph*srcPitch) + (1*MIN(srcWidth-1,loopw+1)));
					H = *(Uint8*)(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (1*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					*(Uint8*)(dst + looph*2*dstPitch + loopw*2*1) = E0;
					*(Uint8*)(dst + looph*2*dstPitch + (loopw*2+1)*1) = E1;
					*(Uint8*)(dst + (looph*2+1)*dstPitch + loopw*2*1) = E2;
					*(Uint8*)(dst + (looph*2+1)*dstPitch + (loopw*2+1)*1) = E3;
				}
			}
			break;
		};

		case 27:
		{
			Uint32 E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
				{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = *(Uint32*)(src + (MAX(0,looph-1)*srcPitch) + (3*loopw));
					D = *(Uint32*)(src + (looph*srcPitch) + (3*MAX(0,loopw-1)));
					E = *(Uint32*)(src + (looph*srcPitch) + (3*loopw));
					F = *(Uint32*)(src + (looph*srcPitch) + (3*MIN(srcWidth-1,loopw+1)));
					H = *(Uint32*)(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (3*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					*(Uint32*)(dst + looph*2*dstPitch + loopw*2*3) = E0;
					*(Uint32*)(dst + looph*2*dstPitch + (loopw*2+1)*3) = E1;
					*(Uint32*)(dst + (looph*2+1)*dstPitch + loopw*2*3) = E2;
					*(Uint32*)(dst + (looph*2+1)*dstPitch + (loopw*2+1)*3) = E3;
				}
			}
			break;
		}
		case 2:
		{
			Uint16 E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
			{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = *(Uint16*)(src + (MAX(0,looph-1)*srcPitch) + (2*loopw));
					D = *(Uint16*)(src + (looph*srcPitch) + (2*MAX(0,loopw-1)));
					E = *(Uint16*)(src + (looph*srcPitch) + (2*loopw));
					F = *(Uint16*)(src + (looph*srcPitch) + (2*MIN(srcWidth-1,loopw+1)));
					H = *(Uint16*)(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (2*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					*(Uint16*)(dst + looph*2*dstPitch + loopw*2*2) = E0;
					*(Uint16*)(dst + looph*2*dstPitch + (loopw*2+1)*2) = E1;
					*(Uint16*)(dst + (looph*2+1)*dstPitch + loopw*2*2) = E2;
					*(Uint16*)(dst + (looph*2+1)*dstPitch + (loopw*2+1)*2) = E3;
				}
			}
			break;
		}

		case 20:
		{
			Uint16 E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
			{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = *(Uint16*)(src + (MAX(0,looph-1)*srcPitch) + (2*loopw));
					D = *(Uint16*)(src + (looph*srcPitch) + (2*MAX(0,loopw-1)));
					E = *(Uint16*)(src + (looph*srcPitch) + (2*loopw));
					F = *(Uint16*)(src + (looph*srcPitch) + (2*MIN(srcWidth-1,loopw+1)));
					H = *(Uint16*)(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (2*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					*(Uint16*)(dst + looph*2*dstPitch + loopw*2*2) = E0;
					*(Uint16*)(dst + looph*2*dstPitch + (loopw*2+1)*2) = E1;
					*(Uint16*)(dst + (looph*2+1)*dstPitch + loopw*2*2) = E2;
					*(Uint16*)(dst + (looph*2+1)*dstPitch + (loopw*2+1)*2) = E3;
				}
			}
			break;
		}
		case 3:
		{
			int E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
			{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = READINT24(src + (MAX(0,looph-1)*srcPitch) + (3*loopw));
					D = READINT24(src + (looph*srcPitch) + (3*MAX(0,loopw-1)));
					E = READINT24(src + (looph*srcPitch) + (3*loopw));
					F = READINT24(src + (looph*srcPitch) + (3*MIN(srcWidth-1,loopw+1)));
					H = READINT24(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (3*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					WRITEINT24((dst + looph*2*dstPitch + loopw*2*3), E0);
					WRITEINT24((dst + looph*2*dstPitch + (loopw*2+1)*3), E1);
					WRITEINT24((dst + (looph*2+1)*dstPitch + loopw*2*3), E2);
					WRITEINT24((dst + (looph*2+1)*dstPitch + (loopw*2+1)*3), E3);
				}
			}
			break;
		}
		case 4:
		{
			Uint32 E0, E1, E2, E3, B, D, E, F, H;
			for(looph = 0; looph < srcHeight; ++looph)
				{
				for(loopw = 0; loopw < srcWidth; ++ loopw)
				{
					B = *(Uint32*)(src + (MAX(0,looph-1)*srcPitch) + (4*loopw));
					D = *(Uint32*)(src + (looph*srcPitch) + (4*MAX(0,loopw-1)));
					E = *(Uint32*)(src + (looph*srcPitch) + (4*loopw));
					F = *(Uint32*)(src + (looph*srcPitch) + (4*MIN(srcWidth-1,loopw+1)));
					H = *(Uint32*)(src + (MIN(srcHeight-1,looph+1)*srcPitch) + (4*loopw));

					E0 = D == B && B != F && D != H ? D : E;
					E1 = B == F && B != D && F != H ? F : E;
					E2 = D == H && D != B && H != F ? D : E;
					E3 = H == F && D != H && B != F ? F : E;

					*(Uint32*)(dst + looph*2*dstPitch + loopw*2*4) = E0;
					*(Uint32*)(dst + looph*2*dstPitch + (loopw*2+1)*4) = E1;
					*(Uint32*)(dst + (looph*2+1)*dstPitch + loopw*2*4) = E2;
					*(Uint32*)(dst + (looph*2+1)*dstPitch + (loopw*2+1)*4) = E3;
				}
			}
			break;
		}
	}
}
