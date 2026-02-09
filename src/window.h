#pragma once



#include "core.h"



/**************************************************************************************************/
/*	Types  */

typedef struct window {
	void* handle;
} window;

enum
{
	WINDOW_MODE_DEFAULT,
	WINDOW_MODE_BORDERLESS,
	WINDOW_MODE_FULLSCREEN,
};



/**************************************************************************************************/
/*	Functions  */

window create_window(const char* name, sint width, sint height, sint mode);
void destroy_window(window* window);
sint update_windows();


