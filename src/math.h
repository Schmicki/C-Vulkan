#pragma once



#include "core.h"



/*************************************************************************************************/

typedef struct vec2
{
	flt x;
	flt y;
} vec2;

typedef struct vec3
{
	flt x;
	flt y;
	flt z;
} vec3;

typedef struct vec4
{
	flt x;
	flt y;
	flt z;
	flt w;
} vec4, quat;

typedef struct mat3
{
	flt m[9];
} mat3;

typedef struct mat4
{
	flt m[16];
} mat4;

typedef struct transform
{
	vec3 location;
	quat rotation;
	vec3 scale;
} transform;



/*************************************************************************************************/

typedef struct ray
{
	vec3 origin;
	vec3 direction;
} ray;

typedef struct plane
{
	vec3 normal;
	flt distance;
} plane;

typedef struct line
{
	vec3 start;
	vec3 end;
} line;

typedef struct triangle
{
	vec3 a, b, c;
} triangle;

typedef struct sphere
{
	vec3 origin;
	flt radius;
} sphere;

typedef struct capsule
{
	line line;
	flt radius;
} capsule;

typedef struct aabb
{
	vec3 min, max;
} aabb;

typedef struct obb
{
	vec3 origin;
	vec3 axes[3];
	vec3 extents;
} obb;



/*************************************************************************************************/

vec2 vec2_set(flt x, flt y);
vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mul(vec2 a, flt v);
vec2 vec2_div(vec2 a, flt v);
flt vec2_dot(vec2 a, vec2 b);
flt vec2_len(vec2 a);
vec2 vec2_unit(vec2 a);
vec2 vec2_lerp(vec2 from, vec2 to, flt alpha);
/* vec2_t vec2_rotate(vec2_t a, flt_t r); */
/* vec2_t vec2_transform(vec2_t a, mat3_t t); */
s32 vec2_cmp(vec2 a, vec2 b);



/*************************************************************************************************/

vec3 vec3_set(flt x, flt y, flt z);
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_mul(vec3 a, flt v);
vec3 vec3_div(vec3 a, flt v);
flt vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
flt vec3_len(vec3 a);
vec3 vec3_unit(vec3 a);
vec3 vec3_lerp(vec3 from, vec3 to, flt alpha);
vec3 vec3_rotate(vec3 a, quat r);
vec3 vec3_transform(vec3 a, mat4 t);
s32 vec3_cmp(vec3 a, vec3 b);



/*************************************************************************************************/

vec4 vec4_set(flt x, flt y, flt z, flt w);
vec4 vec4_add(vec4 a, vec4 b);
vec4 vec4_sub(vec4 a, vec4 b);
vec4 vec4_mul(vec4 a, flt v);
vec4 vec4_div(vec4 a, flt v);
flt vec4_dot(vec4 a, vec4 b);
flt vec4_len(vec4 a);
vec4 vec4_unit(vec4 a);
vec4 vec4_lerp(vec4 from, vec4 to, flt alpha);
vec4 vec4_transform(vec4 a, mat4 t);
s32 vec4_cmp(vec4 a, vec4 b);



/*************************************************************************************************/

quat quat_from_euler_angles(vec3 angles);
quat quat_from_axis_angle(vec3 axis, flt angle);
quat quat_add(quat a, quat b);
quat quat_sub(quat a, quat b);
quat quat_mul(quat a, flt v);
quat quat_div(quat a, flt v);
flt quat_dot(quat a, quat b);
flt quat_len(quat a);
quat quat_conj(quat a);
quat quat_inv(quat a);
quat quat_unit(quat a);
quat quat_lerp(quat from, quat to, flt alpha);
quat quat_slerp(quat from, quat to, flt alpha);
quat quat_slerpl(quat from, quat to, flt alpha);
quat quat_nlerp(quat from, quat to, flt alpha);
quat quat_rotate(quat a, quat r);



/*************************************************************************************************/

mat3 mat3_rotation(quat r);
mat3 mat3_scale(vec3 s);
mat3 mat3_from_mat4(mat4 m);
mat3 mat3_add(mat3 a, mat3 b);
mat3 mat3_sub(mat3 a, mat3 b);
mat3 mat3_mul(mat3 a, mat3 b);
mat3 mat3_transpose(mat3 a);



/*************************************************************************************************/

mat4 mat4_translation(vec3 t);
mat4 mat4_rotation(quat r);
mat4 mat4_scale(vec3 s);
mat4 mat4_model(transform transform);
mat4 mat4_view(vec3 location, vec3 rotation);
/* 'far' <= abs('near') to get infinite far-plane. 'near' < 0 to reverse z. */
mat4 mat4_perspective_vk(flt fov, flt ratio, flt near, flt far);
/* 'far' <= abs('near') to get infinite far-plane. 'near' < 0 to reverse z. */
mat4 mat4_perspective_gl(flt fov, flt ratio, flt near, flt far);
/* 'far' <= abs('near') to get infinite far-plane. 'near' < 0 to reverse z. */
mat4 mat4_perspective_gu(flt fov, flt ratio, flt near, flt far);
mat4 mat4_mvp(mat4 model, mat4 view, mat4 projection);
mat4 mat4_from_mat3(mat3 m);
mat4 mat4_add(mat4 a, mat4 b);
mat4 mat4_sub(mat4 a, mat4 b);
mat4 mat4_mul(mat4 a, mat4 b);
mat4 mat4_transpose(mat4 a);



/*************************************************************************************************/

#define PI (flt)3.141592653589793
#define flt_rtod(r) (r * ((flt)180.0 / PI))
#define flt_dtor(d) (d * (PI / (flt)180.0))

flt flt_abs(flt v);
flt flt_min(flt a, flt b);
flt flt_max(flt a, flt b);
flt flt_trunc(flt v);
flt flt_round(flt v);
flt flt_mod(flt v, flt mod);
flt flt_clamp(flt v, flt min, flt max);
sint flt_isclose(flt a, flt b, flt epsilon);
flt flt_sin(flt v);
flt flt_asin(flt v);
flt flt_cos(flt v);
flt flt_acos(flt v);
flt flt_tan(flt v);
flt flt_atan(flt v);
flt flt_sqrt(flt v);
flt flt_pow(flt v, flt exp);
flt flt_log2(flt v);



/**************************************************************************************************/

ray ray_set(vec3 origin, vec3 direction);



/**************************************************************************************************/

plane plane_from_points(vec3 a, vec3 b, vec3 c);



/**************************************************************************************************/

line line_set(vec3 start, vec3 end);



/**************************************************************************************************/

triangle triangle_from_points(vec3 a, vec3 b, vec3 c);



/**************************************************************************************************/

sphere sphere_set(vec3 origin, flt radius);
sphere sphere_from_points(const vec3* points, sint num);
sphere sphere_add_point(sphere s, vec3 p);



/**************************************************************************************************/

capsule capsule_set(vec3 start, vec3 end, flt radius);



/*************************************************************************************************/

aabb aabb_set(vec3 min, vec3 max);
aabb aabb_from_extends(vec3 origin, vec3 extends);
aabb aabb_from_sphere(vec3 origin, flt radius);
aabb aabb_from_points(const vec3* points, sint size);
aabb aabb_transform(aabb box, mat4 transform);
aabb aabb_union(aabb a, aabb b);
flt aabb_area(aabb a);

void most_distant_points_on_aabb(const vec3* points, sint num, sint* min, sint* max);



/**************************************************************************************************/

obb obb_set(vec3 origin, vec3 axis[3], vec3 extents);
obb obb_from_points(const vec3* points, sint num);



/***************************************************************************************************
	Closest Point Computation */

flt distance_point_plane(plane p, vec3 pt);
flt distance2_point_line(line l, vec3 p);
flt distance2_point_aabb(aabb box, vec3 p);
flt distance2_point_obb(obb box, vec3 p);

vec3 closest_point_ray(ray r, vec3 p);
vec3 closest_point_plane(plane p, vec3 pt);
vec3 closest_point_line(line l, vec3 p);
vec3 closest_point_triangle(triangle t, vec3 p);
vec3 closest_point_sphere(sphere s, vec3 p);
vec3 closest_point_aabb(aabb box, vec3 p);
vec3 closest_point_obb(obb box, vec3 p);
flt closest_points_line_line(line a, line b, vec3* p1, vec3* p2);



/***************************************************************************************************
	Collision Detection */

sint is_point_on_ray(ray r, vec3 p);
sint is_point_on_line(line l, vec3 p);
sint is_point_on_plane(plane p, vec3 pt);
sint is_point_in_sphere(sphere s, vec3 p);
sint is_point_in_aabb(aabb box, vec3 p);
sint is_point_in_obb(obb box, vec3 p);

sint is_sphere_in_sphere(sphere a, sphere b);
sint is_capsule_in_capsule(capsule a, capsule b);
sint is_aabb_in_aabb(aabb a, aabb b);
sint is_obb_in_obb(obb a, obb b);

sint is_sphere_in_capsule(sphere s, capsule c);


