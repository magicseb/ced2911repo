
static int   LUT16to32[65536];
static int   RGBtoYUV[65536];
static int   YUV1, YUV2;
const  int   Ymask = 0x00FF0000;
const  int   Umask = 0x0000FF00;
const  int   Vmask = 0x000000FF;
const  int   trY   = 0x00300000;
const  int   trU   = 0x00000700;
const  int   trV   = 0x00000006;

inline void Interp1(unsigned char * pc, int c1, int c2)
{
  *((int*)pc) = (c1*3+c2) >> 2;
}

inline void Interp2(unsigned char * pc, int c1, int c2, int c3)
{
  *((int*)pc) = (c1*2+c2+c3) >> 2;
}

inline void Interp5(unsigned char * pc, int c1, int c2)
{
  *((int*)pc) = (c1+c2) >> 1;
}

inline void Interp6(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*5+c2*2+c3)/8;

  *((int*)pc) = ((((c1 & 0x00FF00)*5 + (c2 & 0x00FF00)*2 + (c3 & 0x00FF00) ) & 0x0007F800) +
                 (((c1 & 0xFF00FF)*5 + (c2 & 0xFF00FF)*2 + (c3 & 0xFF00FF) ) & 0x07F807F8)) >> 3;
}

inline void Interp7(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*6+c2+c3)/8;

  *((int*)pc) = ((((c1 & 0x00FF00)*6 + (c2 & 0x00FF00) + (c3 & 0x00FF00) ) & 0x0007F800) +
                 (((c1 & 0xFF00FF)*6 + (c2 & 0xFF00FF) + (c3 & 0xFF00FF) ) & 0x07F807F8)) >> 3;
}

inline void Interp9(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*2+(c2+c3)*3)/8;

  *((int*)pc) = ((((c1 & 0x00FF00)*2 + ((c2 & 0x00FF00) + (c3 & 0x00FF00))*3 ) & 0x0007F800) +
                 (((c1 & 0xFF00FF)*2 + ((c2 & 0xFF00FF) + (c3 & 0xFF00FF))*3 ) & 0x07F807F8)) >> 3;
}

inline void Interp10(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*14+c2+c3)/16;

  *((int*)pc) = ((((c1 & 0x00FF00)*14 + (c2 & 0x00FF00) + (c3 & 0x00FF00) ) & 0x000FF000) +
                 (((c1 & 0xFF00FF)*14 + (c2 & 0xFF00FF) + (c3 & 0xFF00FF) ) & 0x0FF00FF0)) >> 4;
}


#define PIXEL00_0     *((int*)(pOut)) = c[5];
#define PIXEL00_10    Interp1(pOut, c[5], c[1]);
#define PIXEL00_11    Interp1(pOut, c[5], c[4]);
#define PIXEL00_12    Interp1(pOut, c[5], c[2]);
#define PIXEL00_20    Interp2(pOut, c[5], c[4], c[2]);
#define PIXEL00_21    Interp2(pOut, c[5], c[1], c[2]);
#define PIXEL00_22    Interp2(pOut, c[5], c[1], c[4]);
#define PIXEL00_60    Interp6(pOut, c[5], c[2], c[4]);
#define PIXEL00_61    Interp6(pOut, c[5], c[4], c[2]);
#define PIXEL00_70    Interp7(pOut, c[5], c[4], c[2]);
#define PIXEL00_90    Interp9(pOut, c[5], c[4], c[2]);
#define PIXEL00_100   Interp10(pOut, c[5], c[4], c[2]);
#define PIXEL01_0     *((int*)(pOut+4)) = c[5];
#define PIXEL01_10    Interp1(pOut+4, c[5], c[3]);
#define PIXEL01_11    Interp1(pOut+4, c[5], c[2]);
#define PIXEL01_12    Interp1(pOut+4, c[5], c[6]);
#define PIXEL01_20    Interp2(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_21    Interp2(pOut+4, c[5], c[3], c[6]);
#define PIXEL01_22    Interp2(pOut+4, c[5], c[3], c[2]);
#define PIXEL01_60    Interp6(pOut+4, c[5], c[6], c[2]);
#define PIXEL01_61    Interp6(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_70    Interp7(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_90    Interp9(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_100   Interp10(pOut+4, c[5], c[2], c[6]);
#define PIXEL10_0     *((int*)(pOut+BpL)) = c[5];
#define PIXEL10_10    Interp1(pOut+BpL, c[5], c[7]);
#define PIXEL10_11    Interp1(pOut+BpL, c[5], c[8]);
#define PIXEL10_12    Interp1(pOut+BpL, c[5], c[4]);
#define PIXEL10_20    Interp2(pOut+BpL, c[5], c[8], c[4]);
#define PIXEL10_21    Interp2(pOut+BpL, c[5], c[7], c[4]);
#define PIXEL10_22    Interp2(pOut+BpL, c[5], c[7], c[8]);
#define PIXEL10_60    Interp6(pOut+BpL, c[5], c[4], c[8]);
#define PIXEL10_61    Interp6(pOut+BpL, c[5], c[8], c[4]);
#define PIXEL10_70    Interp7(pOut+BpL, c[5], c[8], c[4]);
#define PIXEL10_90    Interp9(pOut+BpL, c[5], c[8], c[4]);
#define PIXEL10_100   Interp10(pOut+BpL, c[5], c[8], c[4]);
#define PIXEL11_0     *((int*)(pOut+BpL+4)) = c[5];
#define PIXEL11_10    Interp1(pOut+BpL+4, c[5], c[9]);
#define PIXEL11_11    Interp1(pOut+BpL+4, c[5], c[6]);
#define PIXEL11_12    Interp1(pOut+BpL+4, c[5], c[8]);
#define PIXEL11_20    Interp2(pOut+BpL+4, c[5], c[6], c[8]);
#define PIXEL11_21    Interp2(pOut+BpL+4, c[5], c[9], c[8]);
#define PIXEL11_22    Interp2(pOut+BpL+4, c[5], c[9], c[6]);
#define PIXEL11_60    Interp6(pOut+BpL+4, c[5], c[8], c[6]);
#define PIXEL11_61    Interp6(pOut+BpL+4, c[5], c[6], c[8]);
#define PIXEL11_70    Interp7(pOut+BpL+4, c[5], c[6], c[8]);
#define PIXEL11_90    Interp9(pOut+BpL+4, c[5], c[6], c[8]);
#define PIXEL11_100   Interp10(pOut+BpL+4, c[5], c[6], c[8]);


