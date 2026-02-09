#include <Windows.h>
#include "io.h"



/**************************************************************************************************/
/*	App  */

char* get_working_dir()
{
    DWORD size = GetCurrentDirectoryA(0, NULL);
    char* path = malloc(size);
    
    GetCurrentDirectoryA(size, path);
    return path;
}

void set_working_dir(const char* path)
{
    SetCurrentDirectoryA(path);
}



/**************************************************************************************************/
/*	Path  */

char* get_absolute_path(const char* path)
{
    char* abs;
    DWORD size = GetFullPathNameA(path, 0, NULL, NULL);

    if (size == 0)
        return NULL;

    abs = malloc(size);
    GetFullPathNameA(path, size, abs, NULL);
    return abs;
}

const char* get_filename(const char* path, sint* out_len)
{
    return NULL;
}

const char* get_file_extension(const char* path)
{
    return NULL;
}

sint path_exists(const char* path)
{
    return -1;
}

sint is_file(const char* path)
{
    return -1;
}

sint is_dir(const char* path)
{
    return -1;
}



/**************************************************************************************************/
/*	File  */

#if 0 /* Portable Load File */
void* load_file(const char* path, size_t* out_size)
{
    void* data = NULL;
    void* dst;
    uptr_t data_size = 0;
    uptr_t buffer_size = 0x10000 / 2;
    uptr_t bytes_free;
    FILE* fd = fopen(path, "rb");

    if (fd == NULL)
        return NULL;

    do {
        buffer_size *= 2;
        dst = realloc(data, buffer_size);

        if (dst == NULL)
            goto error;

        data = dst;
        dst = (char*)data + data_size;
        bytes_free = buffer_size - data_size;
        data_size += fread(dst, 1, bytes_free, fd);

    } while (data_size == buffer_size);

    dst = realloc(data, data_size);

    if (dst == NULL)
        goto error;


    if (out_size != NULL)
        *out_size = data_size;

    fclose(fd);
    data = dst;
    return data;

error:
    free(data);
    fclose(fd);
    return NULL;
}
#endif

void* load_file(const char* path, size_t* out_size)
{
    void* buffer;
    size_t file_size;
    size_t bytes_read;
    FILE* fd = fopen(path, "rb");

    if (fd == NULL)
        return NULL;

    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    buffer = malloc(file_size);

    if (buffer == NULL)
    {
        fclose(fd);
        return NULL;
    }

    bytes_read = fread(buffer, 1, file_size, fd);
    fclose(fd);

    if (bytes_read != file_size)
    {
        free(buffer);
        buffer = NULL;
    }

    if (out_size != NULL)
        *out_size = file_size;

    return buffer;
}

sint create_dir(const char* path)
{
    return -1;
}

sint remove_dir(const char* path)
{
    return -1;
}

char** list_dir(const char* path)
{
    return NULL;
}


