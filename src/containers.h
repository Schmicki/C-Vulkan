#pragma once



#include "core.h"



/**************************************************************************************************/

typedef sint(*hashfunc)(void*);
typedef sint(*cmpfunc)(void*, void*);



/**************************************************************************************************/

/* 16 - 24 bytes. */
typedef struct vector
{
	void* data;
	sint elem_size;
	sint len;
	sint cap;
} vector;



/*************************************************************************************************/

/* 12 - 16 bytes. */
typedef struct string
{
	char* data;
	sint len;
	sint cap;
} string;



/*************************************************************************************************/

/* 20 - 32 bytes. */
typedef struct flat_map
{
	void* buckets;
	sint bucket_size;
	sint len;
	sint cap;
	cmpfunc cmp;
} flat_map;



/*************************************************************************************************/

/* 28 - 48 bytes. */
typedef struct flat_hashmap
{
	void* buckets;
	u8* info;
	sint bucket_size;
	sint len;
	sint cap;
	hashfunc hash;
	cmpfunc cmp;
} flat_hashmap;



/*************************************************************************************************/

/* 32 - 56 bytes. */
typedef struct flat_ordered_hashmap
{
	void* dense;
	sint* sparse;
	u8* info;
	sint bucket_size;
	sint len;
	sint cap;
	sint(*hash)(void* a);
	sint(*cmp)(void* a, void* b);
} flat_ordered_hashmap;



/*************************************************************************************************/

/* The elem size should always be >= sizeof(uptr_t). 20 - 24 bytes. */
typedef struct pool
{
	void** data;
	sint elem_size;
	sint len;
	sint cap;
	sint bcap;
} pool;



/*************************************************************************************************/

sint get_container_capacity(sint len);



/*************************************************************************************************/

/* 'size' is the size of a single elem in bytes. */
vector vector_init(sint size, sint len);

void vector_destroy(vector* vec);

void vector_reserve(vector* vec, sint len);

/* Trim array to lowest fitting capacity. */
void vector_trim(vector* vec);

void* vector_get(vector* vec, sint idx);

void vector_insert(vector* vec, void* elem, sint idx);

sint vector_push(vector* vec, void* elem);

void vector_erase(vector* vec, sint idx);

sint vector_find(vector* vec, void* elem, cmpfunc cmp);



/*************************************************************************************************/

string string_init(sint len);

void string_destroy(string* str);

void string_reserve(string* str, sint len);

/* Trim string to lowest fitting capacity. */
void string_trim(string* str);

char* string_get(string* str, sint idx);

void string_insert(string* str, char* seq, sint len, sint idx);

void string_append(string* str, char* seq, sint len);

void string_erase(string* str, sint idx, sint len);

u16* string_to_utf16(string* str, sint* out_len);

s32* string_to_utf32(string* str, sint* out_len);

string string_from_utf16(u16* str);

string string_from_utf32(s32* str);

sint string_cmp(string* a, string* b);

sint string_hash(const char* string);

sint utf8_symbol_size(const char c);

sint utf8_len(const char* str);

sint utf8_len_utf16(const u16* str);

sint utf8_len_utf32(const s32* str);

sint utf8_encode(char* str, s32 symbol);

sint utf8_decode(const char* str, s32* out_symbol);

u16* utf8_to_utf16(const char* str, sint* out_len);

s32* utf8_to_utf32(const char* str, sint* out_len);

sint utf16_symbol_size(const u16 c);

sint utf16_len(const u16* str);

sint utf16_len_utf8(const char* str);

sint utf16_encode(u16* str, s32 symbol);

sint utf16_decode(const u16* str, s32* out_symbol);

sint utf32_len_utf8(const char* str);



/*************************************************************************************************/

/* 'size' is the size of a single bucket in bytes. */
flat_map flat_map_init(sint size, sint len, cmpfunc cmp);

void flat_map_destroy(flat_map* map);

void flat_map_reserve(flat_map* map, sint len);

/* Trim array to lowest fitting capacity. */
void flat_map_trim(flat_map* map);

void* flat_map_get(flat_map* map, void* bucket);

void* flat_map_push(flat_map* map, void* bucket);

/* 'out_bucket' is used to store the data of a bucket if found. */
sint flat_map_pop(flat_map* map, void* bucket, void* out_bucket);



/*************************************************************************************************/

/* 'size' is the size of a single bucket in bytes. */
flat_hashmap flat_hashmap_init(sint size, sint len, hashfunc hash, cmpfunc cmp);

void flat_hashmap_destroy(flat_hashmap* map);

void flat_hashmap_reserve(flat_hashmap* map, sint len);

/* Trim array to lowest fitting capacity. */
void flat_hashmap_trim(flat_hashmap* map);

void* flat_hashmap_get(flat_hashmap* map, void* bucket);

void* flat_hashmap_push(flat_hashmap* map, void* bucket);

/* 'out_bucket' is used to store the data of a bucket if found. */
sint flat_hashmap_pop(flat_hashmap* map, void* bucket, void* out_bucket);



/*************************************************************************************************/

/* 'size' is the size of a single bucket in bytes. */
flat_ordered_hashmap flat_ordered_hashmap_init(sint size, sint len, sint(*hash)(void* a), sint(*cmp)(void* a, void* b));

void flat_ordered_hashmap_destroy(flat_ordered_hashmap* map);

void flat_ordered_hashmap_reserve(flat_ordered_hashmap* map, sint len);

/* Trim vector to lowest fitting capacity. */
void flat_ordered_hashmap_trim(flat_ordered_hashmap* map);

void* flat_ordered_hashmap_get(flat_ordered_hashmap* map, void* bucket);

void* flat_ordered_hashmap_push(flat_ordered_hashmap* map, void* bucket);

/* 'out_bucket' is used to store the data of a bucket if found. */
sint flat_ordered_hashmap_pop(flat_ordered_hashmap* map, void* bucket, void* out_bucket);



/*************************************************************************************************/

/* 'size' is the size of a single elem in bytes.
 * 'bcap' is the number of elements a single block/allocation can hold. */
pool pool_init(sint size, sint len, sint bcap);

void pool_destroy(pool* pool);

/* 'size' is the size of a single elem in bytes. */
void pool_reserve(pool* pool, sint len);

/* 'size' is the size of a single elem in bytes. */
void* pool_get(pool* pool, sint idx);

/* 'size' is the size of a single elem in bytes. */
u32 pool_push(pool* pool, void* elem);

/* 'size' is the size of a single elem in bytes. */
void pool_erase(pool* pool, sint idx);


