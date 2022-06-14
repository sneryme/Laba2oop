#ifndef JSON_H
#define JSON_H
#include <cstring>

class Operation;
class Result;

class Context{
private:
    size_t _line;
    size_t _symbol;
    size_t _index;
    const char *_message;
public:
    explicit Context() noexcept = default;
    Result * run(const Operation &operation);
    size_t line() const noexcept;
    size_t symbol() const noexcept;
    size_t index() const noexcept;
    const char * message() const noexcept;
    void onMistake(size_t line, size_t symbol, size_t index, const char *message) noexcept;
};

class Operation{
protected:
    Operation() = default;
    virtual Result * run(Context &ctx) const = 0;
    friend Result * Context::run(const Operation &operation);
};
class LoadOperation : public Operation{
protected:
    virtual Result * run(Context &ctx) const override;
public:
    explicit LoadOperation(const char *path) noexcept;
    const char * const path;
};
class ValidateOperation : public Operation{
protected: virtual Result * run(Context &ctx) const override;
public:
    explicit ValidateOperation(const char *text) noexcept;
    const char * const text;
};

class Result{
protected:
    Result() = default;
public:
    virtual ~Result() = default;
};

class LoadResult : public Result{
private: char *_text;
public:
    explicit LoadResult(char *text) noexcept;
    ~LoadResult() noexcept;
    const char * text() const noexcept;
};

class ValidateResult : public Result{
public: explicit ValidateResult() noexcept = default;
};
class SuccessResult : public Result{
public: explicit SuccessResult() noexcept = default;
};
class ErrorResult : public Result{
public:
    ErrorResult(const char *message, bool fatal = false) noexcept;
    const char * const message;
    const bool fatal;
};

#endif // JSON_H
