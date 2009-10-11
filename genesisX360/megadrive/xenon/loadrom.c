#include <diskio/diskio.h>
#include <fat/fat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "shared.h"
#include "drawtexture.h"

static uint8 block[0x4000];
/**
 * Information http://www.raphnet.net/electronique/genesis_cart/smdform.txt
void smd2bin(char *inblk, char *outblk)
{
	int n;
	for (n=0; n<0x2000; n++)
	{
		outblk[n*2] = inblk[0x2000+n];
		outblk[n*2+1] = inblk[n];
	}
}
 */
void deinterleave_block(uint8 *src)
{
    int i;
    memcpy(block, src, 0x4000);
    for(i = 0; i < 0x2000; i += 1)
    {
        src[i*2+0] = block[0x2000 + i];
        src[i*2+1] = block[0x0000 + i];
    }
}

char *memcat(char *dst, register char *src, register int len)
{
	register char *d;

	for (d = dst; --len >= 0; *d++ = *src++) ;
	//for (d = dst; *dst++; ) ;
	for (--dst; *dst++ = *src++; ) {printf(" 0x%02x",*src);}
	return dst;
}

int load_rom(char *filename)
{
	if(check_zip(filename)==0)
	{
		printf("\t\t\tUtilise la méthode normale\n");
		return load_rom_file(filename);
	}
	else
	{

		return load_zip_file(filename);
	}
}
/**
 * Information: http://www.zophar.net/fileuploads/2/10614uauyw/Genesis_ROM_Format.txt
 */
int load_zip_file(char *filename)
{
	uint8 *fileptr=malloc(1024*1024*16);//taille rom max = 16Mo
	int romsize,offset=0;
	uint8 header[0x200];
	printf("\t\t\tC'est un zip\n");
	//copie la rom dans un buffer
	romsize=UnZipBuffer(fileptr,0,filename);

	printf("\t\t\tC'est un zip de taille %d -- 0x%02x 0x%02x 0x%02x\n",romsize,fileptr[0],fileptr[1],fileptr[2]);


	if(!fileptr)
	{
		return (0);
	}

	memcpy(cart_rom, fileptr + offset, romsize);

	//Pour les fichier .smd
	if (strncmp((char *)(cart_rom + 0x100),"SEGA", 4))//Passe pas la
	{
		int i;
		//1) Skip the 512 bytes header
		romsize -= 512;
		offset = 512;

		memcpy(header, fileptr, offset);
		//2) Get 16KB from the ROM (16384 bytes)
		for(i = 0; i < (romsize / 0x4000); i += 1)
		{
			//3) Decode the block
			//4) Write the decoded block to the BIN file
			deinterleave_block(fileptr + offset + (i * 0x4000));
		}
	}

	memset(cart_rom, 0, 0x400000);
	if(romsize > 0x400000) romsize = 0x400000;

	memcpy(cart_rom, fileptr + offset, romsize);

	/* Free allocated file data */

	free(fileptr);
	return (1);
}

#define READBLOCKSIZE 1024*64 //64ko
int load_rom_file(char *filename)
{
	int size, offset = 0;
	uint8 header[0x200];

	printf("\t\t Malloc\n");
	uint8 *ptr=malloc(1024*1024*16);//taille rom max = 16Mo
	uint8 *ptr_file=malloc(1024*1024*16);//taille rom max = 16Mo
//	int size;
	int fd;


	printf("\t\tLoadrom\nOuverture de %s\n",filename);

	fd = open(filename, O_RDONLY);

	if(!fd) return (NULL);


	struct stat stat;

	fstat(fd, &stat);

	size=stat.st_size;

	printf("\t\t %s avec une taille de %d\n",filename,size);

	//On va donné le % de chargement ...
	//read(fd, ptr, size);
	int tmpbr=0;

	int readblocksize=READBLOCKSIZE;
	int byteread=5;
	while(size)
	{
		if(readblocksize>size)
		{
			readblocksize=size;
		}
		byteread=read(fd, ptr_file, readblocksize);

		memcpy(ptr+tmpbr,ptr_file,readblocksize);
		//printf( "Loading %s (%d%%)\n", filename, (stat.st_size - size) * 100 / stat.st_size);
		//AFFiche le chargement
		char loadingTxt[256];
		sprintf(loadingTxt,"Loading %s (%d%%)\n", filename, (stat.st_size - size) * 100 / stat.st_size);
		printf("Bloquer (debug at loadrom.c)\n");
		fastPrintf(200,240,0x00000000,loadingTxt);

		tmpbr+=byteread;
		size-=byteread;
	}
	size=stat.st_size;

	if(!ptr)
	{
		return (0);
	}
	if((size / 512) & 1)
	{
		int i;

		size -= 512;
		offset += 512;

		memcpy(header, ptr, 512);

		for(i = 0; i < (size / 0x4000); i += 1)
		{
			deinterleave_block(ptr + offset + (i * 0x4000));
		}
	}
	memset(cart_rom, 0, 0x400000);
	if(size > 0x400000) size = 0x400000;

	memcpy(cart_rom, ptr + offset, size);

	/* Free allocated file data */

	free(ptr);

	return (1);
}

#define PKZIPID 0x504b0304
#define MAXROM 0x500000
#define ZIPCHUNK 2048

typedef struct
{
  unsigned int zipid __attribute__ ((__packed__));  // 0x04034b50
  unsigned short zipversion __attribute__ ((__packed__));
  unsigned short zipflags __attribute__ ((__packed__));
  unsigned short compressionMethod __attribute__ ((__packed__));
  unsigned short lastmodtime __attribute__ ((__packed__));
  unsigned short lastmoddate __attribute__ ((__packed__));
  unsigned int crc32 __attribute__ ((__packed__));
  unsigned int compressedSize __attribute__ ((__packed__));
  unsigned int uncompressedSize __attribute__ ((__packed__));
  unsigned short filenameLength __attribute__ ((__packed__));
  unsigned short extraDataLength __attribute__ ((__packed__));
} PKZIPHEADER;


/*
 * Zip files are stored little endian
 * Support functions for short and int types
 */
static inline unsigned int FLIP32 (unsigned int  b)
{
  unsigned int c;
  c = (b & 0xff000000) >> 24;
  c |= (b & 0xff0000) >> 8;
  c |= (b & 0xff00) << 8;
  c |= (b & 0xff) << 24;
  return c;
}

static inline uint16  FLIP16 (uint16  b)
{
  uint16 c;
  c = (b & 0xff00) >> 8;
  c |= (b & 0xff) << 8;
  return c;
}


int UnZipBuffer (unsigned char *outbuffer, long discoffset, char *filename)
{
	PKZIPHEADER pkzip;
	int zipoffset = 0;
	int zipchunk = 0;
	char out[ZIPCHUNK];
	z_stream zs;
	int res;
	int bufferoffset = 0;
	int have = 0;
	char readbuffer[2048];
	char msg[128];
	FILE *fatfile = NULL;
	int size=0;

	/*** FAT file support ***/
	if (filename)
	{
		fatfile = fopen(filename, "rb");
		if (fatfile == NULL) return 0;
	}




	/*** Read Zip Header ***/
	if (fatfile)
	{
		fseek(fatfile, 0, SEEK_SET);
		fread(readbuffer, 1, 2048, fatfile);
	}
	else
	{
		printf("\t\t\tUnable to unzip file nt found\n");
		return 0;
	}

	/*** Copy PKZip header to local, used as info ***/
	memcpy (&pkzip, &readbuffer, sizeof (PKZIPHEADER));

	sprintf (msg, "\t\t\t\tUnzipping %d bytes ...\n", FLIP32 (pkzip.uncompressedSize));
	printf(msg);

	/*** Prepare the zip stream ***/
	memset (&zs, 0, sizeof (z_stream));
	zs.zalloc = Z_NULL;
	zs.zfree = Z_NULL;
	zs.opaque = Z_NULL;
	zs.avail_in = 0;
	zs.next_in = Z_NULL;
	res = inflateInit2 (&zs, -MAX_WBITS);

	if (res != Z_OK)
	{
		printf("\t\t\tUnable to unzip file\n");
		return 0;
	}

	/*** Set ZipChunk for first pass ***/
	zipoffset = (sizeof (PKZIPHEADER) + FLIP16 (pkzip.filenameLength) + FLIP16 (pkzip.extraDataLength));
	zipchunk = ZIPCHUNK - zipoffset;

	/*** Now do it! ***/
	do
	{
		zs.avail_in = zipchunk;
		zs.next_in = (Bytef *) & readbuffer[zipoffset];

		/*** Now inflate until input buffer is exhausted ***/
		do
		{
			zs.avail_out = ZIPCHUNK;
			zs.next_out = (Bytef *) & out;
			res = inflate (&zs, Z_NO_FLUSH);

			if (res == Z_MEM_ERROR)
			{
				inflateEnd (&zs);
				printf("\t\t\tUnable to unzip file\n");
				return 0;
			}

			have = ZIPCHUNK - zs.avail_out;
			if (have)
			{
				/*** Copy to normal block buffer ***/
				memcpy (&outbuffer[bufferoffset], &out, have);
				bufferoffset += have;
			}
		}
		while (zs.avail_out == 0);

		/*** Readup the next 2k block ***/
		zipoffset = 0;
		zipchunk = ZIPCHUNK;

		if (fatfile)
		{
			fread(readbuffer, 1, 2048, fatfile);
		}

		//avance le chargement ...
		char loadingTxt[256];
//sprintf(loadingTxt,"Loading %s (%d%%)\n", filename, (pkzip.uncompressedSize - size) * 100 /pkzip.uncompressedSize);
		sprintf(loadingTxt,"Loading %s (%d%%)\n",filename,(pkzip.compressedSize - size) * 100 /pkzip.compressedSize);
		fastPrintf(200,240,0x00000000,loadingTxt);
		size+=2048;
	}
	while (res != Z_STREAM_END);

	inflateEnd (&zs);


	/* close file */
	if (fatfile) fclose(fatfile);

	if (res == Z_STREAM_END)
	{
		if (FLIP32 (pkzip.uncompressedSize) == (int) bufferoffset) return bufferoffset;
		else return FLIP32 (pkzip.uncompressedSize);
	}
	return 0;
}



