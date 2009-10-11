#ifndef VIDEO_H_
#define VIDEO_H_

void initScreen(int,int);
void updateScreen(int,int);

struct XenosSurface *gfxplane;
struct XenosDevice _xe, *xe;
struct XenosSurface *fb;
struct XenosShader *sh_ps, *sh_vs;
struct XenosVertexBuffer *vb;
struct XenosIndexBuffer *ib;

// type de vs et ps
extern unsigned char content_datapspsu[];
extern unsigned char content_datavsvsu[];
extern unsigned char shader_3d_ps[];
extern unsigned char shader_3d_vs[];
extern unsigned char shader_font_ps[];
extern unsigned char shader_font_vs[];
struct XenosShader *sh_3d_ps, *sh_3d_vs, *sh_font_ps, *sh_font_vs, *sh_text_ps, *sh_text_vs;

float xe_framecount;

int max_vertices ;
int nr_primitives ;
extern void do_edram_foo(struct XenosDevice *xe, int complete);

#endif
