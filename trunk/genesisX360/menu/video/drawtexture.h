void fastPrintf(int x,int y, int color, char *str);

//Mode Xe :)
struct XenosSurface * XeGetBitmapFromMemory(int width,int height,void *pixeldata);
struct XenosSurface * XeGetFont();
void XeDrawSurface2(struct XenosSurface *txt, float dstx, float dsty, float width, float height, int withalpha);
void XeDrawSurface(struct XenosSurface *txt, float dstx, float dsty, float scale, int withalpha,int center);



