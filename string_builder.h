#ifndef _STRING
#define _STRING

// common string utilities
// as well as a string view structure and a string builder implementation

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define CSTRING_LENGTH(s) (sizeof(s)-1)
  
typedef struct {
    const char* data;
    int size;
} String;

String make_string(const char* s);
String make_string_slice(const char* s, int start, int end);
int string_length(const char* s);
bool string_starts_with(String str, const char* prefix);
bool string_ends_with(String str, const char* postfix);
void print_string(String s);
String trim(String s);
String trim_start(String s);
String trim_end(String s);

typedef struct {
    String* data;
    int size;
    int cap;
} String_List;

String_List make_string_list(int init_cap);
void string_list_append(String_List* list, String s);
String_List split(String s, char delimeter);

typedef struct {
    char* buffer;
    int buffer_capacity;
    int cursor;
} String_Builder;

String_Builder make_string_builder(int initial_capacity);
void sb_append(String_Builder* sb, String string);
void sb_append_char(String_Builder* sb, char ch);
const char* sb_to_c_string(String_Builder* sb);
void sb_clear_and_append(String_Builder* sb, String s);
void sb_append_many(String_Builder* sb, String* strings, int n);
void sb_free(String_Builder* sb);
void sb_clear(String_Builder* sb);

#ifdef STRING_IMPLEMENTATION

String make_string(const char* s) {
    int len = string_length(s);
    return (String) {.data = s, .size = len};
}

int string_length(const char* s) {
    int count = 0;
    while (*s) {
        count++;
        s++;
    }

    return count;
}

String make_string_slice(const char* s, int start, int end) {  // the end index is not included
    if (!(end >= start)) {
        fprintf(stderr, "Invalid argument for make string slice\n");
        exit(1);
    }

    return (String) {
        .data = s + start,
        .size = end - start
    };
}

void print_string(String s) {
    char* buff = (char*)alloca(s.size + 1);
    memcpy(buff, s.data, s.size);
    buff[s.size] = '\0';
    printf("%s", buff);
}

String trim_start(String str) {
    String s = str;
    for (int i = 0; i < s.size; i++) {
        if (s.data[i] == ' ' || s.data[i] == '\t' || s.data[i] == '\n') {
            s.data += 1;
            s.size -= 1;
        }
        else break;
    }

    return s;
}

String trim_end(String str) {
    String s = str;
    for (int i = s.size; i > 0; i--) {
        if (s.data[i] == ' ' || s.data[i] == '\t' || s.data[i] == '\n') {
            s.size -= 1;
        }
        else break;
    }

    return s;
}

String trim(String s) {
    return trim_start(trim_end(s));
}

bool string_starts_with(String str, const char* prefix) {
    String pre = make_string(prefix);
    if (str.size < pre.size) return false;
    for (int i = 0; i < pre.size; i++) {
        if (str.data[i] != pre.data[i]) return false;
    }
    return true;
}

bool string_ends_with(String str, const char* postfix) {
    String post = make_string(postfix);
    if (str.size < post.size) return false;
    int index = str.size - post.size;
    for (int i = 0; i < post.size; i++) {
        if (str.data[index + i] != post.data[i]) return false;
    }
    return true;
}

String_Builder make_string_builder(int initial_capacity) {
    String_Builder sb = (String_Builder) {
        .buffer = NULL,
        .buffer_capacity = 0,
        .cursor = 0,
    };

    sb.buffer = (char*)malloc(initial_capacity);
    sb.buffer_capacity = initial_capacity;
    sb.cursor = 0;
    sb.buffer[0] = '\0';
    return sb;
}

void sb_resize(String_Builder* sb) {
    char* nbuff = (char*)malloc(sb->buffer_capacity * 2 * sizeof(char));
    memcpy(nbuff, sb->buffer, sb->cursor);
    free(sb->buffer);
    sb->buffer = nbuff;
    sb->buffer_capacity *= 2;
}

int sb_grow_to_size(String_Builder* sb, int size) {
    int count = 0;
    while (size >= sb->buffer_capacity) {
        sb_resize(sb);
        count++;
        if (count > 5) {
            fprintf(stderr, "String builder buffer resize failed repeatedly: Possible memory allocation issue or corrupted buffer state.\n"
                            "Relevant: buffer_capacity: %d, cursor: %d, provided string size: %d",
                            sb->buffer_capacity, sb->cursor, size);
            return 1;
        }
    }

    return 0;
}

void sb_append(String_Builder* sb, String string) {
    sb_grow_to_size(sb, sb->cursor + string.size);

    memcpy(sb->buffer + sb->cursor, string.data, string.size);
    sb->cursor += string.size;
}

void sb_append_char(String_Builder* sb, char ch) {
    sb_grow_to_size(sb, sb->cursor + 1);

    sb->buffer[sb->cursor] = ch;
    sb->cursor += 1;
}

void sb_clear_and_append(String_Builder* sb, String s) {
    sb->cursor = 0;
    sb_append(sb, s);
}

void sb_append_many(String_Builder* sb, String* strings, int n) {
    int total_length = 0;
    for (int i = 0; i < n; i++) {
        total_length += strings[i].size;
    }

    sb_grow_to_size(sb, sb->cursor + total_length);
    for (int i = 0; i < n; i++) {
        memcpy(sb->buffer + sb->cursor, strings[i].data, strings[i].size);
        sb->cursor += strings[i].size;
    }
}

const char* sb_to_c_string(String_Builder* sb) {
    sb->buffer[sb->cursor] = '\0';
    return sb->buffer;
}

void sb_free(String_Builder* sb) {
    free(sb->buffer);
    sb->cursor = 0;
    sb->buffer_capacity = 0;
    sb->buffer = NULL;
}

void sb_clear(String_Builder* sb) {
    sb->cursor = 0;
    sb->buffer[0] = '\0';
}

String_List split(String string, char delimeter) {
    String_List list = make_string_list(string.size / 10);  // careful with allocation on large inputs
    int start = 0;
    for (int i = 0; i < string.size; i++) {
        if (string.data[i] == delimeter) {
            String s = (String){.data = string.data + start, .size = i - start};
            string_list_append(&list, s);
            start = i + 1;
        }
    }

    string_list_append(&list, (String){.data = string.data + start, .size = string.size - start});

    return list;
}

String_List make_string_list(int init_cap) {
    int cap = MAX(8, init_cap);
    String_List list;
    list.data = malloc(cap * sizeof(String));
    list.cap = cap;
    list.size = 0;
    return list;
}

void string_list_append(String_List* list, String s) {
    assert(list->cap);

    if (list->size + 1 >= list->cap) {
        int new_cap = list->cap * 2;
        String* ndata = malloc(new_cap);
        memcpy(ndata, list->data, list->size * sizeof(String));
        free(list->data);
        list->data = ndata;
        list->cap = new_cap;
    }

    list->data[list->size] = s;
    list->size += 1;
}

#endif  // STRING_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _STRING
