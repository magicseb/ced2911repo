#ifndef MAIN_H
#define MAIN_H

#define nItemOnScreen 13 //Nombre d'item à l'ecran
#define DECALAGEROWS	0.4//Decalage (par rapport au haut) | xoffset
#define ROWHEIGHT	-0.1//Taille d'une row

typedef struct
{
	char name[512];
	unsigned short type;
	unsigned long filesize;
	unsigned int count;
}nodeInfo_s;

typedef struct
{
	int count;
	char currentDirname[512];//Nom du repertoire courrant
	//nodeInfo_s node[2048];//nombre de fichier max par dossier
	nodeInfo_s node[8192];
}dirInfo_s;

//extern dirInfo_s dirInfo;//Contient tous les repertoires

int iSelectedItem;//Item Selectionner
unsigned int maxItem;//Nombre d'item maximum
unsigned short emuLoaded;//pas d'emu lancer encore

char *cleanDirname(char *path);
dirInfo_s updateCurDir(const char *dirName);
float bound(int f);
#define BOUND_PI 3.14

#endif
