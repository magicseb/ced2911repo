#include <stdio.h>
#include <xenos/xe.h>
#include <xenos/xenos.h>
#include <input/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "config.h"
#include "shared.h"
#include "config.h"

/**
Permet la lecture de la configuration a partir d'un fichier .ini
**/
/*

*/
struct keycode_s keycode[]={
	{"INPUT_MODE",	INPUT_MODE	},
	{"INPUT_Z",	INPUT_Z		},
	{"INPUT_Y",	INPUT_Y		},
	{"INPUT_X",	INPUT_X		},
	{"INPUT_START",	INPUT_START	},
	{"INPUT_C",	INPUT_C		},
	{"INPUT_B",	INPUT_B		},
	{"INPUT_A",	INPUT_A		},
	{"INPUT_LEFT",	INPUT_LEFT	},
	{"INPUT_RIGHT",	INPUT_RIGHT	},
	{"INPUT_DOWN",	INPUT_DOWN	},
	{"INPUT_UP",	INPUT_UP	},
};

struct keycode_s padcode[]={
	{"PAD_UP",		PAD_UP		},
	{"PAD_DOWN",		PAD_DOWN	},
	{"PAD_LEFT",		PAD_LEFT	},
	{"PAD_RIGHT",		PAD_RIGHT	},
	{"PAD_A",		PAD_A		},
	{"PAD_B",		PAD_B		},
	{"PAD_C",		PAD_B		},
	{"PAD_X",		PAD_X		},
	{"PAD_Y",		PAD_Y		},
	{"PAD_Z",		PAD_Y		},
	{"PAD_RB",		PAD_RB		},
	{"PAD_LB",		PAD_LB		},
	{"PAD_RT",		PAD_RT		},
	{"PAD_LT",		PAD_LT		},
	{"PAD_START",		PAD_START	},
	{"PAD_SELECT",		PAD_SELECT	},
	{"PAD_LOGO",		PAD_LOGO	},
	{"PAD_LSTICK",		PAD_LSTICK	},
	{"PAD_RSTICK",		PAD_RSTICK	},
	{"PAD_LSTICK_XP",	PAD_LSTICK_XP	},
	{"PAD_LSTICK_YP",	PAD_LSTICK_YP	},
	{"PAD_LSTICK_XN",	PAD_LSTICK_XN	},
	{"PAD_LSTICK_YN",	PAD_LSTICK_YN	},
	{"PAD_RSTICK_XP",	PAD_RSTICK_XP	},
	{"PAD_RSTICK_YP",	PAD_RSTICK_YP	},
	{"PAD_RSTICK_XN",	PAD_RSTICK_XN	},
	{"PAD_RSTICK_YN",	PAD_RSTICK_YN	},
};

/*
#define	PAD_LSTICK_XP 0x12
#define	PAD_LSTICK_YP 0x13
#define	PAD_LSTICK_XN 0x14
#define	PAD_LSTICK_YN 0x15
#define	PAD_RSTICK_XP 0x16
#define	PAD_RSTICK_YP 0x17
#define	PAD_RSTICK_XN 0x18
#define	PAD_RSTICK_YN 0x19
*/

/**
retourne un tableau correspondant au touche appuyer
**/
#define PUSHEDVALUE 27000
#define GETVALP(x)	(x>PUSHEDVALUE)?1:0	
#define GETVALN(x)	(x<-PUSHEDVALUE)?1:0
struct c_input_s controllerFromConfig(struct controller_data_s xinput)
{
	struct c_input_s ret;

	ret.val[PAD_UP]		=xinput.up;
	ret.val[PAD_DOWN]	=xinput.down;
	ret.val[PAD_LEFT]	=xinput.left;
	ret.val[PAD_RIGHT]	=xinput.right;
	ret.val[PAD_A]		=xinput.a;
	ret.val[PAD_B]		=xinput.b;
	ret.val[PAD_X]		=xinput.x;
	ret.val[PAD_Y]		=xinput.y;
	ret.val[PAD_RB]		=xinput.rb;
	ret.val[PAD_LB]		=xinput.lb;
	ret.val[PAD_RT]		=xinput.rt;
	ret.val[PAD_LT]		=xinput.lt;
	ret.val[PAD_START]	=xinput.start;
	ret.val[PAD_SELECT]	=xinput.select;
	ret.val[PAD_LOGO]	=xinput.logo;
	ret.val[PAD_LSTICK]	=xinput.s1_z;
	ret.val[PAD_RSTICK]	=xinput.s2_z;
	ret.val[PAD_LSTICK_XP]	=GETVALP(xinput.s1_x);
	ret.val[PAD_LSTICK_XN]	=GETVALN(xinput.s1_x);
	ret.val[PAD_LSTICK_YP]	=GETVALP(xinput.s1_y);
	ret.val[PAD_LSTICK_YN]	=GETVALN(xinput.s1_y);
	ret.val[PAD_RSTICK_XP]	=GETVALP(xinput.s2_x);
	ret.val[PAD_RSTICK_XN]	=GETVALN(xinput.s2_x);
	ret.val[PAD_RSTICK_YP]	=GETVALP(xinput.s2_y);
	ret.val[PAD_RSTICK_YN]	=GETVALN(xinput.s2_y);

	return ret;
}

//cherche le code d'une touche
int charKeyToVal(char *keyname,struct keycode_s *keycode_t)
{
	int i;
	for(i=0;i<0x24;i++)
	{
		if(strcmp(keyname,keycode_t[i].name)==0)
		{
			printf("\t\tTROUVER\n");
			return keycode_t[i].code;
		}
	}
	printf("\t\tERREUR %s\n",keyname);
	return 0;//Aucun code trouver
}

//configuration par defaut
void default_configuration()
{
//Emulation
	globalconfig.up=charKeyToVal("PAD_UP",padcode);
	globalconfig.down=charKeyToVal("PAD_DOWN",padcode);
	globalconfig.left=charKeyToVal("PAD_LEFT",padcode);
	globalconfig.right=charKeyToVal("PAD_RIGHT",padcode);
	globalconfig.a=charKeyToVal("PAD_A",padcode);
	globalconfig.b=charKeyToVal("PAD_B",padcode);
	globalconfig.c=charKeyToVal("PAD_RB",padcode);
	globalconfig.x=charKeyToVal("PAD_X",padcode);
	globalconfig.y=charKeyToVal("PAD_Y",padcode);
	globalconfig.z=charKeyToVal("PAD_LB",padcode);
	globalconfig.mode=charKeyToVal("PAD_SELECT",padcode);
	globalconfig.start=charKeyToVal("PAD_START",padcode);
//Configuration
	globalconfig.back=charKeyToVal("PAD_LOGO",padcode);
	globalconfig.doresize=charKeyToVal("PAD_RSTICK",padcode);
	globalconfig.rup=charKeyToVal("PAD_RSTICK_YP",padcode);
	globalconfig.rdown=charKeyToVal("PAD_RSTICK_YN",padcode);
	globalconfig.rleft=charKeyToVal("PAD_RSTICK_XP",padcode);
	globalconfig.rright=charKeyToVal("PAD_RSTICK_XN",padcode);
}


/*
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
*/

void parseLine(const char *opt1, const char *opt2)
{
	if(strlen(opt2)<1)
	{
		return 0;
	}
	printf("'%s' => '%s'\n",opt1,opt2);
	if(!strcmp(opt1,"INPUT_MODE"))
	{
		globalconfig.mode=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_START"))
	{
		globalconfig.start=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_UP"))
	{
		globalconfig.up=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_DOWN"))
	{
		globalconfig.down=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_RIGHT"))
	{
		globalconfig.right=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_LEFT"))
	{
		globalconfig.left=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_A"))
	{
		globalconfig.a=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_B"))
	{
		globalconfig.b=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_C"))
	{
		globalconfig.c=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_X"))
	{
		globalconfig.x=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_Y"))
	{
		globalconfig.y=charKeyToVal(opt2,padcode);
	}
	if(!strcmp(opt1,"INPUT_Z"))
	{
		globalconfig.z=charKeyToVal(opt2,padcode);
	}
}

void parseBuf(uint8 *readbuffer)
{
	//on parcour le buffer a la recherche de ligne
	int offset=0,lastoffset=0,i=0;
	uint8 *parsebuffer=malloc(1024*1024*1);
	memcpy(parsebuffer,readbuffer,1024*1024*1);
	

	while(*parsebuffer)
	{
		//printf("0x%02x",*buffer);
		if(*parsebuffer=='\n')
		{
			lastoffset=offset;
			offset=i;
			printf("ligne !!\n");	
			printf("offset=%x\-lastoffset=%x\n", offset,lastoffset);	
	
			char line[256];
			int linesize=offset-lastoffset;

			if(linesize>1)
			{
				strncpy(line,(char*)readbuffer+lastoffset,linesize);

				line[linesize]='\0';

				char opt1[256]="\0";
				char opt2[256]="\0";
				sscanf (line, "%s = %s", &opt1, &opt2);
				printf("%s -> %s\t|\t ligne = %d\n", opt1,opt2,offset-lastoffset);
				parseLine(opt1,opt2);
			}
		}
		*parsebuffer++;
		i++;
		
	}
}

void parseConfigFile(char *filename)
{
	printf("\t\t\t Configuration => Ouverture de %s\n",filename);

	int pFile;
	int size;
	uint8 *ptr=malloc(1024*1024*1);//1Mo

	pFile = open (filename,O_RDONLY);
	if (pFile!=NULL)
	{

		struct stat stat;
		fstat(pFile, &stat);
		size=stat.st_size;

		read(pFile, ptr, size);
		parseBuf(ptr);
/*
		while(!(feof(pFile)))
		{
			char opt1[256]="\0";
			char opt2[256]="\0";
			fscanf (pFile, "%s = %s", &opt1, &opt2);
			parseLine(opt1, opt2);
		}	
*/
		
	}
	close (pFile);
}









