#include "json-private.h"

#include <stdexcept>

Result * Context::run(const Operation &operation) {
    return operation.run(*this);
}
size_t Context::line() const noexcept { return _line; }
size_t Context::symbol() const noexcept { return _symbol; }
size_t Context::index() const noexcept { return _index; }
const char * Context::message() const noexcept { return _message; }
void Context::onMistake(size_t line, size_t symbol, size_t index, const char *message) noexcept {
    _line = line;
    _symbol = symbol;
    _index = index;
    _message = message;
}

LoadOperation::LoadOperation(const char *_path) noexcept : path(_path) {}
Result * LoadOperation::run(Context &ctx) const {
    Loader loader(path);
    return loader.load();
}

ValidateOperation::ValidateOperation(const char *_text) noexcept : text(_text) {}
Result * ValidateOperation::run(Context &ctx) const{
    Validator validator(text);
    validator.validate();
    if (validator.message) {
        ctx.onMistake(validator.line, validator.symbol, validator.index, validator.message);
        return new ValidateResult();
    } else {
        ctx.onMistake(0, 0, 0, nullptr);
        return new SuccessResult();
    }
}

LoadResult::LoadResult(char *text) noexcept : _text(text) {}
LoadResult::~LoadResult() noexcept{
    free(_text);
}
const char * LoadResult::text() const noexcept { return _text; }
ErrorResult::ErrorResult(const char *_message, bool _fatal) noexcept : message(_message),fatal(_fatal) {}
