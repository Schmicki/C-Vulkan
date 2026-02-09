#pragma once



#include "core.h"
#include "window.h"
#include "vkrender.h"



/**************************************************************************************************/
/*	Types  */





/**************************************************************************************************/
/*	Functions  */

void initialize_renderer();
void destroy_renderer();
void create_window_surface(window* window);
void destroy_window_surface();
void create_render_pass();
void destroy_render_pass();
void create_graphics_pipelines();
void destroy_graphics_pipelines();
void create_framebuffers();
void destroy_framebuffers();
void create_command_buffers();
void create_sync_objects();
void destroy_sync_objects();
void draw_frame();


