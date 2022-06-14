#include "json-private.h"

#include <iostream>

#define MESSAGE_FIELD_EXPECTED_NAME "Expected field name"
#define MESSAGE_EXPECTED_SEMICOLON  "Expected semicolon separator"
#define MESSAGE_EXPECTED_COMMA      "Expected comma separator"
#define MESSAGE_OBJECT_DUPLICATE    "Duplicate field"
#define MESSAGE_OBJECT_UNCLOSED     "Unclosed object"
#define MESSAGE_ARRAY_UNCLOSED      "Unclosed array"
#define MESSAGE_STRING_NEW_LINE     "Unexpected new line in string"
#define MESSAGE_STRING_UNCLOSED     "Unclosed string"
#define MESSAGE_SPECIAL_UNEXPECTED  "Expected 'true', 'false' or 'null' as a special value"
#define MESSAGE_UNEXPECTED          "Unexpected symbol"
#define MESSAGE_ONE_ROOT            "Only one root object"

bool StrSlice::operator==(const StrSlice &rhs) const{
    return (end - begin) == (rhs.end - rhs.begin) && !strncmp(begin, rhs.begin, end - begin);
}

Validator::Validator(const char *_text) noexcept :
    text(_text), line(0), symbol(0), index(0), message(nullptr) {}

void Validator::validate() {
    bool result = true;
    skip_whitespace();
    if (*text != '\0' && (*text != '{' || !object()))
        result = false;
    else {
        skip_whitespace();
        if (*text != '\0') {
            message = MESSAGE_ONE_ROOT;
            result = false;
        }
    }

    if (!result && message == nullptr)
        message = "";
}

void Validator::symbol_next(size_t step){
    symbol += step;
    index += step;
}

void Validator::line_next(){
    line++;
    symbol = 0;
    index++;
}

void Validator::skip_whitespace(){
    for (; *text; text++) {
        if (*text == ' ' || *text == '\t')
            symbol_next();
        else if (*text == '\n')
            line_next();
        else
            break;
    }
}

bool Validator::field(Set<StrSlice> &set){
    if (*text != '"') {
        message = MESSAGE_FIELD_EXPECTED_NAME;
        return false;
    }

    const char *begin = text + 1;
    if (!string())
        return false;
    const char *end = text - 2;
    StrSlice slice { begin, end };
    if (!set.add(slice)) {
        message = MESSAGE_OBJECT_DUPLICATE;
        return false;
    }

    skip_whitespace();

    if (*text != ':') {
        message = MESSAGE_EXPECTED_SEMICOLON;
        return false;
    }
    text++;
    symbol_next();

    skip_whitespace();

    return branch();
}

bool Validator::object(){
    bool closed = false;
    bool comma = false;
    Set<StrSlice> fields;

    text++;
    symbol_next();
    for (; *text; text++, symbol_next()) {
        skip_whitespace();
        if (*text == '}' && !comma) {
            text++;
            symbol_next();
            closed = true;
            break;
        }

        if (!field(fields))
            break;

        skip_whitespace();
        if (*text == '}') {
            text++;
            symbol_next();
            closed = true;
            break;
        }

        if (*text != ',') {
            message = MESSAGE_EXPECTED_COMMA;
            break;
        }
        comma = true;
    }

    if (!closed && message == nullptr)
        message = MESSAGE_OBJECT_UNCLOSED;

    return closed;
}

bool Validator::array(){
    bool closed = false;
    bool comma = false;
    text++;
    symbol_next();
    for (; *text; text++, symbol_next()) {
        skip_whitespace();
        if (*text == ']' && !comma) {
            text++;
            symbol_next();
            closed = true;
            break;
        }

        if (!branch())
            break;

        skip_whitespace();
        if (*text == ']') {
            text++;
            symbol_next();
            closed = true;
            break;
        }

        if (*text != ',') {
            message = MESSAGE_EXPECTED_COMMA;
            break;
        }
        comma = true;
    }

    if (!closed && message == nullptr)
        message = MESSAGE_ARRAY_UNCLOSED;

    return closed;
}

bool Validator::string(){
    bool closed = false;
    text++;
    symbol_next();
    for (; *text; text++, symbol_next()) {
        if (*text == '\n') {
            message = MESSAGE_STRING_NEW_LINE;
            break;
        }
        if (*text == '"') {
            text++;
            symbol_next();
            closed = true;
            break;
        }
    }

    if (!closed && message == nullptr)
        message = MESSAGE_STRING_UNCLOSED;

    return closed;
}

bool Validator::number(){
    bool valid = true;
    bool dot = false;
    if (*text == '-') {
        text++;
        symbol_next();
    }

    for (; *text; text++, symbol_next()) {
        if (*text == '.' && !dot)
            dot = true;
        else if (*text >= '0' && *text <= '9');
        else break;
    }

    return valid;
}

bool Validator::special(){
    if (!strncmp(text, "true", 4) || !strncmp(text, "null", 4)) {
        text = (text) + 4;
        symbol_next(4);
        return true;
    } else if (!strncmp(text, "false", 5)) {
        text = (text) + 5;
        symbol_next(5);
        return true;
    } else {
        message = MESSAGE_SPECIAL_UNEXPECTED;
        return false;
    }
}

bool Validator::branch(){
    switch (*text) {
    case '{':
        return object();
    case '[':
        return array();
    case '"':
        return string();
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return number();
    case 't':
    case 'f':
    case 'n':
        return special();
    case '\0':
        return true;
    default:
        message = MESSAGE_UNEXPECTED;
        return false;
    }
}
