#include "libxenon.h"
#include "menu.h"
#include "xe_png.h"


#define printf

int fd=0;

struct file_buffer_t
{
  char name[256];
  unsigned char *data;
  long length;
  long offset;
};

struct pngMem{
	unsigned char *png_end;
	unsigned char *data;
	int size;
	int offset;//pour le parcours
};

/* The replacement fread wrapper which we will pass to png_set_read_fn(). */
void png_x_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	printf("\t\t\tpng_x_read !!\n");

	if ( ! (length > 0) )
	{
		printf("\t\t\tlength !!\n");
		return NULL;
	}
	png_size_t lenread = (png_size_t)read( fd, (void*)data,(size_t)length);
	printf("\t\t\tlenread %d!!\n",lenread);
	if ( lenread < length )
	{
		printf("\t\t\tlenread !!\n");
		return NULL;
	}
}

int offset=0;
void cc_png_mem_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	printf("[png_mem_read]-->\n");
	png_size_t  lenread;

	/* The file 'handle', a pointer, is stored in png_ptr->io_ptr */
	if ( png_ptr->io_ptr == NULL )
	{
		printf("\t\t\tpng_ptr->io_ptr == NULL !!\n");
		return NULL;
	}
	else
	{
		printf("\t\t\tpng_ptr->io_ptr == OK !!\n");
	}

	unsigned char *PNGdata=(unsigned char*)png_ptr->io_ptr;

	printf("[png_mem_read] 0x%02x 0x%02x 0x%02x 0x%02x \n", PNGdata[0],PNGdata[1],PNGdata[2],PNGdata[3]);
	if ( ! (length > 0) )
	{
		printf("\t\t\t(length > 0) !!\n");
		return NULL;
	}


//void * memcpy ( void * destination, const void * source, size_t num );
	memcpy(data,	PNGdata+offset, length);
	offset += length;
	printf("[png_mem_read]-->Fin\n");

}

static void png_mem_read (png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct file_buffer_t *src = (struct file_buffer_t *)png_get_io_ptr (png_ptr);

	/* Copy data from image buffer */
	memcpy (data, src->data + src->offset, length);
	printf("[png_mem_read] 0x%02x 0x%02x 0x%02x 0x%02x \n", data[0],data[1],data[2],data[3]);
	/* Advance in the file */
	src->offset += length;
}

//Lits un fichier png en mémoire
struct XenosSurface *loadPNGFromMemory( unsigned char *PNGdata )
{

	unsigned char header[8];	// 8 is the maximum size that can be checked
	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	char *filename="from memory";

	offset=0;
	printf( "Loading %s...\n", filename );

	struct file_buffer_t *file;
	file = (struct file_buffer_t *)malloc (sizeof (struct  file_buffer_t));
	file->length=1024*1024*5;
	file->data = (unsigned char *)malloc (file->length);//5mo ...
	file->offset=0;
	printf( "Allocation de file reussie\n" );
	memcpy(file->data,PNGdata,file->length);
	printf( "Allocation de file memcpy reussie\n" );

#if 0
	/* open file and test for it being a png */
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		printf("[read_png_file] File %s could not be opened for reading\n", filename);
		return 0;
	}

	fread(header, 1, 8, fp);

	memcpy(header,PNGdata,8);//...
	if (png_sig_cmp(header, 0, 8))
	{
		printf("[read_png_file] File %s is not recognized as a PNG file\n", filename);
		return 0;
	}
	else
	{
		printf("[read_png_file] 0x%02x 0x%02x 0x%02x 0x%02x \n", PNGdata[0],PNGdata[1],PNGdata[2],PNGdata[3]);
	}


	/* Check for valid magic number */
	if (!png_check_sig (file->data, 8))
	{
		printf ("error: is not a valid PNG image!\n");
		return NULL;
	}
#endif
	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		printf("[read_png_file] png_create_read_struct failed\n");
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("[read_png_file] png_create_info_struct failed\n");
		return 0;
	}

/*	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during init_io");
		return 0;
	} */

	//png_init_io(png_ptr, fp);
	printf("[read_png_file] png_set_read_fn\n");
	png_set_read_fn(png_ptr,(png_voidp *)file, png_mem_read );//permet de lire à partir de pngfile buff
	printf("[read_png_file] png_set_sig_bytes\n");
	//png_set_sig_bytes(png_ptr, 8);//on avance de 8 ?
	printf("[read_png_file] png_read_info\n");
	png_read_info(png_ptr, info_ptr);

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;
	printf("[read_png_file] png_set_interlace_handling\n");
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
	{
		printf("no support :(\n");
		return 0;
	}

	if (color_type == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_BEFORE);

	png_set_swap_alpha(png_ptr);
	printf("[read_png_file] //On créer la surface\n");
//On créer la surface
	struct XenosSurface *surface = Xe_CreateTexture(xe, width, height, 1, XE_FMT_8888 | XE_FMT_ARGB, 0);

	uint8_t *data = Xe_Surface_LockRect(xe, surface, 0, 0, 0, 0, XE_LOCK_WRITE);

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = data + surface->pitch * y;

	png_read_image(png_ptr, row_pointers);

	///fclose(fp);

	printf( "done\n" );

	Xe_Surface_Unlock(xe, surface);

	free(row_pointers);

	return surface;
}

struct XenosSurface *loadPNG( const char *filename )
{

	unsigned char header[8];	// 8 is the maximum size that can be checked
	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	printf( "Loading %s...", filename );

	/* open file and test for it being a png */
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		printf("[read_png_file] File %s could not be opened for reading\n", filename);
		return 0;
	}

	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		printf("[read_png_file] File %s is not recognized as a PNG file\n", filename);
		return 0;
	}

	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		printf("[read_png_file] png_create_read_struct failed\n");
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("[read_png_file] png_create_info_struct failed\n");
		return 0;
	}

/*	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during init_io");
		return 0;
	} */

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
	{
		printf("no support :(\n");
		return 0;
	}

	if (color_type == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_BEFORE);

	png_set_swap_alpha(png_ptr);
	struct XenosSurface *surface = Xe_CreateTexture(xe, width, height, 1, XE_FMT_8888 | XE_FMT_ARGB, 0);

	//struct XenosSurface *surface = Xe_CreateTexture(xe, width, height, 1, XE_FMT_8888 | XE_FMT_ARGB, 0);
	uint8_t *data = Xe_Surface_LockRect(xe, surface, 0, 0, 0, 0, XE_LOCK_WRITE);

	/* read file */
/*	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during read_image");
//		Xe_DestroyTexture(surface);
		return 0;
	} */

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

	for (y=0; y<height; y++)
		row_pointers[y] = data + surface->pitch * y;

	png_read_image(png_ptr, row_pointers);

	fclose(fp);

	printf( "done\n" );

	Xe_Surface_Unlock(xe, surface);

	free(row_pointers);

	return surface;
}


