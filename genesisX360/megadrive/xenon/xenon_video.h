void videoInit(void);
void updateVideo(int,int);

struct XenosSurface *gfxplane;
struct XenosDevice _xe, *xe;
struct XenosSurface *fb;
struct XenosShader *sh_ps, *sh_vs;
struct XenosVertexBuffer *vb;
struct XenosIndexBuffer *ib;
float cubAnglex,cubAngley,cubAnglez;
float cubPosx,cubPosy,cubPosz;

extern void do_edram_foo(struct XenosDevice *xe, int complete);

#include "igm.h" //pour struct XenosShader *igm_ps,*igm_vs;


