#pragma once



#include <stdio.h> /* fopen, fclose, fread, printf, ... */
#include "core.h"



/**************************************************************************************************/
/*	App  */

char* get_working_dir();
void set_working_dir(const char* path);



/**************************************************************************************************/
/*	Path  */

/*  Returns a new string that needs to be freed!  */
char* get_absolute_path(const char* path);
const char* get_filename(const char* path, sint* out_len);
const char* get_file_extension(const char* path);
sint path_exists(const char* path);
sint is_file(const char* path);
sint is_dir(const char* path);



/**************************************************************************************************/
/*	File  */

void* load_file(const char* path, size_t* out_size);
sint create_dir(const char* path);
sint remove_dir(const char* path);
char** list_dir(const char* path);


