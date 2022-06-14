#ifndef JSON_PRIVATE_H
#define JSON_PRIVATE_H
#include "json.h"
#include "set.h"
#include <cstdio>

class Loader{
private:
    const char *path;
    FILE *fd;
    char *text;
    Result *error;
    bool read();
    bool expand(size_t *size);
public:
    Loader(const char *path) noexcept;
    Result * load();
};

struct StrSlice{
    const char *begin;
    const char *end;
    bool operator==(const StrSlice &rhs) const;
};

class Validator{
private:
    const char *text;
    void symbol_next(size_t step = 1);
    void line_next();
    void skip_whitespace();
    bool field(Set<StrSlice> &set);
    bool object();
    bool array();
    bool string();
    bool number();
    bool special();
    bool branch();
public:
    Validator(const char *text) noexcept;
    void validate();
    size_t line;
    size_t symbol;
    size_t index;
    const char *message;
};

#endif // JSON_PRIVATE_H
