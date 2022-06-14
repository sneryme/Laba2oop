#ifndef SET_H
#define SET_H
#include <cstring>
#include <stdexcept>

template <typename T> class Set{
private:
    T *data;
    size_t _size;
    size_t _capacity;
    void expand() noexcept;
public:
    Set() noexcept;
    Set(const Set<T> &set) noexcept;
    Set(Set<T> &&set) noexcept;
    ~Set() noexcept;
    size_t size() const noexcept;
    const T & get(size_t index) const;
    bool add(const T &element) noexcept;
};

template <typename T> Set<T>::Set() noexcept : data(nullptr), _size(0), _capacity(0){}

template <typename T> Set<T>::Set(const Set<T> &set) noexcept : data(new T[set._size]), _size(set._size), _capacity(set._capacity){
    for (size_t i = 0; i < _size; i++)
        data[i] = set.data[i];
}

template <typename T> Set<T>::Set(Set<T> &&set) noexcept : data(set.data), _size(set._size), _capacity(set._capacity){
    set.data = nullptr;
}

template <typename T> Set<T>::~Set() noexcept{
    if (data != nullptr)
        delete[] data;
}

template <typename T> size_t Set<T>::size() const noexcept { return _size; }

template <typename T> const T & Set<T>::get(size_t index) const{
    if (index >= _size)
        throw new std::invalid_argument("Index OOB");

    return data[index];
}

template <typename T> bool Set<T>::add(const T &element) noexcept{
    for (size_t i = 0; i < _size; i++)
        if (data[i] == element)
            return false;

    if (_size == _capacity)
        expand();

    data[_size++] = element;
    return true;
}

template <typename T> void Set<T>::expand() noexcept{
    _capacity = _capacity == 0 ? 1 : _capacity * 2;
    T *data2 = new T[_capacity];
    if (data != nullptr) {
        for (size_t i = 0; i < _size; i++)
            data2[i] = data[i];
        delete[] data;
    }

    data = data2;
}

#endif // END_H
