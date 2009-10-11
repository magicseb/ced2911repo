#ifndef _SHARED_H_
#define _SHARED_H_
#define USE_SOUND_SMS 0

#include <stdio.h>
#include <math.h>
#include <zlib.h>

#include "types.h"
#include "macros.h"
#include "m68k.h"
#include "z80.h"
#include "genesis.h"
#include "vdp.h"
#include "render.h"
#include "mem68k.h"
#include "memz80.h"
#include "membnk.h"
#include "memvdp.h"
#include "system.h"
#include "unzip.h"
#include "fileio.h"
#include "loadrom.h"
#include "io.h"
#include "sound.h"
#include "fm.h"
#if USE_SOUND_SMS
	#include <limits.h>
	#include "sn76489.h"
#else
	#include "sn76496.h"
#endif
#include "osd.h"

#endif /* _SHARED_H_ */

