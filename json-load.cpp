#include "json-private.h"
#include <cstdio>
#include <cstdlib>
#define ERROR_INTERNAL     "Internal error"
#define ERROR_ALLOCATION   "Allocation failure"
#define ERROR_PATH_EMPTY   "Empty path"
#define ERROR_FILE_READING "Failed to read file data"
#define ERROR_FILE_CLOSING "Failed to close file"
#define INITIAL_BUFFER_SIZE 16

Loader::Loader(const char *_path) noexcept :
    path(_path), fd(nullptr), text(nullptr),error(nullptr){}

Result * Loader::load() {
    if (*path == '\0')
        return new ErrorResult(ERROR_PATH_EMPTY);

    fd = fopen(path, "r");
    if (fd == nullptr)
        return new ErrorResult(ERROR_FILE_READING);

    LoadResult *result = nullptr;
    if (read()) {
        result = new LoadResult(text);
        if (result == nullptr)
            error = new ErrorResult(ERROR_ALLOCATION);
    }

    return result ? result : error;
}

bool Loader::read(){
    size_t size = INITIAL_BUFFER_SIZE;
    size_t limit = size;
    text = (char *) calloc(size, sizeof(char));
    if (text == nullptr) {
        error = new ErrorResult(ERROR_ALLOCATION, true);
        return false;
    }

    char *cursor = text;
    bool flag = true;
    while (flag) {
        if (!fgets(cursor, limit, fd)) {
            error = new ErrorResult(ERROR_FILE_READING);
            flag = false;
        } else {
            size_t newSize = strlen(text);
            if (newSize == size - 1 && !expand(&size))
                flag = false;
            else {
                cursor = text + newSize;
                limit = size - newSize;
            }
        }
    }

    if (feof(fd))
        return true;
    else {
        free(text);
        return false;
    }
}

bool Loader::expand(size_t *size) {
    *size *= 2;
    char *text2 = (char *) realloc(text, *size * sizeof(char));
    if (text2 == nullptr) {
        error = new ErrorResult(ERROR_ALLOCATION, true);
        return false;
    } else {
        text = text2;
        return true;
    }
}
