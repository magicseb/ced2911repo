#include "shared.h"

static gzFile zfile;
extern char sramFilename[256];

/* Battery-backed RAM saving */
int sram_read_open()
{
	// Open the file
	zfile = gzopen(sramFilename, "rb");

	// Return
	if(zfile)
		return 1;
	else
		return 0;
}

void sram_read_close()
{
	gzclose(zfile);
}

uint8 sram_read_uint8()
{
	uint8 value;
	gzread(zfile, &value, sizeof(value));
	return value;
}

uint16 sram_read_uint16()
{
	uint16 value;
	gzread(zfile, &value, sizeof(value));
	return value;
}

uint32 sram_read_uint32()
{
	uint32 value;
	gzread(zfile, &value, sizeof(value));
	return value;
}

int sram_write_open()
{
	// Open the file
	zfile = gzopen(sramFilename, "wb9");

	// Return
	if(zfile)
		return 1;
	else
		return 0;
}

void sram_write_close()
{
	gzclose(zfile);
}

void sram_write_uint8(uint8 value)
{
	gzwrite(zfile, &value, sizeof(value));
}

void sram_write_uint16(uint16 value)
{
	gzwrite(zfile, &value, sizeof(value));
}

void sram_write_uint32(uint32 value)
{
	gzwrite(zfile, &value, sizeof(value));
}
