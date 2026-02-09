#include "containers.h"



/*************************************************************************************************/

sint get_container_capacity(sint len)
{
	return len < 8 ? 8 : (UINT_MAX >> clz(len - 1)) + 1;
}



/*************************************************************************************************/

vector vector_init(sint size, sint len)
{
	vector array = { NULL, size, 0, 0 };
	vector_reserve(&array, len);
	return array;
}

void vector_destroy(vector* vec)
{
	free(vec->data);
}

void vector_reserve(vector* vec, sint len)
{
	if (vec->cap < len)
	{
		void* new_data;

		len = get_container_capacity(len);
		new_data = malloc((uptr)vec->elem_size * len);
		vec->cap = len;

		memcpy(new_data, vec->data, (uptr)vec->len * vec->elem_size);
		vector_destroy(vec);
		vec->data = new_data;
	}
}

void vector_trim(vector* vec)
{
	if ((vec->cap / 4) > vec->len)
	{
		vec->cap = get_container_capacity(vec->len * 2);
		vec->data = realloc(vec->data, (uptr)vec->cap * vec->elem_size);
	}
}

void* vector_get(vector* vec, sint idx)
{
	return (void*)((uptr)vec->data + (uptr)idx * vec->elem_size);
}

void vector_insert(vector* vec, void* elem, sint idx)
{
	void* dst, *src;

	vector_reserve(vec, vec->len + 1);
	src = (void*)((uptr)vec->data + (uptr)idx * vec->elem_size);
	dst = (void*)((uptr)src + vec->elem_size);
	memmove(dst, src, (uptr)(vec->len - idx) * vec->elem_size);
	memcpy(src, elem, vec->elem_size);
	vec->len++;
}

sint vector_push(vector* vec, void* elem)
{
	vector_insert(vec, elem, vec->len);
	return vec->len - 1;
}

void vector_erase(vector* vec, sint idx)
{
	void* src, * dst;
	uptr sz;

	if (idx >= vec->len)
		return;

	vec->len--;

	sz = (uptr)idx * (uptr)vec->elem_size;
	src = (void*)((uptr)vec->data + sz + vec->elem_size);
	dst = (void*)((uptr)vec->data + sz);
	sz = (uptr)(vec->len - idx) * vec->elem_size;

	memmove(dst, src, sz);
	vector_trim(vec);
}

sint vector_find(vector* vec, void* elem, cmpfunc cmp)
{
	sint i;
	uptr e, end;

	e = (uptr)vec->data;
	end = e + (uptr)vec->len * vec->elem_size;

	for (i = 0; e < end; e += vec->elem_size, i++)
	{
		if (cmp(elem, (void*)e) == 0)
			return i;
	}

	return INVALID_INDEX;
}



/*************************************************************************************************/

string string_init(sint len)
{
	string str = { NULL, 0, 0 };
	string_reserve(&str, len);
	return str;
}

void destroy_string(string* str)
{
	free(str->data);
}

void string_reserve(string* str, sint len)
{
	if (str->cap <= len)
	{
		char* new_data;

		str->cap = get_container_capacity(len + 1);
		new_data = (char*)malloc(str->cap);
		memcpy(new_data, str->data, str->len);
		new_data[str->len] = 0;
		destroy_string(str);
		str->data = new_data;
	}
}

void string_trim(string* str)
{
	if ((str->cap / 4) > str->len)
	{
		str->cap = get_container_capacity(str->len * 2);
		str->data = realloc(str->data, (uptr)str->cap);
	}
}

char* string_get(string* str, sint idx)
{
	return str->data + idx;
}

void string_insert(string* str, char* seq, sint len, sint idx)
{
	string_reserve(str, str->len + len);
	memmove(str->data + idx + len, str->data + idx, str->len - idx);
	memcpy(str->data + idx, seq, len);
	str->len += len;
	str->data[str->len] = 0;
}

void string_append(string* str, char* seq, sint len)
{
	string_insert(str, seq, len, str->len);
}

void string_erase(string* str, sint idx, sint len)
{
	if ((idx + len) > str->len)
		return;

	memmove(str->data + idx, str->data + idx + len, str->len - idx - len);
	str->len -= len;
	str->data[str->len] = 0;
	string_trim(str);
}

u16* string_to_utf16(string* str, sint* out_len)
{
	sint x, y, size;
	s32 symbol;
	u16* string;

	size = utf16_len_utf8(str->data);
	string = malloc(sizeof(u16) * (size + 1));
	string[size] = 0;
	
	if (out_len != NULL)
		*out_len = size;

	for (x = 0, y = 0; str->data[x] != 0;)
		x += utf8_decode(str->data + x, &symbol),
		y += utf16_encode(string + y, symbol);

	return string;
}

s32* string_to_utf32(string* str, sint* out_len)
{
	sint x, y, size;
	s32 symbol;
	s32* string;

	size = utf32_len_utf8(str->data);
	string = malloc(sizeof(s32) * (size + 1));
	string[size] = 0;
	
	if (out_len != NULL)
		*out_len = size;

	for (x = 0, y = 0; str->data[x] != 0;)
		x += utf8_decode(str->data + x, &symbol),
		string[y++] = symbol;

	return string;
}

string string_from_utf16(u16* str)
{
	sint x, y, size;
	s32 symbol;
	string string;

	size = utf8_len_utf16(str);
	string = string_init(size);
	string.len = size;
	string.data[size] = 0;

	for (x = 0, y = 0; str[x] != 0;)
		x += utf16_decode(str + x, &symbol),
		y += utf8_encode(string.data + y, symbol);

	return string;
}

string string_from_utf32(s32* str)
{
	sint x, y, size;
	s32 symbol;
	string string;

	size = utf8_len_utf32(str);
	string = string_init(size);
	string.len = size;
	string.data[size] = 0;

	for (x = 0, y = 0; str[x] != 0;)
		symbol = str[x++], y += utf8_encode(string.data + y, symbol);

	return string;
}

sint string_cmp(string* a, string* b)
{
	return strcmp(a->data, b->data);
}

sint string_hash(const char* string)
{
	sint multiplier = 31;
	sint i, m, hash = 0;

	for (i = 0, m = 1; string[i] != 0; m *= multiplier, i++)
	{
		hash += string[i] * m;
	}

	return hash;
}

/* UTF-8
 * 01111111                             7-bit ASCII characters
 * 110xxxxx 10xxxxxx                    2-byte sequence 11-bit characters
 * 1110xxxx 10xxxxxx 10xxxxxx           3-byte sequence 16-bit characters
 * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  4-byte sequence 21-bit characters
 *
 * ASCII: 0 - 127
 *
 * Start: 192u - 253u		||	-64 - -3
 * Following: 128u - 191u	||	-128 - -65
 *
 * Start 2: 192u - 223u		||	-64 - -33
 * Start 3: 224u - 239u		||	-32 - -17
 * Start 4: 240u - 247u		||	-16 - -9
 * Start 5: 248u - 251u		||	-8 - -5
 * Start 6: 252u - 253u		||	-4 - -3
 *
 * UTF-16 Little Endian & Big Endian (standard x86 is little endian, the byte order is mirrored
 * on big endian)
 * xxxxxxxx xxxxxxxx                    16-bit Unicode characters
 * 110110xx xxxxxxxx                    High
 * 110111xx xxxxxxxx                    Low
 * 110110xx xxxxxxxx 110110xx xxxxxxxx  4-byte sequence 20-bit charcaters
 * Everything between
 * (11011000 00000000 - 11011011 11111111) (0xD800 - 0xDBFF)
 * (11011100 00000000 - 11011111 11111111) (0xDC00 - 0xDFFF)
 * is reserved space in unicode so you can identify UTF-16 "Surrogate Pairs". */

sint utf8_symbol_size(const char c)
{
	return 1 + ((u8)c > 0xBF) + ((u8)c > 0xDF) + ((u8)c > 0xEF);
}

sint utf8_len(const char* str)
{
	sint len;

	if (str == NULL)
		return 0;

	for (len = 0; str[len] != 0; len += utf8_symbol_size(str[len]));
	return len;
}

sint utf8_len_utf16(const u16* str)
{
	sint i, len;

	if (str == NULL)
		return 0;

	for (i = 0, len = 0; str[i] != 0; utf16_symbol_size(str[i]) == 2 ?
		(len += 4, i += 2) : (len += 1 + (str[i] > 0x7F) + (str[i] > 0x7FF), i++));
	return len;
}

sint utf8_len_utf32(const s32* str)
{
	sint i, len;
	for (i = 0, len = 0; str[i] != 0; len += 1 + (str[i] > 0x7F) + (str[i] > 0x7FF) + (str[i] > 0xFFFF), i++);
	return len;
}

sint _utf8_encode(char* str, s32 symbol)
{
	sint len = (symbol > 0x7F) + (symbol > 0x7FF) + (symbol > 0xFFFF);
	sint bits = len * 6;
	sint i = 0;
	char header = ~(0xFF >> (len + 1)) * (len != 0);
	
	str[i++] = header + (symbol >> bits);
	while (bits > 0)
		str[i++] = 0x80 + ((symbol >> (bits -= 6)) & 0x3F);
	return len + 1; 
}

sint utf8_encode(char* str, s32 symbol)
{
	if (symbol < 0x80)
	{
		str[0] = symbol;
		return 1;
	}
	else if (symbol < 0x800)
	{
		str[0] = 0xC0 | (char)(symbol >> 6);
		str[1] = 0x80 | (char)(symbol & 0x3F);
		return 2;
	}
	else if (symbol < 0x10000)
	{
		str[0] = 0xE0 | (char)(symbol >> 12);
		str[1] = 0x80 | (char)(symbol >> 6 & 0x3F);
		str[2] = 0x80 | (char)(symbol & 0x3F);
		return 3;
	}
	else
	{
		str[0] = 0xF0 | (char)(symbol >> 18);
		str[1] = 0x80 | (char)(symbol >> 12) & 0x3F;
		str[2] = 0x80 | (char)(symbol >> 6) & 0x3F;
		str[3] = 0x80 | (char)(symbol & 0x3F);
		return 4;
	}
}

sint utf8_decode(const char* str, s32* out_symbol)
{
	switch (utf8_symbol_size(str[0]))
	{
	default:
		return 0;
	case 0:
		*out_symbol = 0;
		return 0;
	case 1:
		*out_symbol = str[0];
		return 1;
	case 2:
		*out_symbol = ((0x1F & str[0]) << 6) | (0x1F & str[1]);
		return 2;
	case 3:
		*out_symbol = ((0xF & str[0]) << 12) | ((0x1F & str[1]) << 6) | (0x1F & str[2]);
		return 3;
	case 4:
		*out_symbol = ((0x7 & str[0]) << 18) | ((0x1F & str[1]) << 12) | ((0x1F & str[2]) << 6) | (0x1F & str[3]);
		return 4;
	}
}

u16* utf8_to_utf16(const char* str, sint* out_len)
{
	sint x, y, size;
	s32 symbol;
	u16* string;

	size = utf16_len_utf8(str);
	string = malloc(sizeof(u16) * (size + 1));
	string[size] = 0;
	
	if (out_len != NULL)
		*out_len = size;

	for (x = 0, y = 0; str[x] != 0;)
		x += utf8_decode(str + x, &symbol),
		y += utf16_encode(string + y, symbol);

	return string;
}

s32* utf8_to_utf32(const char* str, sint* out_len)
{
	sint x, y, size;
	s32 symbol;
	s32* string;

	size = utf32_len_utf8(str);
	string = malloc(sizeof(s32) * (size + 1));
	string[size] = 0;

	if (out_len != NULL)
		*out_len = size;

	for (x = 0, y = 0; str[x] != 0;)
		x += utf8_decode(str + x, &symbol),
		string[y++] = symbol;

	return string;
}

sint utf16_symbol_size(const u16 c)
{
	return 1 + ((c > 0xD7FF) & (c < 0xE000));
}

sint utf16_len(const u16* str)
{
	sint len;

	if (str == NULL)
		return 0;

	for (len = 0; str[len] != 0; len += utf16_symbol_size(str[len]));
	return len;
}

sint utf16_len_utf8(const char* str)
{
	sint i, len;

	if (str == NULL)
		return 0;

	for (i = 0, len = 0; str[i] != 0;)
	{
		sint size;

		size = utf8_symbol_size(str[i]);
		len += 1 + (size > 3);
		i += size;
	}

	return len;
}

sint utf16_encode(u16* str, s32 symbol)
{
	if (symbol < 0x10000)
	{
		str[0] = (u16)symbol;
		return 1;
	}
	else
	{
		symbol -= 0x10000;
		str[0] = 0xD800 | ((symbol >> 10) & 0x3FF);
		str[1] = 0xDC00 | (symbol & 0x3FF);
		return 2;
	}
}

sint utf16_decode(const u16* str, s32* out_symbol)
{
	sint size;

	size = utf16_symbol_size(str[0]);
	size == 1 ? (*out_symbol = str[0]) :
		(*out_symbol = ((((sint)str[0] & 0x3FF) << 10) | ((sint)str[1] & 0x3FF)) + 0x10000);
	return size;
}

sint utf32_len_utf8(const char* str)
{
	sint i, len;

	if (str == NULL)
		return 0;

	for (i = 0, len = 0; str[i] != 0; i += utf8_symbol_size(str[len]), i++);
	return len;
}



/*************************************************************************************************/

flat_map flat_map_init(sint size, sint len, cmpfunc cmp)
{
	flat_map map = { NULL, size, 0, 0, cmp };
	flat_map_reserve(&map, len);
	return map;
}

void flat_map_destroy(flat_map* map)
{
	free(map->buckets);
}

void flat_map_reserve(flat_map* map, sint len)
{
	if (map->cap < len)
	{
		void* new_data;

		len = get_container_capacity(len);
		new_data = malloc((uptr)len * map->bucket_size);
		map->cap = len;

		memcpy(new_data, map->buckets, (uptr)map->len * map->bucket_size);
		flat_map_destroy(map);
		map->buckets = new_data;
	}
}

void flat_map_trim(flat_map* map)
{
	if ((map->cap / 4) > map->len)
	{
		map->cap = get_container_capacity(map->len * 2);
		map->buckets = realloc(map->buckets, (uptr)map->cap * map->bucket_size);
	}
}

void* flat_map_get_index(flat_map* map, void* bucket, sint* out_index)
{
	if (map->len == 0)
	{
		*out_index = 0;
		return NULL;
	}

	sint low, mid, high;
	sint result;

	low = 0; high = map->len - 1;

	while (low <= high)
	{
		void* _bucket;

		mid = low + ((high - low) / 2);
		_bucket = (u8*)map->buckets + ((uptr)mid * map->bucket_size);
		result = map->cmp(bucket, _bucket);

		if (result > 0)
		{
			low = mid + 1;
		}
		else if (result < 0)
		{
			if (mid != 0)
			{
				high = mid - 1;
				continue;
			}

			*out_index = 0;
			return NULL;
		}
		else
		{
			*out_index = mid;
			return _bucket;
		}
	}

	*out_index = low;
	return NULL;
}

void* flat_map_get(flat_map* map, void* bucket)
{
	sint index;
	return flat_map_get_index(map, bucket, &index);
}

void* flat_map_push(flat_map* map, void* bucket)
{
	sint index;
	void* _bucket;

	_bucket = flat_map_get_index(map, bucket, &index);

	if (_bucket != NULL)
		return _bucket;

	vector_insert((vector*)map, bucket, index);
	return NULL;
}

sint flat_map_pop(flat_map* map, void* bucket, void* out_bucket)
{
	sint index;
	void* _bucket;

	_bucket = flat_map_get_index(map, bucket, &index);

	if (_bucket != NULL)
	{
		memcpy(out_bucket, _bucket, map->bucket_size);
		vector_erase((vector*)map, index);
		return SUCCESS;
	}

	return INVALID_INDEX;
}



/*************************************************************************************************/

flat_hashmap flat_hashmap_init(sint size, sint len, hashfunc hash, cmpfunc cmp)
{
	sint sz, cap;
	flat_hashmap map;

	cap = get_container_capacity(len);
	sz = (cap + 2) * size;

	map.buckets = malloc(sz + cap);
	map.info = (u8*)map.buckets + sz;
	map.bucket_size = size;
	map.cap = cap;
	map.len = 0;
	map.hash = hash;
	map.cmp = cmp;

	memset(map.info, U8_MAX, map.cap);
	return map;
}

void flat_hashmap_destroy(flat_hashmap* map)
{
	free(map->buckets);
}

void flat_hashmap_reserve(flat_hashmap* map, sint len)
{
	if (len > (map->cap - (map->cap / 4)))
	{
		flat_hashmap new_map;
		sint i;

		new_map = flat_hashmap_init(map->bucket_size, len * 2, map->hash, map->cmp);

		for (i = 0; i < map->cap; i++)
		{
			if (map->info[i] != U8_MAX)
			{
				void* _bucket = (void*)((uptr)map->buckets + ((uptr)map->bucket_size * i));
				flat_hashmap_push(&new_map, _bucket);
			}
		}

		flat_hashmap_destroy(map);
		*map = new_map;
	}
}

void flat_hashmap_trim(flat_hashmap* map)
{
	if ((map->cap - (map->cap - (map->cap / 4))) > map->len)
	{
		flat_hashmap new_map;
		sint i;

		new_map = flat_hashmap_init(map->bucket_size, map->len, map->hash, map->cmp);

		for (i = 0; i < map->cap; i++)
		{
			if (map->info[i] != U8_MAX)
			{
				void* _bucket = (void*)((uptr)map->buckets + ((uptr)map->bucket_size * i));
				flat_hashmap_push(&new_map, _bucket);
			}
		}

		flat_hashmap_destroy(map);
		*map = new_map;
	}
}

void* flat_hashmap_get_index(flat_hashmap* map, sint hash, void* bucket, sint* out_index)
{
	sint mask, idx, distance;

	mask = map->cap - 1;
	idx = hash & mask;
	distance = 0;

	for (;; idx = (idx + 1) & mask, distance++)
	{
		void* _bucket;
		sint _distance;

		_bucket = (void*)((uptr)map->buckets + ((uptr)idx * map->bucket_size));
		_distance = map->info[idx];

		if ((_distance == U8_MAX) | (distance > _distance))
		{
			*out_index = idx;
			return NULL;
		}
		else if (map->cmp(bucket, _bucket) == 0)
		{
			*out_index = idx;
			return _bucket;
		}
	}
}

void* flat_hashmap_get(flat_hashmap* map, void* bucket)
{
	sint idx;
	return flat_hashmap_get_index(map, map->hash(bucket), bucket, &idx);
}

void* flat_hashmap_push(flat_hashmap* map, void* bucket)
{
	sint mask, hash, idx, distance;
	void* _bucket;

	flat_hashmap_reserve(map, map->len + 1);

	hash = map->hash(bucket);
	_bucket = flat_hashmap_get_index(map, hash, bucket, &idx);

	if (_bucket != NULL)
		return _bucket;

	mask = map->cap - 1;
	distance = ((map->cap + idx) - (hash & mask)) & mask;

	for (;; idx = (idx + 1) & mask, distance++)
	{
		sint _distance;

		_bucket = (void*)((uptr)map->buckets + ((uptr)idx * map->bucket_size));
		_distance = map->info[idx];

		if (_distance == U8_MAX)
		{
			memcpy(_bucket, bucket, map->bucket_size);
			map->info[idx] = distance;
			map->len++;
			return NULL;
		}
		else if (distance > _distance)
		{
			void* tmp_one, * tmp_two;

			tmp_one = (void*)((uptr)map->buckets + ((uptr)map->cap * map->bucket_size));
			tmp_two = (void*)((uptr)tmp_one + map->bucket_size);

			memcpy(tmp_one, _bucket, map->bucket_size);
			memcpy(_bucket, bucket, map->bucket_size);
			memcpy(tmp_two, tmp_one, map->bucket_size);

			bucket = tmp_two;
			map->info[idx] = distance;
			distance = _distance;
		}
	}
}

sint flat_hashmap_pop(flat_hashmap* map, void* bucket, void* out_key)
{
	sint mask, hash, idx, next;

	hash = map->hash(bucket);

	if (flat_hashmap_get_index(map, hash, bucket, &idx) == NULL)
		return INVALID_INDEX;

	memcpy(out_key, (void*)((uptr)map->buckets + ((uptr)idx * map->bucket_size)), map->bucket_size);

	mask = map->cap - 1;

	for (next = (idx + 1) & mask;; idx = next, next = (next + 1) & mask)
	{
		void* _bucket;
		void* next_bucket;
		sint next_distance;

		next_distance = map->info[next];

		if ((next_distance == U8_MAX) | (next_distance == 0))
		{
			map->info[idx] = U8_MAX;
			map->len--;
			break;
		}

		_bucket = (void*)((uptr)map->buckets + ((uptr)idx * map->bucket_size));
		next_bucket = (void*)((uptr)_bucket + map->bucket_size);

		memcpy(_bucket, next_bucket, map->bucket_size);
		map->info[idx] = next_distance - 1;
	}

	flat_hashmap_trim(map);

	return SUCCESS;
}



/**************************************************************************************************/
/*	flat_ordered_hashmap_t  */

flat_ordered_hashmap flat_ordered_hashmap_init(sint size, sint len, sint(*hash)(void* a), sint(*cmp)(void* a, void* b))
{
	sint sz, cap, rest;
	flat_ordered_hashmap map;

	cap = get_container_capacity(len);
	sz = (cap * size);
	rest = sz % sizeof(sint);

	if (rest != 0)
		sz += sizeof(sint) - rest;

	map.dense = malloc(sz + (sizeof(sint) * cap) + cap);
	map.sparse = (sint*)((uptr)map.dense + sz);
	map.info = (u8*)(map.sparse + cap);
	map.bucket_size = size;
	map.cap = cap;
	map.len = 0;
	map.hash = hash;
	map.cmp = cmp;

	memset(map.info, -1, map.cap);
	return map;
}

void flat_ordered_hashmap_destroy(flat_ordered_hashmap* map)
{
	free(map->dense);
}

void flat_ordered_hashmap_reserve(flat_ordered_hashmap* map, sint len)
{
	if (len > (map->cap - (map->cap / 4)))
	{
		flat_ordered_hashmap new_map;
		sint i;

		new_map = flat_ordered_hashmap_init(map->bucket_size, len * 2, map->hash, map->cmp);

		for (i = 0; i < map->len; i++)
		{
			void* _bucket = (void*)((uptr)map->dense + (uptr)map->bucket_size * i);
			flat_ordered_hashmap_push(&new_map, _bucket);
		}

		flat_ordered_hashmap_destroy(map);
		*map = new_map;
	}
}

void flat_ordered_hashmap_trim(flat_ordered_hashmap* map)
{
	if ((map->cap - (map->cap - (map->cap / 4))) > map->len)
	{
		flat_ordered_hashmap new_map;
		sint i;

		new_map = flat_ordered_hashmap_init(map->bucket_size, map->len, map->hash, map->cmp);

		for (i = 0; i < map->len; i++)
		{
			void* _bucket = (void*)((uptr)map->dense + ((uptr)map->bucket_size * i));
			flat_ordered_hashmap_push(&new_map, _bucket);
		}

		flat_ordered_hashmap_destroy(map);
		*map = new_map;
	}
}

void* flat_ordered_hashmap_get_index(flat_ordered_hashmap* map, sint hash, sint* bucket, sint* out_index)
{
	int mask, idx, distance;

	mask = map->cap - 1;
	idx = hash & mask;
	distance = 0;

	for (;; idx = (idx + 1) & mask, distance++)
	{
		void* _bucket;
		sint _distance;

		_distance = map->info[idx];

		if ((_distance == 0xFF) | (distance > _distance))
		{
			*out_index = idx;
			return NULL;
		}

		_bucket = (void*)((uptr)map->dense + ((uptr)map->sparse[idx] * map->bucket_size));

		if (map->cmp(bucket, _bucket) == 0)
		{
			*out_index = idx;
			return _bucket;
		}
	}
}

void* flat_ordered_hashmap_get(flat_ordered_hashmap* map, void* bucket)
{
	sint idx;
	return flat_ordered_hashmap_get_index(map, map->hash(bucket), bucket, &idx);
}

void* flat_ordered_hashmap_push(flat_ordered_hashmap* map, void* bucket)
{
	sint mask, hash, idx, distance, tmp;
	void* _bucket;

	flat_ordered_hashmap_reserve(map, map->len + 1);

	hash = map->hash(bucket);
	_bucket = flat_ordered_hashmap_get_index(map, hash, bucket, &idx);

	if (_bucket != NULL)
		return _bucket;

	mask = map->cap - 1;
	distance = ((map->cap + idx) - (hash & mask)) & mask;

	for (tmp = -1;; idx = (idx + 1) & mask, distance++)
	{
		sint _distance = map->info[idx];

		if (_distance == 0xFF)
		{
			if (tmp == -1)
			{
				/* Add bucket to dense */
				_bucket = (void*)((size_t)map->dense + ((size_t)map->len * map->bucket_size));
				memcpy(_bucket, bucket, map->bucket_size);
				map->sparse[idx] = map->len++;
			}
			else
			{
				/* Replace index */
				map->sparse[idx] = tmp;
			}

			map->info[idx] = distance;
			return NULL;
		}
		else if (distance > _distance)
		{
			sint tmp_x = map->sparse[idx];

			if (tmp == -1)
			{
				/* Add bucket to dense */
				_bucket = (void*)((size_t)map->dense + ((size_t)map->len * map->bucket_size));
				memcpy(_bucket, bucket, map->bucket_size);
				map->sparse[idx] = map->len++;
			}
			else
			{
				/* Replace index */
				map->sparse[idx] = tmp;
			}

			tmp = tmp_x;
			map->info[idx] = distance;
			distance = _distance;
		}
	}
}

sint flat_ordered_hashmap_pop(flat_ordered_hashmap* map, void* bucket, void* out_bucket)
{
	sint hash, idx, mask, next, rest, dense_idx;
	void* first;

	hash = map->hash(bucket);

	if (flat_ordered_hashmap_get_index(map, hash, bucket, &idx) == NULL)
		return INVALID_INDEX;

	dense_idx = map->sparse[idx];
	first = (void*)((size_t)map->dense + ((size_t)dense_idx * map->bucket_size));
	rest = (size_t)(map->len - dense_idx - 1) * map->bucket_size;

	memcpy(out_bucket, first, map->bucket_size);
	memmove(first, (void*)((size_t)first + map->bucket_size), rest);
	map->len--;

	mask = map->cap - 1;

	for (next = (idx + 1) & mask;; idx = next, next = (next + 1) & mask)
	{
		sint next_distance = map->info[next];

		if ((next_distance == 0xFF) | (next_distance == 0))
		{
			map->info[idx] = 0xFF;
			break;
		}

		map->sparse[idx] = map->sparse[next];
		map->info[idx] = next_distance - 1;
	}

	flat_ordered_hashmap_trim(map);

	return SUCCESS;
}



/**************************************************************************************************/
/*	pool_t  */


