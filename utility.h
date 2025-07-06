#ifndef _UTILITY_H
#define _UTILITY_H

#include <stdint.h>
#include "string_builder.h"

[[noreturn]]
void panic(char const * const);

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define BIT(x) (1 << x)

typedef struct {
    float r, g, b, a;
} FColor;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Color;

#define BLACK Color{0, 0, 0, 0xff}
#define WHITE Color{0xff, 0xff, 0xff, 0xff}

#define RED   Color{0xff, 0, 0, 0xff}
#define GREEN Color{0, 0xff, 0, 0xff}
#define BLUE  Color{0, 0, 0xff, 0xff}

#define COLOR_ARG(color) color.r, color.g, color.b, color.a
FColor to_fcolor(Color color);
Color to_color(FColor fcolor);

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#define ASSERT(condition) do \
  {if (!condition) {fprintf(stderr,"Assertion %s failed\n", #condition); exit(1);}} while(0);

[[noreturn]]
void panic_and_abort(char const * const msg);

uint64_t next_multiple_of_wordsize(uint64_t n);

char* number_to_string(double number, int precision /* after decimal point */);

int hash_string(const String* string);
const char* ordinal_string(int n);
bool compare_string(String, String);

typedef struct {
    unsigned char r, g, b;
} rgb_t;

typedef struct {
    unsigned char r, g, b, a;
} rgba_t;

bool output_ppm(char* file_name, rgb_t* data, int width, int height);

typedef struct {
    char* data;
    size_t size;
} File;

long file_len(FILE* handle);
File load_file(const char* path);

#ifdef UTILITY_IMPLEMENTATION

bool output_ppm(char* file_name, rgb_t* data, int width, int height) {
    FILE* output = fopen(file_name, "w");
    if (!output) {
        return false;
    }

    // header
    const char* magic = "P6";
    const int max_color_value = 255;
    fprintf(output, "%s %d %d %d\n", magic, width, height, max_color_value);

    int size = width * height;
    for (int i = 0; i < size; i++) {
        rgb_t rgb = data[i];
        fprintf(output, "%c%c%c", rgb.r, rgb.g, rgb.b);
    }

    fclose(output);
    return true;
}

[[noreturn]]
void panic(char const * const msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

FColor to_fcolor(Color color) {
  return (FColor) {.r = (float)color.r / 255, .g = (float)color.g / 255, .b = (float)color.b / 255, .a = (float)color.a / 255};
}

Color to_color(FColor fcolor) {
  return (Color) {.r = (u8)(fcolor.r * 255), .g = (u8)(fcolor.g * 255), .b = (u8)(fcolor.b * 255), .a = (u8)(fcolor.a * 255)};
}

uint64_t next_multiple_of_wordsize(uint64_t n) {
    const size_t wordsize = sizeof(void*);
    return ((n-1)|(wordsize-1)) + 1;
}

long file_len(FILE* handle) {
    long curr = ftell(handle);
    fseek(handle, 0, SEEK_END);
    long size = ftell(handle);
    fseek(handle, 0, curr);
    return size;
}

File load_file(const char* path) {
    FILE* handle = fopen(path, "r");

    long file_size = file_len(handle);
    char* data = (char*) malloc(file_size);
    if (!data) {
      fprintf(stderr, "Memory allocation failure trying to load the file %s\n", path);
    }
    long read = fread(data, 1, file_size, handle);
    if (read != file_size) {
      fprintf(stderr, "Failed to read the entirety of the file %s", path);
    }

    fclose(handle);

    return (File){data, (size_t) file_size};
}

char* number_to_string(double number, int precision /* after decimal point */) {
  // decimal

  int range = 0;
  int pow = 1;
  while ((int)number / pow) {
    range++;
    pow *= 10;
  }

  size_t size = range + 2;
  if (precision) {
    size += precision;
  }
  char* buffer = (char*)malloc(size);

  size_t cursor = 0;
  if (number < 0) {
    buffer[0] = '-';
    cursor++;
  }

  if (!range) {
    buffer[cursor] = '0';
    cursor++;
  }
  long n = (long)number;
  pow /= 10;
  for (int i = 0; i < range; i++) {
    buffer[cursor] = ((n / pow) % 10) + '0';
    cursor++;
    pow /= 10;
  }

  if (precision) {
    buffer[cursor] = '.';
    cursor++;
  }

  double mantissa = (number - n);
  for (int i = 0; i < precision; i++) {
    mantissa *= 10;
    buffer[cursor] = ((long)(mantissa) % 10) + '0';
    cursor++;
  }

  buffer[cursor] = '\0';

  return buffer;
}

int hash_string(const String* string) {
    int result = 0;
    int pow = 1;

    for (int i = string->size - 1; i > 0; i--, pow *= 31) {
        result += string->data[i] * pow;
    }

    result += string->data[0] * pow;

    return result;
}

const char* ordinal_string(int n) {
    static char buffer[16];  // enough for large int + suffix + null
    const char *suffix = "th";
    int last_two = n % 100;

    if (last_two < 11 || last_two > 13) {
        switch (n % 10) {
            case 1: suffix = "st"; break;
            case 2: suffix = "nd"; break;
            case 3: suffix = "rd"; break;
        }
    }

    snprintf(buffer, sizeof(buffer), "%d%s", n, suffix);
    return buffer;
}

#endif

#endif // _UTILITY_H
