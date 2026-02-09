#pragma once



#include "core.h"



/**************************************************************************************************/
/*	Types  */

#define MAX_PARALLEL_FRAMES 3

typedef struct window_config {
	uint vsync; /* 0 = NONE, 1 = HALF, 2 = FULL, 3 = FAST */
} window_config;

typedef struct render_config {
	uint parallel_frames;
} render_config;

extern window_config g_wndcfg;
extern render_config g_rndcfg;
