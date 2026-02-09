#include <math.h>
#include "math.h"



/*************************************************************************************************/

vec2 vec2_set(flt x, flt y)
{
    vec2 vec = { x, y };
    return vec;
}

vec2 vec2_add(vec2 a, vec2 b)
{
    vec2 tmp = { (a.x + b.x), (a.y + b.y) };
    return tmp;
}

vec2 vec2_sub(vec2 a, vec2 b)
{
    vec2 tmp = { (a.x - b.x), (a.y - b.y) };
    return tmp;
}

vec2 vec2_mul(vec2 a, flt v)
{
    vec2 tmp = { (a.x * v), (a.y * v) };
    return tmp;
}

vec2 vec2_div(vec2 a, flt v)
{
    vec2 tmp = { (a.x / v), (a.y / v) };
    return tmp;
}

flt vec2_dot(vec2 a, vec2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

flt vec2_len(vec2 a)
{
    return flt_sqrt(vec2_dot(a, a));
}

vec2 vec2_unit(vec2 a)
{
    return vec2_div(a, vec2_len(a));
}

vec2 vec2_lerp(vec2 from, vec2 to, flt alpha)
{
    return vec2_add(from, vec2_mul(vec2_sub(to, from), alpha));
}

s32 vec2_cmp(vec2 a, vec2 b)
{
    if (a.x != b.x) return a.x > b.x ? 1 : -1;
    if (a.y != b.y) return a.y > b.y ? 1 : -1;
    return 0;
}



/*************************************************************************************************/

vec3 vec3_set(flt x, flt y, flt z)
{
    vec3 vec = { x, y, z };
    return vec;
}

vec3 vec3_add(vec3 a, vec3 b)
{
    vec3 tmp = { (a.x + b.x), (a.y + b.y), (a.z + b.z) };
    return tmp;
}

vec3 vec3_sub(vec3 a, vec3 b)
{
    vec3 tmp = { (a.x - b.x), (a.y - b.y), (a.z - b.z) };
    return tmp;
}

vec3 vec3_mul(vec3 a, flt v)
{
    vec3 tmp = { (a.x * v), (a.y * v), (a.z * v) };
    return tmp;
}

vec3 vec3_div(vec3 a, flt v)
{
    vec3 tmp = { (a.x / v), (a.y / v), (a.z / v) };
    return tmp;
}

vec3 vec3_cross(vec3 a, vec3 b)
{
    vec3 tmp = {
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x),
    };
    return tmp;
}

flt vec3_dot(vec3 a, vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

flt vec3_len(vec3 a)
{
    return flt_sqrt(vec3_dot(a, a));
}

vec3 vec3_unit(vec3 a)
{
    flt len = vec3_len(a);
    return len > (flt)0 ? vec3_div(a, len) : a;
}

vec3 vec3_lerp(vec3 from, vec3 to, flt alpha)
{
    return vec3_add(from, vec3_mul(vec3_sub(to, from), alpha));
}

vec3 vec3_rotate(vec3 a, quat r)
{
    return vec3_transform(a, mat4_rotation(r));
}

vec3 vec3_transform(vec3 a, mat4 t)
{
    vec3 tmp = {
        a.x * t.m[0] + a.y * t.m[1] + a.z * t.m[2] + t.m[3],
        a.x * t.m[4] + a.y * t.m[5] + a.z * t.m[6] + t.m[7],
        a.x * t.m[8] + a.y * t.m[9] + a.z * t.m[10] + t.m[11]
    };
    return tmp;
}

s32 vec3_cmp(vec3 a, vec3 b)
{
    if (a.x != b.x) return a.x > b.x ? 1 : -1;
    if (a.y != b.y) return a.y > b.y ? 1 : -1;
    if (a.z != b.z) return a.z > b.z ? 1 : -1;
    return 0;
}



/*************************************************************************************************/

vec4 vec4_set(flt x, flt y, flt z, flt w)
{
    vec4 vec = { x, y, z, w };
    return vec;
}

vec4 vec4_add(vec4 a, vec4 b)
{
    vec4 tmp = { (a.x + b.x), (a.y + b.y), (a.z + b.z), (a.w + b.w) };
    return tmp;
}

vec4 vec4_sub(vec4 a, vec4 b)
{
    vec4 tmp = { (a.x - b.x), (a.y - b.y), (a.z - b.z), (a.w - b.w) };
    return tmp;
}

vec4 vec4_mul(vec4 a, flt v)
{
    vec4 tmp = { (a.x * v), (a.y * v), (a.z * v), (a.w * v) };
    return tmp;
}

vec4 vec4_div(vec4 a, flt v)
{
    vec4 tmp = { (a.x / v), (a.y / v), (a.z / v), (a.w / v) };
    return tmp;
}

flt vec4_dot(vec4 a, vec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

flt vec4_len(vec4 a)
{
    return flt_sqrt(vec4_dot(a, a));
}

vec4 vec4_unit(vec4 a)
{
    return vec4_div(a, vec4_len(a));
}

vec4 vec4_lerp(vec4 from, vec4 to, flt alpha)
{
    return vec4_add(from, vec4_mul(vec4_sub(to, from), alpha));
}

vec4 vec4_transform(vec4 a, mat4 t)
{
    vec4 tmp = {
        a.x * t.m[0] + a.y * t.m[1]
        + a.z * t.m[2] + a.w * t.m[3],
        a.x * t.m[4] + a.y * t.m[5]
        + a.z * t.m[6] + a.w * t.m[7],
        a.x * t.m[8] + a.y * t.m[9]
        + a.z * t.m[10] + a.w * t.m[11],
        a.x * t.m[12] + a.y * t.m[13]
        + a.z * t.m[14] + a.w * t.m[15]
    };
    return tmp;
}

s32 vec4_cmp(vec4 a, vec4 b)
{
    if (a.x != b.x) return a.x > b.x ? 1 : -1;
    if (a.y != b.y) return a.y > b.y ? 1 : -1;
    if (a.z != b.z) return a.z > b.z ? 1 : -1;
    if (a.w != b.w) return a.w > b.w ? 1 : -1;
    return 0;
}



/*************************************************************************************************/

quat quat_from_euler_angles(vec3 angles)
{
    flt cx, sx, cy, sy, cz, sz;
    cx = flt_cos(flt_dtor(angles.x * 0.5f)),
        sx = flt_sin(flt_dtor(angles.x * 0.5f)),
        cy = flt_cos(flt_dtor(angles.y * 0.5f)),
        sy = flt_sin(flt_dtor(angles.y * 0.5f)),
        cz = flt_cos(flt_dtor(angles.z * 0.5f)),
        sz = flt_sin(flt_dtor(angles.z * 0.5f));

    quat tmp = {
        cx * sy * sz - sx * cy * cz,
        -cx * sy * cz - sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    };
    return tmp;
}

quat quat_from_axis_angle(vec3 axis, flt angle)
{
    flt sin, cos;
    cos = flt_cos(flt_dtor(angle * 0.5f)),
        sin = flt_sin(flt_dtor(angle * 0.5f));
    axis = vec3_mul(axis, sin);
    quat tmp = { axis.x, axis.y, axis.z, cos };
    return tmp;
}

quat quat_add(quat a, quat b)
{
    quat tmp = { (a.x + b.x), (a.y + b.y), (a.z + b.z), (a.w + b.w) };
    return tmp;
}

quat quat_sub(quat a, quat b)
{
    quat tmp = { (a.x - b.x), (a.y - b.y), (a.z - b.z), (a.w - b.w) };
    return tmp;
}

quat quat_mul(quat a, flt v)
{
    quat tmp = { (a.x * v), (a.y * v), (a.z * v), (a.w * v) };
    return tmp;
}

quat quat_div(quat a, flt v)
{
    quat tmp = { (a.x / v), (a.y / v), (a.z / v), (a.w / v) };
    return tmp;
}

flt quat_dot(quat a, quat b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

flt quat_len(quat a)
{
    return flt_sqrt(quat_dot(a, a));
}

quat quat_unit(quat a)
{
    return quat_div(a, quat_len(a));
}

quat quat_conj(quat a)
{
    quat tmp = { -a.x, -a.y, -a.z, a.w };
    return tmp;
}

quat quat_inv(quat a)
{
    return quat_div(quat_conj(a), quat_dot(a, a));
}

quat quat_lerp(quat from, quat to, flt alpha)
{
    return quat_add(from, quat_mul(quat_sub(to, from), alpha));
}

quat quat_nlerp(quat from, quat to, flt alpha)
{
    return quat_unit(quat_lerp(from, to, alpha));
}

quat quat_slerp(quat from, quat to, flt alpha)
{
    flt dot, angle;
    dot = quat_dot(from, to);
    angle = flt_acos(dot);
    from = dot < 0 ? from : quat_mul(from, (flt)-1.0);
    from = quat_mul(from, flt_sin(((flt)1.0 - alpha) * angle));
    from = quat_add(from, quat_mul(to, flt_sin(alpha * angle)));
    return quat_div(from, flt_sin(angle));
}

quat quat_slerpl(quat from, quat to, flt alpha)
{
    flt dot, angle;
    dot = quat_dot(from, to);
    angle = flt_acos(dot);
    from = dot < 0 ? quat_mul(from, (flt)-1.0) : from;
    from = quat_mul(from, flt_sin(((flt)1.0 - alpha) * angle));
    from = quat_add(from, quat_mul(to, flt_sin(alpha * angle)));
    return quat_div(from, flt_sin(angle));
}

quat quat_rotate(quat a, quat b)
{
    quat tmp = {
        (a.x * b.w) + (a.w * b.x) + (a.y * b.z) - (a.z * b.y),
        (a.y * b.w) + (a.w * b.y) + (a.z * b.x) - (a.x * b.z),
        (a.z * b.w) + (a.w * b.z) + (a.x * b.y) - (a.y * b.x),
        (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
    };
    return tmp;
}



/*************************************************************************************************/

mat3 mat3_rotation(quat r)
{
    mat3 tmp = {
        (flt)1.0 - ((flt)2.0 * r.y * r.y) - ((flt)2.0 * r.z * r.z),
        ((flt)2.0 * r.x * r.y) - ((flt)2.0 * r.w * r.z),
        ((flt)2.0 * r.x * r.z) + ((flt)2.0 * r.w * r.y),

        ((flt)2.0 * r.x * r.y) + ((flt)2.0 * r.w * r.z),
        (flt)1.0 - ((flt)2.0 * r.x * r.x) - ((flt)2.0 * r.z * r.z),
        ((flt)2.0 * r.y * r.z) - ((flt)2.0 * r.w * r.x),

        ((flt)2.0 * r.x * r.z) - ((flt)2.0 * r.w * r.y),
        ((flt)2.0 * r.y * r.z) + ((flt)2.0 * r.w * r.x),
        (flt)1.0 - ((flt)2.0 * r.x * r.x) - ((flt)2.0 * r.y * r.y)
    };
    return tmp;
}

mat3 mat3_scale(vec3 s)
{
    mat3 tmp = {
        s.x, 0.0f, 0.0f,
        0.0f, s.y, 0.0f,
        0.0f, 0.0f, s.z
    };
    return tmp;
}

mat3 mat3_from_mat4(mat4 m)
{
    mat3 tmp = {
        m.m[0], m.m[1], m.m[2],
        m.m[4], m.m[5], m.m[6],
        m.m[8], m.m[9], m.m[10]
    };
    return tmp;
}

mat3 mat3_add(mat3 a, mat3 b)
{
    mat3 tmp = {
        a.m[0] + b.m[0], a.m[1] + b.m[1], a.m[2] + b.m[2],
        a.m[3] + b.m[3], a.m[4] + b.m[4], a.m[5] + b.m[5],
        a.m[6] + b.m[6], a.m[7] + b.m[7], a.m[8] + b.m[8]
    };
    return tmp;
}

mat3 mat3_sub(mat3 a, mat3 b)
{
    mat3 tmp = {
        a.m[0] - b.m[0], a.m[1] - b.m[1], a.m[2] - b.m[2],
        a.m[3] - b.m[3], a.m[4] - b.m[4], a.m[5] - b.m[5],
        a.m[6] - b.m[6], a.m[7] - b.m[7], a.m[8] - b.m[8]
    };
    return tmp;
}

mat3 mat3_mul(mat3 a, mat3 b)
{
    mat3 tmp = {
        (a.m[0] * b.m[0]) + (a.m[1] * b.m[3]) + (a.m[2] * b.m[6]),
        (a.m[0] * b.m[1]) + (a.m[1] * b.m[4]) + (a.m[2] * b.m[7]),
        (a.m[0] * b.m[2]) + (a.m[1] * b.m[5]) + (a.m[2] * b.m[8]),

        (a.m[3] * b.m[0]) + (a.m[4] * b.m[3]) + (a.m[5] * b.m[6]),
        (a.m[3] * b.m[1]) + (a.m[4] * b.m[4]) + (a.m[5] * b.m[7]),
        (a.m[3] * b.m[2]) + (a.m[4] * b.m[5]) + (a.m[5] * b.m[8]),

        (a.m[6] * b.m[0]) + (a.m[7] * b.m[3]) + (a.m[8] * b.m[6]),
        (a.m[6] * b.m[1]) + (a.m[7] * b.m[4]) + (a.m[8] * b.m[7]),
        (a.m[6] * b.m[2]) + (a.m[7] * b.m[5]) + (a.m[8] * b.m[8]),
    };
    return tmp;
}

mat3 mat3_transpose(mat3 a)
{
    mat3 tmp = {
        a.m[0], a.m[3], a.m[6],
        a.m[1], a.m[4], a.m[7],
        a.m[2], a.m[5], a.m[8]
    };
    return tmp;
}



/*************************************************************************************************/

mat4 mat4_translation(vec3 t)
{
    mat4 tmp = {
        1.0f, 0.0f, 0.0f, t.x,
        0.0f, 1.0f, 0.0f, t.y,
        0.0f, 0.0f, 1.0f, t.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return tmp;
}

mat4 mat4_rotation(quat r)
{
    mat4 tmp = {
        (flt)1.0 - ((flt)2.0 * r.y * r.y) - ((flt)2.0 * r.z * r.z),
        ((flt)2.0 * r.x * r.y) - ((flt)2.0 * r.w * r.z),
        ((flt)2.0 * r.x * r.z) + ((flt)2.0 * r.w * r.y),
        (flt)0.0,

        ((flt)2.0 * r.x * r.y) + ((flt)2.0 * r.w * r.z),
        (flt)1.0 - ((flt)2.0 * r.x * r.x) - ((flt)2.0 * r.z * r.z),
        ((flt)2.0 * r.y * r.z) - ((flt)2.0 * r.w * r.x),
        (flt)0.0,

        ((flt)2.0 * r.x * r.z) - ((flt)2.0 * r.w * r.y),
        ((flt)2.0 * r.y * r.z) + ((flt)2.0 * r.w * r.x),
        (flt)1.0 - ((flt)2.0 * r.x * r.x) - ((flt)2.0 * r.y * r.y),
        (flt)0.0,

        (flt)0.0,
        (flt)0.0,
        (flt)0.0,
        (flt)1.0
    };
    return tmp;
}

mat4 mat4_scale(vec3 s)
{
    mat4 tmp = {
        s.x, 0.0f, 0.0f, 0.0f,
        0.0f, s.y, 0.0f, 0.0f,
        0.0f, 0.0f, s.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return tmp;
}

mat4 mat4_model(transform transform)
{
    return mat4_mul(mat4_mul(
        mat4_translation(transform.location),
        mat4_rotation(transform.rotation)),
        mat4_scale(transform.scale));
}

mat4 mat4_view(vec3 location, vec3 rotation)
{
    return mat4_mul(
        mat4_rotation(quat_inv(quat_from_euler_angles(rotation))),
        mat4_translation(vec3_mul(location, (flt)-1)));
}

mat4 mat4_perspective_vk(flt fov, flt ratio, flt near, flt far)
{
    flt fov_div_two_tan, a, b, c, d;
    fov_div_two_tan = flt_tan(flt_dtor(fov / 2.0f));
    a = 1.0f / (ratio * fov_div_two_tan);
    b = 1.0f / fov_div_two_tan;
    c, d;

    if (far > flt_abs(near))
    {
        if (near > 0.0f)
        {
            c = far / (far - near);
            d = (-near * far) / (far - near);
        }
        else
        {
            /* Reverse z-plane. */
            c = near / (far + near);
            d = (-near * far) / (far + near);
        }
    }
    else
    {
        /* Infinite far plane. */
        if (near > 0.0f)
        {
            c = 1.0f;
            d = -near;
        }
        else
        {
            /* Reverse z-plane. */
            c = 0.0f;
            d = -near;
        }
    }

    mat4 tmp = {
           a, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f,    -b, 0.0f,
        0.0f,    c, 0.0f,    d,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    return tmp;
}

mat4 mat4_perspective_gl(flt fov, flt ratio, flt near, flt far)
{
    flt fov_div_two_tan, a, b, c, d;
    fov_div_two_tan = flt_tan(flt_dtor(fov / 2.0f));
    a = 1.0f / (ratio * fov_div_two_tan);
    b = 1.0f / fov_div_two_tan;
    c, d;

    if (far > flt_abs(near))
    {
        if (near > 0.0f)
        {
            c = (far + near) / (far - near);
            d = (-2.0f * near * far) / (far - near);
        }
        else
        {
            /* Reverse z-plane. */
            c = -(far - near) / (far + near);
            d = (2.0f * -near * far) / (far + near);
        }
    }
    else
    {
        /* Infinite far plane. */
        if (near > 0.0f)
        {
            c = 1.0f;
            d = -2.0f * near;
        }
        else
        {
            /* Reverse z-plane. */
            c = 0.0f;
            d = -near;
        }
    }

    mat4 tmp = {
           a, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f,    b, 0.0f,
        0.0f,    c, 0.0f,    d,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    return tmp;
}

mat4 _mat4_perspective_gu(flt fov, flt ratio, flt near, flt far)
{
    flt fov_div_two_tan, a, b, c, d;
    fov_div_two_tan = flt_tan(flt_dtor(fov / 2.0f));
    a = 1.0f / (ratio * fov_div_two_tan);
    b = 1.0f / fov_div_two_tan;
    c, d;

    if (far > flt_abs(near))
    {
        if (near > 0.0f)
        {
            c = near / (far - near);
            d = (-near * far) / (far - near);
        }
        else
        {
            /* Reverse z-plane. */
            c = -far / (far + near);
            d = (-near * far) / (far + near);
        }
    }
    else
    {
        /* Infinite far plane. */
        if (near > 0.0f)
        {
            c = 0.0f;
            d = -near;
        }
        else
        {
            /* Reverse z-plane. */
            c = -1.0f;
            d = -near;
        }
    }

    mat4 tmp = {
           a, 0.0f, 0.0f, 0.0f,
        0.0f,    b, 0.0f, 0.0f,
        0.0f, 0.0f,    c,    d,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    return tmp;
}

mat4 mat4_perspective_gu(flt fov, flt ratio, flt near, flt far)
{
    flt fov_div_two_tan, a, b, c, d;
    fov_div_two_tan = flt_tan(flt_dtor(fov / 2.0f));
    a = 1.0f / (ratio * fov_div_two_tan);
    b = 1.0f / fov_div_two_tan;
    c, d;

    if (far > flt_abs(near))
    {
        if (near > 0.0f)
        {
            c = (far + near) / (far - near);
            d = (-2.0f * near * far) / (far - near);
        }
        else
        {
            /* Reverse z-plane. */
            c = -far / (far + near);
            d = (-near * far) / (far + near);
        }
    }
    else
    {
        /* Infinite far plane. */
        if (near > 0.0f)
        {
            c = 0.0f;
            d = -near;
        }
        else
        {
            /* Reverse z-plane. */
            c = -1.0f;
            d = -near;
        }
    }

    mat4 tmp = {
           a, 0.0f, 0.0f, 0.0f,
        0.0f,    b, 0.0f, 0.0f,
        0.0f, 0.0f,    c,    d,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    return tmp;
}

mat4 mat4_mvp(mat4 model, mat4 view, mat4 projection)
{
    return mat4_mul(projection, mat4_mul(view, model));
}

mat4 mat4_from_mat3(mat3 m)
{
    mat4 tmp = {
        m.m[0], m.m[1], m.m[2], (flt)0.0,
        m.m[3], m.m[4], m.m[5], (flt)0.0,
        m.m[6], m.m[7], m.m[8], (flt)0.0,
        (flt)0.0, (flt)0.0, (flt)0.0, (flt)1.0
    };

    return tmp;
}

mat4 mat4_add(mat4 a, mat4 b)
{
    mat4 tmp = {
        a.m[0] + b.m[0], a.m[1] + b.m[1], a.m[2] + b.m[2], a.m[3] + b.m[3],
        a.m[4] + b.m[4], a.m[5] + b.m[5], a.m[6] + b.m[6], a.m[7] + b.m[7],
        a.m[8] + b.m[8], a.m[9] + b.m[9], a.m[10] + b.m[10], a.m[11] + b.m[11],
        a.m[12] + b.m[12], a.m[13] + b.m[13], a.m[14] + b.m[14], a.m[15] + b.m[15]
    };
    return tmp;
}

mat4 mat4_sub(mat4 a, mat4 b)
{
    mat4 tmp = {
        a.m[0] - b.m[0], a.m[1] - b.m[1], a.m[2] - b.m[2], a.m[3] - b.m[3],
        a.m[4] - b.m[4], a.m[5] - b.m[5], a.m[6] - b.m[6], a.m[7] - b.m[7],
        a.m[8] - b.m[8], a.m[9] - b.m[9], a.m[10] - b.m[10], a.m[11] - b.m[11],
        a.m[12] - b.m[12], a.m[13] - b.m[13], a.m[14] - b.m[14], a.m[15] - b.m[15]
    };
    return tmp;
}

mat4 mat4_mul(mat4 a, mat4 b)
{
    mat4 tmp = {
        a.m[0] * b.m[0] + a.m[1] * b.m[4] + a.m[2] * b.m[8] + a.m[3] * b.m[12],
        a.m[0] * b.m[1] + a.m[1] * b.m[5] + a.m[2] * b.m[9] + a.m[3] * b.m[13],
        a.m[0] * b.m[2] + a.m[1] * b.m[6] + a.m[2] * b.m[10] + a.m[3] * b.m[14],
        a.m[0] * b.m[3] + a.m[1] * b.m[7] + a.m[2] * b.m[11] + a.m[3] * b.m[15],

        a.m[4] * b.m[0] + a.m[5] * b.m[4] + a.m[6] * b.m[8] + a.m[7] * b.m[12],
        a.m[4] * b.m[1] + a.m[5] * b.m[5] + a.m[6] * b.m[9] + a.m[7] * b.m[13],
        a.m[4] * b.m[2] + a.m[5] * b.m[6] + a.m[6] * b.m[10] + a.m[7] * b.m[14],
        a.m[4] * b.m[3] + a.m[5] * b.m[7] + a.m[6] * b.m[11] + a.m[7] * b.m[15],

        a.m[8] * b.m[0] + a.m[9] * b.m[4] + a.m[10] * b.m[8] + a.m[11] * b.m[12],
        a.m[8] * b.m[1] + a.m[9] * b.m[5] + a.m[10] * b.m[9] + a.m[11] * b.m[13],
        a.m[8] * b.m[2] + a.m[9] * b.m[6] + a.m[10] * b.m[10] + a.m[11] * b.m[14],
        a.m[8] * b.m[3] + a.m[9] * b.m[7] + a.m[10] * b.m[11] + a.m[11] * b.m[15],

        a.m[12] * b.m[0] + a.m[13] * b.m[4] + a.m[14] * b.m[8] + a.m[15] * b.m[12],
        a.m[12] * b.m[1] + a.m[13] * b.m[5] + a.m[14] * b.m[9] + a.m[15] * b.m[13],
        a.m[12] * b.m[2] + a.m[13] * b.m[6] + a.m[14] * b.m[10] + a.m[15] * b.m[14],
        a.m[12] * b.m[3] + a.m[13] * b.m[7] + a.m[14] * b.m[11] + a.m[15] * b.m[15],
    };
    return tmp;
}

mat4 mat4_transpose(mat4 a)
{
    mat4 tmp = {
        a.m[0], a.m[4], a.m[8], a.m[12],
        a.m[1], a.m[5], a.m[9], a.m[13],
        a.m[2], a.m[6], a.m[10], a.m[14],
        a.m[3], a.m[7], a.m[11], a.m[15]
    };
    return tmp;
}



/*************************************************************************************************/

#if defined(FLT_64)

flt flt_abs(flt v)
{
    return fabs(v);
}

flt flt_min(flt a, flt b)
{
    return fmin(a, b);
}

flt flt_max(flt a, flt b)
{
    return fmax(a, b);
}

flt flt_trunc(flt v)
{
    return trunc(v);
}

flt flt_round(flt v)
{
    return round(v);
}

flt flt_mod(flt v, flt mod)
{
    return fmod(v, mod);
}

flt flt_clamp(flt v, flt min, flt max)
{
    return fmax(min, fminf(max, v));
}

sint flt_isclose(flt a, flt b, flt epsilon)
{
    if (a == b)
        return 1;

    if (isinf(a) || isinf(b))
        return 0;

    return flt_abs(a - b) <= flt_max(epsilon * flt_max(a, b), 0.000000000001);
}

flt flt_sin(flt v)
{
    return sin(v);
}

flt flt_asin(flt v)
{
    return asin(v);
}

flt flt_cos(flt v)
{
    return cos(v);
}

flt flt_acos(flt v)
{
    return acos(v);
}

flt flt_tan(flt v)
{
    return tan(v);
}

flt flt_atan(flt v)
{
    return atan(v);
}

flt flt_pow(flt v, flt exp)
{
    return pow(v, exp);
}

flt flt_log2(flt v)
{
    return log2(v);
}

flt flt_sqrt(flt v)
{
    return sqrt(v);
}

#else

flt flt_abs(flt v)
{
    return fabsf(v);
}

flt flt_min(flt a, flt b)
{
    return fminf(a, b);
}

flt flt_max(flt a, flt b)
{
    return fmaxf(a, b);
}

flt flt_trunc(flt v)
{
    return truncf(v);
}

flt flt_round(flt v)
{
    return roundf(v);
}

flt flt_mod(flt v, flt mod)
{
    return fmodf(v, mod);
}

flt flt_clamp(flt v, flt min, flt max)
{
    return fmaxf(min, fminf(max, v));
}

sint flt_isclose(flt a, flt b, flt epsilon)
{
    if (a == b)
        return 1;

    if (isinf(a) || isinf(b))
        return 0;

    return flt_abs(a - b) <= flt_max(epsilon * flt_max(a, b), 0.0000001f);
}

flt flt_sin(flt v)
{
    return sinf(v);
}

flt flt_asin(flt v)
{
    return asinf(v);
}

flt flt_cos(flt v)
{
    return cosf(v);
}

flt flt_acos(flt v)
{
    return acosf(v);
}

flt flt_tan(flt v)
{
    return tanf(v);
}

flt flt_atan(flt v)
{
    return atanf(v);
}

flt flt_pow(flt v, flt exp)
{
    return powf(v, exp);
}

flt flt_log2(flt v)
{
    return log2f(v);
}

flt flt_sqrt(flt v)
{
    return sqrtf(v);
}

#endif



/**************************************************************************************************/

ray ray_set(vec3 origin, vec3 direction)
{
    ray r = { origin, direction };
    return r;
}



/**************************************************************************************************/

plane plane_from_points(vec3 a, vec3 b, vec3 c)
{
    plane p;
    b = vec3_sub(b, a);
    c = vec3_sub(c, a);
    p.normal = vec3_unit(vec3_cross(b, c));
    p.distance = vec3_dot(p.normal, a);
    return p;
}



/**************************************************************************************************/

line line_set(vec3 start, vec3 end)
{
    line l = { start, end };
    return l;
}



/**************************************************************************************************/

triangle triangle_from_points(vec3 a, vec3 b, vec3 c)
{
    triangle t = {a, b, c};
    return t;
}



/**************************************************************************************************/


sphere sphere_set(vec3 origin, flt radius)
{
    sphere s = { origin, radius };
    return s;
}

/*  Ritter sphere algorithm  */
sphere sphere_from_points(const vec3* points, sint num)
{
    sphere s;
    sint i, min, max;

    most_distant_points_on_aabb(points, num, &min, &max);

    s.origin = vec3_mul(vec3_add(points[min], points[max]), (flt)0.5);
    s.radius = vec3_len(vec3_sub(points[max], s.origin));

    for (i = 0; i < num; i++)
        s = sphere_add_point(s, points[i]);

    return s;
}

sphere sphere_add_point(sphere s, vec3 p)
{
    vec3 offset = vec3_sub(p, s.origin);
    flt squared_distance = vec3_dot(offset, offset);
    flt distance, new_radius;

    if (squared_distance <= s.radius * s.radius)
        return s;

    distance = flt_sqrt(squared_distance);
    new_radius = (s.radius + distance) * (flt)0.5;
    offset = vec3_mul(offset, (new_radius - s.radius) / distance);

    s.radius = new_radius;
    s.origin = vec3_add(s.origin, offset);

    return s;
}



/**************************************************************************************************/

capsule capsule_set(vec3 start, vec3 end, flt radius)
{
    capsule c = { start, end, radius };
    return c;
}



/**************************************************************************************************/

aabb aabb_set(vec3 min, vec3 max)
{
    aabb box = { min, max };
    return box;
}

aabb aabb_from_extends(vec3 center, vec3 extends)
{
    aabb box;
    box.min = vec3_sub(center, extends);
    box.max = vec3_add(center, extends);
    return box;
}

aabb aabb_from_sphere(vec3 origin, flt radius)
{
    vec3 min = { origin.x - radius, origin.y - radius, origin.z - radius };
    vec3 max = { origin.x + radius, origin.y + radius, origin.z + radius };
    aabb box = { min, max };
    return box;
}

aabb aabb_from_points(const vec3* points, sint num)
{
    vec3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
    vec3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    aabb box;
    sint i;

    for (i = 0; i < num; i++)
    {
        min.x = flt_min(min.x, points[i].x);
        min.y = flt_min(min.y, points[i].y);
        min.z = flt_min(min.z, points[i].z);

        max.x = flt_max(max.x, points[i].x);
        max.y = flt_max(max.y, points[i].y);
        max.z = flt_max(max.z, points[i].z);
    }

    box.min = min;
    box.max = max;
    return box;
}

aabb aabb_transform(aabb box, mat4 transform)
{
    aabb b;
    flt* amin = (flt*)&box.min, *amax = (flt*)&box.max;
    flt* bmin = (flt*)&b.min, *bmax = (flt*)&b.max;
    flt translation[3] = { transform.m[3], transform.m[7], transform.m[11] };
    sint i, j;

    for (i = 0; i < 3; i++)
    {
        bmin[i] = bmax[i] = translation[i];

        for (j = 0; j < 3; j++)
        {
            flt x = transform.m[i * 4 + j] * amin[j];
            flt y = transform.m[i * 4 + j] * amax[j];

            if (x < y)
            {
                bmin[i] += x;
                bmax[i] += y;
            }
            else
            {
                bmin[i] += y;
                bmax[i] += x;
            }
        }
    }

    return b;
}

aabb aabb_union(aabb a, aabb b)
{
    a.min.x = flt_min(a.min.x, b.min.x);
    a.min.y = flt_min(a.min.y, b.min.y);
    a.min.z = flt_min(a.min.z, b.min.z);

    a.max.x = flt_max(a.max.x, b.max.x);
    a.max.y = flt_max(a.max.y, b.max.y);
    a.max.z = flt_max(a.max.z, b.max.z);

    return a;
}

flt aabb_area(aabb a)
{
    return (a.max.x - a.min.x) * (a.max.y - a.min.y) * (a.max.z - a.min.z);
}

void most_distant_points_on_aabb(const vec3* points, sint num, sint* min, sint* max)
{
    sint i, minx = 0, miny = 0, minz = 0, maxx = 0, maxy = 0, maxz = 0;
    flt squared_distance_x, squared_distance_y, squared_distance_z;
    vec3 offset;

    for (i = 0; i < num; i++)
    {
        if (points[i].x < points[minx].x) minx = i;
        if (points[i].x > points[maxx].x) maxx = i;
        if (points[i].y < points[miny].y) miny = i;
        if (points[i].y > points[maxy].y) maxy = i;
        if (points[i].z < points[minz].x) minz = i;
        if (points[i].z > points[maxz].x) maxz = i;
    }

    offset = vec3_sub(points[maxx], points[minx]);
    squared_distance_x = vec3_dot(offset, offset);
    offset = vec3_sub(points[maxy], points[miny]);
    squared_distance_y = vec3_dot(offset, offset);
    offset = vec3_sub(points[maxz], points[minz]);
    squared_distance_z = vec3_dot(offset, offset);

    if (squared_distance_x > squared_distance_y && squared_distance_x > squared_distance_z)
    {
        *min = minx;
        *max = maxx;
    }
    else if (squared_distance_y > squared_distance_z)
    {
        *min = miny;
        *max = maxy;
    }
    else
    {
        *min = minz;
        *max = maxz;
    }
}



/**************************************************************************************************/

obb obb_set(vec3 origin, vec3 axis[3], vec3 extents)
{
    obb box = { origin, {axis[0], axis[1], axis[2]}, extents };
    return box;
}



/***************************************************************************************************
    Closest Point */

flt distance_point_plane(plane p, vec3 pt)
{
    return vec3_dot(p.normal, pt) - p.distance;
}

flt distance2_point_line(line l, vec3 p)
{
    vec3 ab = vec3_sub(l.end, l.start);
    vec3 ac = vec3_sub(l.start, p);
    vec3 bc = vec3_sub(l.end, p);

    flt dot = vec3_dot(ac, ab);
    flt line_length_squared;

    if (dot <= 0.0f)
        return vec3_dot(ac, ac);

    line_length_squared = vec3_dot(ab, ab);

    if (dot >= line_length_squared)
        return vec3_dot(bc, bc);

    return vec3_dot(ac, ac) - dot * dot / line_length_squared;
}

flt distance2_point_aabb(aabb box, vec3 p)
{
    p.x = flt_max((flt)0.0, flt_max(box.min.x - p.x, p.x - box.max.x));
    p.y = flt_max((flt)0.0, flt_max(box.min.y - p.y, p.y - box.max.y));
    p.z = flt_max((flt)0.0, flt_max(box.min.z - p.z, p.z - box.max.z));
    return vec3_dot(p, p);
}

flt distance2_point_obb(obb box, vec3 p)
{
    vec3 to_point = vec3_sub(p, box.origin);
    vec3 excess;

    excess.x = flt_max((flt)0.0, flt_abs(vec3_dot(to_point, box.axes[0])) - box.extents.x);
    excess.y = flt_max((flt)0.0, flt_abs(vec3_dot(to_point, box.axes[1])) - box.extents.y);
    excess.z = flt_max((flt)0.0, flt_abs(vec3_dot(to_point, box.axes[2])) - box.extents.z);

    return vec3_dot(excess, excess);
}

vec3 closest_point_line(line l, vec3 p)
{
    vec3 ab = vec3_sub(l.end, l.start);
    vec3 ac = vec3_sub(p, l.start);
    flt t = vec3_dot(ac, ab) / vec3_dot(ab, ab);

    t = flt_clamp(t, 0.0f, 1.0f);
    return vec3_add(l.start, vec3_mul(ab, t));
}

vec3 closest_point_triangle(triangle t, vec3 p)
{
    flt va, vb, vc, v, w, denom;
    flt d1, d2, d3, d4, d5, d6;
    vec3 ab, ac, bc, ap, bp, cp;
    
    ab = vec3_sub(t.b, t.a);
    ac = vec3_sub(t.c, t.a);

    ap = vec3_sub(p, t.a);
    d1 = vec3_dot(ab, ap);
    d2 = vec3_dot(ac, ap);

    if (d1 <= 0.0f && d2 <= 0.0f)
        return t.a;

    bp = vec3_sub(p, t.b);
    d3 = vec3_dot(ab, bp);
    d4 = vec3_dot(ac, bp);

    if (d3 >= (flt)0 && d4 <= d3)
        return t.b;

    vc = d1 * d4 - d3 * d2;
    if (vc <= (flt)0 && d1 >= (flt)0 && d3 <= (flt)0)
    {
        v = d1 / (d1 - d3);
        return vec3_add(t.a, vec3_mul(ab, v));
    }

    cp = vec3_sub(p, t.c);
    d5 = vec3_dot(ab, cp);
    d6 = vec3_dot(ac, cp);
    if (d6 >= (flt)0 && d5 <= d6)
        return t.c;

    vb = d5 * d2 - d1 * d6;
    if (vb <= (flt)0 && d2 >= (flt)0 && d6 <= (flt)0)
    {
        w = d2 / (d2 - d6);
        return vec3_add(t.a, vec3_mul(ac, w));
    }

    v = d4 - d3;
    w = d5 - d6;

    va = d3 * d6 - d5 * d4;
    if (va <= (flt)0 && v >= (flt)0 && w >= (flt)0)
    {
        w = v / (v + w);
        bc = vec3_sub(t.c, t.b);
        return vec3_add(t.b, vec3_mul(bc, w));
    }

    denom = (flt)1.0f / (va + vb + vc);
    ab = vec3_mul(ab, vb * denom);
    ac = vec3_mul(ac, vc * denom);
    return vec3_add(t.a, vec3_add(ab, ac));
}

vec3 closest_point_ray(ray r, vec3 p)
{
    vec3 ab = r.direction;
    vec3 ac = vec3_sub(p, r.origin);
    flt t = vec3_dot(ac, ab) / vec3_dot(ab, ab);

    t = flt_max(t, 0.0f);

    return vec3_add(r.origin, vec3_mul(ab, t));
}

vec3 closest_point_plane(plane p, vec3 pt)
{
    flt distance = vec3_dot(p.normal, pt) - p.distance;
    vec3 offset = vec3_mul(p.normal, distance);
    return vec3_sub(pt, offset);
}

vec3 closest_point_sphere(sphere s, vec3 p)
{
    vec3 to_point = vec3_sub(s.origin, p);
    to_point = vec3_unit(to_point);
    to_point = vec3_mul(to_point, s.radius);
    return vec3_add(s.origin, to_point);
}

vec3 closest_point_aabb(aabb box, vec3 p)
{
    p.x = flt_clamp(p.x, box.min.x, box.max.x);
    p.y = flt_clamp(p.y, box.min.y, box.max.y);
    p.z = flt_clamp(p.z, box.min.z, box.max.z);
    return p;
}

vec3 closest_point_obb(obb box, vec3 p)
{
    vec3 to_point = vec3_sub(p, box.origin);
    vec3 point = box.origin;

    point.x += flt_clamp(vec3_dot(to_point, box.axes[0]), -box.extents.x, box.extents.x);
    point.y += flt_clamp(vec3_dot(to_point, box.axes[1]), -box.extents.y, box.extents.y);
    point.z += flt_clamp(vec3_dot(to_point, box.axes[2]), -box.extents.z, box.extents.z);

    return point;
}

flt closest_points_line_line(line a, line b, vec3* p1, vec3* p2)
{
    vec3 va = vec3_sub(a.end, a.start);
    vec3 vb = vec3_sub(b.end, b.start);
    vec3 ab = vec3_sub(a.start, b.start);
    flt alen2 = vec3_dot(va, va);
    flt blen2 = vec3_dot(vb, vb);
    flt dc, da, db = vec3_dot(vb, ab);
    flt denom, s, t;

    /*  if both lines have a length of 0, return the start points  */
    if (alen2 <= FLT_EPSILON && blen2 <= FLT_EPSILON)
    {
        s = t = (flt)0.0;
        *p1 = a.start;
        *p2 = b.start;
        return vec3_dot(ab, ab);
    }

    /* if only line a is a point */
    if (alen2 <= FLT_EPSILON)
    {
        s = (flt)0.0;
        t = flt_clamp(db / blen2, (flt)0.0, (flt)1.0);
    }
    else
    {
        da = vec3_dot(va, ab);
        if (blen2 <= FLT_EPSILON)
        {
            t = (flt)0.0;
            s = flt_clamp(-da / alen2, (flt)0.0, (flt)0.0);
        }
        else
        {
            dc = vec3_dot(va, vb);
            denom = alen2 * blen2 - dc * dc;

            if (denom != (flt)0.0)
                s = flt_clamp((dc * db - da * blen2) / denom, (flt)0.0, (flt)1.0);
            else
                s = (flt)0.0;
            
            t = dc * s + db;

            if (t < (flt)0.0)
            {
                t = (flt)0.0;
                s = flt_clamp(-da / alen2, (flt)0.0, (flt)1.0);
            }
            else if (t > blen2)
            {
                t = (flt)1.0;
                s = flt_clamp((dc - da) / alen2, (flt)0.0, (flt)1.0);
            }
            else
            {
                t /= blen2;
            }
        }
    }

    *p1 = vec3_add(a.start, vec3_mul(va, s));
    *p2 = vec3_add(b.start, vec3_mul(vb, t));
    ab = vec3_sub(*p1, *p2);
    return vec3_dot(ab, ab);
}



/***************************************************************************************************
    Collision Detection */

sint is_point_on_line(line l, vec3 p)
{
    vec3 ab = vec3_sub(l.end, l.start);
    vec3 ac = vec3_sub(l.start, p);
    vec3 bc = vec3_sub(l.end, p);

    flt sqlen_ab = vec3_dot(ab, ab);
    flt sqlen_ac = vec3_dot(ac, ac);
    flt sqlen_bc = vec3_dot(bc, bc);

    return flt_isclose(sqlen_ab, sqlen_ac + sqlen_bc, FLT_EPSILON);
}

sint is_point_on_ray(ray r, vec3 p)
{
    vec3 to_point = vec3_unit(vec3_sub(p, r.origin));
    flt dot = vec3_dot(to_point, r.direction);
    return flt_isclose(dot, 1.0f, FLT_EPSILON);
}

sint is_point_on_plane(plane p, vec3 pt)
{
    return flt_isclose(distance_point_plane(p, pt), 0.0f, FLT_EPSILON);
}

sint is_point_in_sphere(sphere s, vec3 p)
{
    vec3 to_point = vec3_sub(s.origin, p);
    flt squared_distance = vec3_dot(to_point, to_point);

    return squared_distance <= s.radius * s.radius;
}

sint is_point_in_aabb(aabb box, vec3 p)
{
    return (p.x > box.min.x && p.y > box.min.y && p.z > box.min.z) &&
        (p.x < box.max.x && p.y < box.max.y && p.z < box.max.z);
}

sint is_point_in_obb(obb box, vec3 p)
{
    vec3 to_point = vec3_sub(p, box.origin);

    return (box.extents.x >= vec3_dot(to_point, box.axes[0]))
        && (box.extents.y >= vec3_dot(to_point, box.axes[1]))
        && (box.extents.z >= vec3_dot(to_point, box.axes[2]));
}

sint is_sphere_in_capsule(sphere s, capsule c)
{
    flt squared_distance = distance2_point_line(c.line, s.origin);
    flt radius = s.radius + c.radius;
    return squared_distance <= radius * radius;
}

sint is_sphere_in_sphere(sphere a, sphere b)
{
    vec3 offset = vec3_sub(a.origin, b.origin);
    flt squared_distance = vec3_dot(offset, offset);
    flt radius = a.radius + b.radius;

    return radius * radius >= squared_distance;
}

sint is_capsule_in_capsule(capsule a, capsule b)
{
    vec3 p1, p2;
    flt dist2 = closest_points_line_line(a.line, b.line, &p1, &p2);
    flt radius = a.radius + b.radius;
    return dist2 <= radius * radius;
}

sint is_aabb_in_aabb(aabb a, aabb b)
{
    return (a.min.x > b.max.x || a.max.x < b.min.x)
        || (a.min.y > b.max.y || a.max.y < b.min.y)
        || (a.min.z > b.max.z || a.max.z < b.min.z);
}

sint is_obb_in_obb(obb a, obb b)
{
    const sint crossidx[] = { 1, 2, 0, 1 };
    sint i, j, x, y, z, w;

    flt ra, rb;
    flt rotation[3][3];
    flt absrot[3][3];
    flt t[3];

    flt* ea = (flt*)&a.extents;
    flt* eb = (flt*)&b.extents;
    vec3 ab = vec3_sub(b.origin, a.origin);

    for (i = 0; i < 3; i++)
    {
        ra = ea[i];
        rb = (flt)0.0;

        for (j = 0; j < 3; j++)
        {
            rotation[i][j] = vec3_dot(a.axes[i], b.axes[j]);
            absrot[i][j] = flt_abs(rotation[i][j]) + FLT_EPSILON;
            rb += eb[j] * absrot[i][j];
        }

        t[i] = vec3_dot(ab, a.axes[i]);
        if (flt_abs(t[i]) > ra + rb)
            return 0;
    }

    for (i = 0; i < 3; i++)
    {
        ra = ea[0] * absrot[0][i] + ea[1] * absrot[1][i] + ea[2] * absrot[2][i];
        rb = eb[i];

        if (flt_abs(t[0] * rotation[0][i] + t[1] * rotation[1][i] + t[2] * rotation[2][i]) > ra + rb)
            return 0;
    }

    for (i = 0; i < 3; i++)
    {
        x = crossidx[i]; y = crossidx[i + 1];
        for (j = 0; j < 3; j++)
        {
            z = crossidx[j]; w = crossidx[j + 1];
            ra = ea[x] * absrot[y][j] + ea[y] * absrot[x][j];
            rb = eb[z] * absrot[i][w] + eb[w] * absrot[i][z];

            if (flt_abs(t[y] * rotation[x][j] - t[x] * rotation[y][j]) > ra + rb)
                return 0;
        }
    }

    return 1;
}


