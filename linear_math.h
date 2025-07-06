#include <math.h>

#define INDEX(r, c) (r * 4 +c)

typedef enum {
    AXIS_X, AXIS_Y, AXIS_Z, AXIS_W
} Axis;

typedef union {
    struct {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;
        float m41, m42, m43, m44;
    };
    float m[16];
} mat4;

mat4 mat4_identity() {
    return (mat4) {
        .m11 = 1, .m12 = 0, .m13 = 0, .m14 = 0,
        .m21 = 0, .m22 = 1, .m23 = 0, .m24 = 0,
        .m31 = 0, .m32 = 0, .m33 = 1, .m34 = 0,
        .m41 = 0, .m42 = 0, .m43 = 0, .m44 = 1,
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
    mat4 result;
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

typedef struct {
    float x;
    float y;
} vec2;

typedef struct {
    float x, y, z;
} vec3;
