#ifndef __config_h
#define __config_h

/* Input bitmasks */
#define INPUT_MODE      (0x00000800)
#define INPUT_Z         (0x00000400)
#define INPUT_Y         (0x00000200)
#define INPUT_X         (0x00000100)
#define INPUT_START     (0x00000080)
#define INPUT_C         (0x00000040)
#define INPUT_B         (0x00000020)
#define INPUT_A         (0x00000010)
#define INPUT_LEFT      (0x00000008)
#define INPUT_RIGHT     (0x00000004)
#define INPUT_DOWN      (0x00000002)
#define INPUT_UP        (0x00000001)


#define PAD_UP 0x01
#define PAD_DOWN 0x02
#define PAD_LEFT 0x03
#define PAD_RIGHT 0x04
#define PAD_A 0x05
#define	PAD_B 0x06
#define PAD_X 0x07
#define PAD_Y 0x08
#define	PAD_RB 0x09
#define	PAD_LB 0x0A
#define PAD_RT 0x0B
#define PAD_LT 0x0C
#define	PAD_START 0x0D
#define PAD_SELECT 0x0E
#define PAD_LOGO 0x0F
#define	PAD_LSTICK 0x10
#define PAD_RSTICK 0x11
#define	PAD_LSTICK_XP 0x12
#define	PAD_LSTICK_YP 0x13
#define	PAD_LSTICK_XN 0x14
#define	PAD_LSTICK_YN 0x15
#define	PAD_RSTICK_XP 0x16
#define	PAD_RSTICK_YP 0x17
#define	PAD_RSTICK_XN 0x18
#define	PAD_RSTICK_YN 0x19

struct config_s
{
	//Touches de la mégadrive
	int up,down,left,right,a,b,c,x,y,z,start,mode;

	//Touches pour la configuration (ecran, retour au menu etc ...)
	int doresize,rup,rdown,rleft,rright;
	int back;

	//Couleur du fonds
	int bgcolor;
	int background;
	
	//Reglage de l'ecran
	float scaleX,scaleY;
};
struct config_s globalconfig;


/**

Keycode des inputs

**/

struct c_input_s
{
	int val[0x30];
}c_input;

struct keycode_s
{
	char name[256];
	int code;
};




void default_configuration();
struct c_input_s controllerFromConfig(struct controller_data_s xinput);

#endif

