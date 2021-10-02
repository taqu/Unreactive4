#pragma once
// clang-format off
/*
Copyright (c) 2021 Takuro Sakai

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**
 * @file UNRX4Container.h
 * @author t-sakai
 */
// clang-format on
#include "UNRX4.h"

//-------------------
/**
 * @brief Simple std::vector implementation with using this lib's specific allocator, restricted operations.
 * @tparam T ... Element type
*/
template<class T>
class UNRX4Array
{
public:
    static constexpr unrx4::size_t Expand = 16;

    UNRX4Array();

    /**
     * @brief Construct and reserve the buffer for elements.
     * @param capacity ... Initial capacity
     * @warn Behave like C#'s List, not C++'s vector
    */
    explicit UNRX4Array(unrx4::size_t capacity);

    UNRX4Array(UNRX4Array&& other);
    ~UNRX4Array();

    UNRX4Array& operator=(UNRX4Array&& other);

    unrx4::size_t capacity() const;
    unrx4::size_t size() const;
    void clear();

    void push_back(const T& x);
    void push_back(T&& x);

    void pop_back();
    void pop_front();
    void remove(T target);
    void removeAt(unrx4::size_t index);

    const T& operator[](unrx4::size_t index) const;
    T& operator[](unrx4::size_t index);

    const T* cbegin() const;
    const T* cend() const;
    T* begin();
    T* end();

private:
    UNRX4Array(const UNRX4Array&) = delete;
    UNRX4Array& operator=(const UNRX4Array&) = delete;

    void resize(unrx4::size_t capacity);
    unrx4::size_t capacity_;
    unrx4::size_t size_;
    T* items_;
};

template<class T>
UNRX4Array<T>::UNRX4Array()
    : capacity_(0)
    , size_(0)
    , items_(nullptr)
{
}

template<class T>
UNRX4Array<T>::UNRX4Array(unrx4::size_t capacity)
    : capacity_(0)
    , size_(0)
    , items_(nullptr)
{
    resize(capacity);
}

template<class T>
UNRX4Array<T>::UNRX4Array(UNRX4Array&& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , items_(other.items_)
{
    other.capacity_ = 0;
    other.size_ = 0;
    other.items_ = nullptr;
}

template<class T>
UNRX4Array<T>::~UNRX4Array()
{
    clear();
    FMemory::Free(items_);
    capacity_ = 0;
    size_ = 0;
    items_ = nullptr;
}

template<class T>
UNRX4Array<T>& UNRX4Array<T>::operator=(UNRX4Array<T>&& other)
{
    if(this == &other) {
        return *this;
    }
    clear();
    FMemory::Free(items_);
    capacity_ = other.capacity_;
    size_ = other.size_;
    items_ = other.items_;
    other.capacity_ = 0;
    other.size_ = 0;
    other.items_ = nullptr;
    return *this;
}

template<class T>
unrx4::size_t UNRX4Array<T>::capacity() const
{
    return capacity_;
}

template<class T>
unrx4::size_t UNRX4Array<T>::size() const
{
    return size_;
}

template<class T>
void UNRX4Array<T>::clear()
{
    for(unrx4::size_t i = 0; i < size_; ++i) {
        items_[i].~T();
    }
    size_ = 0;
}

template<class T>
void UNRX4Array<T>::push_back(const T& x)
{
    if(capacity_ <= size_) {
        resize(capacity_ + Expand);
    }
    new(&items_[size_]) T(x);
    ++size_;
}

template<class T>
void UNRX4Array<T>::push_back(T&& x)
{
    if(capacity_ <= size_) {
        resize(capacity_ + Expand);
    }
    new(&items_[size_]) T(std::move(x));
    ++size_;
}

template<class T>
void UNRX4Array<T>::pop_back()
{
    UNRX4_ASSERT(0 < size_);
    --size_;
    items_[size_].~T();
}

template<class T>
void UNRX4Array<T>::pop_front()
{
    UNRX4_ASSERT(0 < size_);
    for(unrx4::size_t i = 1; i < size_; ++i) {
        items_[i - 1] = std::move(items_[i]);
    }
    --size_;
    items_[size_].~T();
}

template<class T>
void UNRX4Array<T>::remove(T target)
{
    for(unrx4::size_t i = 0; i < size_; ++i) {
        if(target == items_[i]){
            removeAt(i);
            return;
        }
    }
}

template<class T>
void UNRX4Array<T>::removeAt(unrx4::size_t index)
{
    UNRX4_ASSERT(index < size_);
    for(unrx4::size_t i = index + 1; i < size_; ++i) {
        items_[i - 1] = std::move(items_[i]);
    }
    --size_;
    items_[size_].~T();
}

template<class T>
const T& UNRX4Array<T>::operator[](unrx4::size_t index) const
{
    UNRX4_ASSERT(index < size_);
    return items_[index];
}

template<class T>
T& UNRX4Array<T>::operator[](unrx4::size_t index)
{
    UNRX4_ASSERT(index < size_);
    return items_[index];
}

template<class T>
const T* UNRX4Array<T>::cbegin() const
{
    return items_;
}

template<class T>
const T* UNRX4Array<T>::cend() const
{
    return items_ + size_;
}

template<class T>
T* UNRX4Array<T>::begin()
{
    return items_;
}

template<class T>
T* UNRX4Array<T>::end()
{
    return items_ + size_;
}

template<class T>
void UNRX4Array<T>::resize(unrx4::size_t capacity)
{
    T* items = reinterpret_cast<T*>(FMemory::Malloc(capacity * sizeof(T)));
    for(unrx4::size_t i = 0; i < size_; ++i) {
        new(&items[i]) T(std::move(items_[i]));
    }
    clear();
    FMemory::Free(items_);
    capacity_ = capacity;
    items_ = items;
}
