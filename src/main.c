#include <Windows.h>
#include "core.h"
#include "containers.h"
#include "window.h"
#include "rendering.h"
#include "io.h"

int main(int argc, const char** argv)
{
	initialize_renderer();

	window window = create_window("Rid Worlds - WIP", 1280, 720, WINDOW_MODE_BORDERLESS);
	
	create_window_surface(&window);
	create_render_pass();
	create_graphics_pipelines();
	create_framebuffers();
	create_command_buffers();
	create_sync_objects();

	while (update_windows())
	{
		draw_frame();
	}

	destroy_sync_objects();
	destroy_framebuffers();
	destroy_render_pass();
	destroy_graphics_pipelines();
	destroy_window_surface();
	destroy_renderer();
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return main(0, NULL);
}


