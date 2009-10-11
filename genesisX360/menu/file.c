#include "libxenon.h"
#include "menu.h"

/**
Enleve les . et .. d'un path
**/
char *cleanDirname(char *path)
{
	printf("cleanDirname ->%s\n",path);
	int i;

	int pathLen=strlen(path);

	char *newPath=malloc(pathLen);

	int j=0;//compteur de la nouvelle variable
	//Sauvegarde les 2 derniers slashs
	int lastSlashPos=0;
	int curSlashPos=0;
	for(i=0;i<pathLen;i++)
	{

		if(path[i]=='/')
		{
			curSlashPos=lastSlashPos;
			lastSlashPos=i;
		}

		if(path[i-1]=='/')
		{
			if((path[i]=='.')&&(path[i+1]=='.'))//retour en arriere
			{
				j=(curSlashPos);
				i+=2;
			}
			if((path[i]=='.'))
			{
				i++;
				j=lastSlashPos;
			}
		}
		newPath[j]=path[i];
		j++;
	}
	newPath[j]='\0';//Fin
	return newPath;
}

//Mets à jour la valeur de FatLS (Dossier en cours)
dirInfo_s updateCurDir(const char *dirName)
{
	char *cleanedDirName=cleanDirname(dirName);
	//char cleanedDirName[256];
	//strcpy(cleanDirname,dirName);

	usb_do_poll();
	dirInfo_s currentDirInfo;

	strcpy(currentDirInfo.currentDirname,cleanedDirName);

	//init cdi
	currentDirInfo.count=0;

	struct dirent *lecture;
	DIR *rep;
	//rep= opendir("uda:" );
	rep= opendir(cleanedDirName );

	while ((lecture = readdir(rep))) {
		//Accepte que certaine extention
		if(lecture->d_type==0x01)
		{
			//printf("0x%02x--%s\n",lecture->d_type, lecture->d_name);
			strcpy(currentDirInfo.node[currentDirInfo.count].name,lecture->d_name);
			currentDirInfo.node[currentDirInfo.count].type=lecture->d_type;
			currentDirInfo.count++;
		}
		else
		{
			//if((strstr(lecture->d_name,".ZIP"))||(strstr(lecture->d_name,".BIN"))||(strstr(lecture->d_name,".GG")))
			{
				//printf("0x%02x--%s\n",lecture->d_type, lecture->d_name);
				strcpy(currentDirInfo.node[currentDirInfo.count].name,lecture->d_name);
				currentDirInfo.node[currentDirInfo.count].type=lecture->d_type;
				currentDirInfo.count++;
			}
		}
	}
	closedir(rep);

	maxItem=currentDirInfo.count;
	return currentDirInfo;
}
