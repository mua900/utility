#ifndef _LINEAR_MATH
#define _LINEAR_MATH

#include <math.h>
#include <stdio.h>

#define INDEX(r, c) (r * 4 +c)

typedef struct {
    float x;
    float y;
} vec2;

vec2 vec2_add(vec2 v, vec2 u);
vec2 vec2_sub(vec2 v, vec2 u);

float vec2_det(vec2 c1, vec2 c2);  // returns the determinant of the matrix whose columns are the parameter vectors

typedef struct {
    float x, y, z;
} vec3;

vec3 vec3_add(vec3 v, vec3 u);
vec3 vec3_sub(vec3 v, vec3 u);

vec2 to_v2(vec3 v);

typedef struct {
    int x, y;
} ivec2;

typedef struct {
    int x, y, z;
} ivec3;

float dot2(vec2 v, vec2 u);
float dot3(vec3 v, vec3 u);

ivec2 to_ivec2(vec2 v);
ivec3 to_ivec3(vec3 v);
vec2 to_vec2(ivec2 v);
vec3 to_vec3(ivec3 v);

typedef enum {
    AXIS_X, AXIS_Y, AXIS_Z, AXIS_W
} Axis;

typedef union {
    struct {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
    };
    float m[16];
} mat3;

mat3 mat3_identity();
mat3 mat3_transpose(mat3 m);
mat3 mat3_inverse(mat3 m, float det);
float mat3_get(mat3* m, int row, int col);
float* mat3_get_ref(mat3* m, int row, int col);
void mat3_multiply(mat3* dest, mat3* m, mat3* n);
void mat3_transform(mat3* m, vec3* v);
void mat3_axis_rotation_matrix(mat3* m, float t, Axis axis);
mat3 mat3_rotation_matrix(float t, vec3 rotation_axis);
mat3 mat3_transform_matrix(vec3 i, vec3 j, vec3 k);  // change of basis
void mat3_rotate(mat3* m, float t, Axis axis);
void mat3_print(mat3* m);

typedef union {
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float m[16];
} mat4;

mat4 mat4_identity();
float mat4_get(mat4* m, int row, int col);
float* mat4_get_ref(mat4* m, int row, int col);
void mat4_multiply(mat4* dest, mat4* m, mat4* n);
void mat4_rotation_matrix(mat4* m, float t, Axis axis);
void mat4_rotate(mat4* m, float t, Axis axis);
void mat4_ortho(mat4* m, float left, float right, float bottom, float top, float near, float far);
void mat4_print(mat4* m);

#ifdef LINEAR_MATH_IMPLEMENTATION

mat3 mat3_identity() {
    return (mat3) {
        .m00 = 1, .m01 = 0, .m02 = 0,
        .m10 = 0, .m11 = 1, .m12 = 0,
        .m20 = 0, .m21 = 0, .m22 = 1,
    };
}

float mat3_get(mat3* m, int row, int col) {
    float* c = (float*)m;
    return c[row * 3 + col];
}

float* mat3_get_ref(mat3* m, int row, int col) {
    float* c = (float*)m;
    return &c[row * 3 + col];
}

void mat3_transform(mat3* m, vec3* v) {
    *v = (vec3){
        .x = m->m00 * v->x + m->m01 * v->y + m->m02 * v->z,
        .y = m->m10 * v->x + m->m11 * v->y + m->m12 * v->z,
        .z = m->m20 * v->x + m->m21 * v->y + m->m22 * v->z,
    };
}

// the dest and the arguments can be the same thanks to the copy
void mat3_multiply(mat3* dest, mat3* m, mat3* n) {
    mat3 d;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float accum = 0;
            for (int k = 0; k < 3; k++) {
                accum += m->m[INDEX(i, k)] * n->m[INDEX(k, j)];
            }

            d.m[INDEX(i, j)] = accum;
        }
    }

    memcpy(dest, &d, sizeof(mat3));
}

void mat3_axis_rotation_matrix(mat3* m, float t, Axis axis) {
    float cosine = cos(t);
    float sine   = sin(t);

    int r1, r2, c1, c2;
    if (axis == AXIS_X) {
        r1 = 1; r2 = 2;
        c1 = 1; c2 = 2;
    }
    else if (axis == AXIS_Y) {
        r1 = 0; r2 = 2;
        c1 = 0; c2 = 2;
    }
    else if (axis == AXIS_Z) {
        c1 = 0; c2 = 1;
        r1 = 0; r2 = 1;
    }

    *m = mat3_identity();

    m->m[INDEX(r1, c2)] = - sine;
    m->m[INDEX(r1, c1)] = cosine;
    m->m[INDEX(r2, c1)] = sine;
    m->m[INDEX(r2, c2)] = cosine;
}

mat3 mat3_transpose(mat3 m) {
    return (mat3) {
        .m00 = m.m00, .m01 = m.m10, .m02 = m.m20,
        .m10 = m.m01, .m11 = m.m11, .m12 = m.m21,
        .m20 = m.m02, .m21 = m.m12, .m22 = m.m22,
    };
}

mat3 mat3_rotation_matrix(float t, vec3 rotation_axis) {
    mat3 rot_matrix = mat3_identity();

    // new basis
    vec3 nx = rotation_axis;
    vec3 ny = vec3_perpendicular(rotation_axis);
    vec3 nz = vec3_cross(nx, ny);

    // forward transform into the new basis
    mat3 forward = mat3_transform_matrix(nx, ny, nz);
    mat3_multiply(&rot_matrix, &rot_matrix, &forward);

    float c = cos(t);
    float s = sin(t);
    mat3 local_rot_x = (mat3){
        .m00 = 1, .m01 = 0, .m02 = 0,
        .m10 = 0, .m11 = c, .m12 = -s,
        .m20 = 0, .m21 = s, .m22 =  c,
    };
    mat3_multiply(&rot_matrix, &local_rot_x, &rot_matrix);

    // backward transform back to the old basis

    // since the transform is to an orthonormal basis, the matrix is orthogonal which means it's inverse is equal to it's transpose
    mat3 backward = mat3_transpose(forward);
    mat3_multiply(&rot_matrix, &backward, &rot_matrix);

    return rot_matrix;
}

void mat3_rotate(mat3* m, float t, Axis axis)
{
    mat3 rot_matrix;
    mat3_axis_rotation_matrix(&rot_matrix, t, axis);
    mat3_multiply(m, m, &rot_matrix);
}

float mat3_det(mat3 m) {
    return m.m00 * (m.m11 * m.m22 - m.m21 * m.m12) + m.m01 * (m.m20 * m.m12 - m.m10 * m.m22) + m.m02 * (m.m10 * m.m21 - m.m20 * m.m11);
}

mat3 mat3_inverse(mat3 m, float det) {
    mat3 inv = mat3_identity();

    float inv_det = 1 / det;

    inv.m00 =  (m.m11 * m.m22 - m.m12 * m.m21) * inv_det;
    inv.m01 = -(m.m01 * m.m22 - m.m02 * m.m21) * inv_det;
    inv.m02 =  (m.m01 * m.m12 - m.m02 * m.m11) * inv_det;

    inv.m10 = -(m.m10 * m.m22 - m.m12 * m.m20) * inv_det;
    inv.m11 =  (m.m00 * m.m22 - m.m02 * m.m20) * inv_det;
    inv.m12 = -(m.m00 * m.m12 - m.m02 * m.m10) * inv_det;

    inv.m20 =  (m.m10 * m.m21 - m.m11 * m.m20) * inv_det;
    inv.m21 = -(m.m00 * m.m21 - m.m01 * m.m20) * inv_det;
    inv.m22 =  (m.m00 * m.m11 - m.m01 * m.m10) * inv_det;

    return inv;
}

mat3 mat3_transform_matrix(vec3 i, vec3 j, vec3 k) {
    mat3 forward = (mat3) {
        .m00 = i.x, .m01 = j.x, .m02 = k.x,
        .m10 = i.y, .m11 = j.y, .m12 = k.y,
        .m20 = i.z, .m21 = j.z, .m22 = k.z,
    };

    return mat3_inverse(forward, mat3_det(forward));
}

void mat3_print(mat3* m) {
    printf("%.2f %.2f %.2f\n", m->m[INDEX(0, 0)], m->m[INDEX(0, 1)], m->m[INDEX(0, 2)]);
    printf("%.2f %.2f %.2f\n", m->m[INDEX(1, 0)], m->m[INDEX(1, 1)], m->m[INDEX(1, 2)]);
    printf("%.2f %.2f %.2f\n", m->m[INDEX(2, 0)], m->m[INDEX(2, 1)], m->m[INDEX(2, 2)]);
    printf("\n");
}

mat4 mat4_identity() {
    return (mat4) {
        .m00 = 1, .m01 = 0, .m02 = 0, .m03 = 0,
        .m10 = 0, .m11 = 1, .m12 = 0, .m13 = 0,
        .m20 = 0, .m21 = 0, .m22 = 1, .m23 = 0,
        .m30 = 0, .m31 = 0, .m32 = 0, .m33 = 1,
    };
}

float mat4_get(mat4* m, int row, int col) {
    float* c = (float*)m;
    return c[row * 4 + col];
}

float* mat4_get_ref(mat4* m, int row, int col) {
    float* c = (float*)m;
    return &c[row * 4 + col];
}

void mat4_multiply(mat4* dest, mat4* m, mat4* n) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float accum = 0;
            for (int k = 0; k < 4; k++) {
                accum += m->m[INDEX(i, k)] * n->m[INDEX(k, j)];
            }

            dest->m[INDEX(i, j)] = accum;
        }
    }
}

void mat4_rotation_matrix(mat4* m, float t, Axis axis) {
    float cosine = cos(t);
    float sine   = sin(t);

    int r1, r2, c1, c2;
    if (axis == AXIS_X) {
        r1 = 1; r2 = 2;
        c1 = 1; c2 = 2;
    }
    else if (axis == AXIS_Y) {
        r1 = 0; r2 = 2;
        c1 = 0; c2 = 2;
    }
    else if (axis == AXIS_Z) {
        c1 = 0; c2 = 1;
        r1 = 0; r2 = 1;
    }

    *m = mat4_identity();

    m->m[INDEX(r1, c2)] = - sine;
    m->m[INDEX(r1, c1)] = cosine;
    m->m[INDEX(r2, c1)] = sine;
    m->m[INDEX(r2, c2)] = cosine;
}

void mat4_rotate(mat4* m, float t, Axis axis)
{
    mat4 rot_matrix;
    mat4_rotation_matrix(&rot_matrix, t, axis);
    mat4_multiply(m, m, &rot_matrix);
}

// https://learnwebgl.brown37.net/08_projections/projections_ortho.html
void mat4_ortho(mat4* m, float left, float right, float bottom, float top, float near, float far)
{
    float* mc = (float*)m;

    mc[INDEX(0, 0)] = 2.0f / (right - left);
    mc[INDEX(0, 1)] = 0;
    mc[INDEX(0, 2)] = 0;
    mc[INDEX(0, 3)] = - (right + left) / (right - left);

    mc[INDEX(1, 0)] = 0;
    mc[INDEX(1, 1)] = 2.0f / (top - bottom);
    mc[INDEX(1, 2)] = 0;
    mc[INDEX(1, 3)] = - (top + bottom) / (top - bottom);

    mc[INDEX(2, 0)] = 0;
    mc[INDEX(2, 1)] = 0;
    mc[INDEX(2, 2)] = 2.0f / (near - far);
    mc[INDEX(2, 3)] = - (far + near) / (far - near);

    mc[INDEX(3, 0)] = 0;
    mc[INDEX(3, 1)] = 0;
    mc[INDEX(3, 2)] = 0;
    mc[INDEX(3, 3)] = 1;
}

void mat4_print(mat4* m) {
    printf("%.2f %.2f %.2f %.2f\n", m->m[INDEX(0, 0)], m->m[INDEX(0, 1)], m->m[INDEX(0, 2)], m->m[INDEX(0, 3)]);
    printf("%.2f %.2f %.2f %.2f\n", m->m[INDEX(1, 0)], m->m[INDEX(1, 1)], m->m[INDEX(1, 2)], m->m[INDEX(1, 3)]);
    printf("%.2f %.2f %.2f %.2f\n", m->m[INDEX(2, 0)], m->m[INDEX(2, 1)], m->m[INDEX(2, 2)], m->m[INDEX(2, 3)]);
    printf("%.2f %.2f %.2f %.2f\n", m->m[INDEX(3, 0)], m->m[INDEX(3, 1)], m->m[INDEX(3, 2)], m->m[INDEX(3, 3)]);
    printf("\n");
}

ivec2 to_ivec2(vec2 v) {
    return (ivec2){(int)v.x, (int)v.y};
}

ivec3 to_ivec3(vec3 v) {
    return (ivec3){(int)v.x, (int)v.y, (int)v.z};
}

vec2 to_vec2(ivec2 v) {
    return (vec2){(float)v.x, (float)v.y};
}

vec3 to_vec3(ivec3 v) {
    return (vec3){(float)v.x, (float)v.y, (float)v.z};
}

float dot2(vec2 v, vec2 u) {
    return v.x * u.x + v.y * u.y;
}

float dot3(vec3 v, vec3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

vec2 vec2_add(vec2 v, vec2 u) {
    return (vec2){.x = v.x + u.x, .y = v.y + u.y};
}

vec2 vec2_sub(vec2 v, vec2 u) {
    return (vec2){.x = v.x - u.x, .y = v.y - u.y};
}

vec3 vec3_add(vec3 v, vec3 u) {
    return (vec3){.x = v.x + u.x, .y = v.y + u.y, .z = v.z + u.z};
}

vec3 vec3_sub(vec3 v, vec3 u) {
    return (vec3){.x = v.x - u.x, .y = v.y - u.y, .z = v.z - u.z};
}

// returns the determinant of the matrix whose columns are the parameter vectors
float vec2_det(vec2 c1, vec2 c2) {
    return c1.x * c2.y - c1.y * c2.x;
}

vec2 to_v2(vec3 v) {return (vec2) {.x = v.x, .y = v.y};}

vec2 vec2_normalize(vec2 v) {
    float len = sqrt(v.x * v.x + v.y * v.y);
    return (vec2){v.x/len, v.y/len};
}

#endif // LINEAR_MATH_IMPLEMENTATION

#endif // _LINEAR_MATH
