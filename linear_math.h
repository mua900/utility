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

#endif // LINEAR_MATH_IMPLEMENTATION

#endif // _LINEAR_MATH
