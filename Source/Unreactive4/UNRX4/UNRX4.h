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
 * @file UNRX4.h
 * @author t-sakai
 */
// clang-format on
#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

DECLARE_DELEGATE(FUNRX4OnClickDelegate);

#define UNRX4_ASSERT(exp) check(exp)

namespace unrx4
{
using error_code_type = int32;
using time_point = FDateTime;

using s8 = int8;
using s16 = int16;
using s32 = int32;

using u8 = uint8;
using u16 = uint16;
using u32 = uint32;

using size_t = SIZE_T;

using uintptr_t = UPTRINT;
} // namespace unrx4

//-------------------
/**
 * @brief Allocate a memory block. This is malloc as default.
*/
void* unrx4_malloc(size_t size);

/**
 * @brief Deallocate a memory block allocated by the allocate function above. This is free as default.
*/
void unrx4_free(void* ptr);

//-------------------
/**
* @brief Allocate a memory chunk for the reactive system
*/
UNREACTIVE4_API
class UNRX4SmallAllocater
{
public:
    UNRX4SmallAllocater();
    ~UNRX4SmallAllocater();

    void* allocate(unrx4::size_t size);
    void deallocate(void* ptr);

private:
    UNRX4SmallAllocater(const UNRX4SmallAllocater&) = delete;
    UNRX4SmallAllocater& operator=(const UNRX4SmallAllocater&) = delete;
    //32, 64, 96, 128, 160, 192, 224, 256
    static constexpr unrx4::size_t TableSize = 8;
    static constexpr unrx4::size_t MinChunkShift = 5;
    static constexpr unrx4::size_t MinChunkSize = 32;
    static constexpr unrx4::size_t MaxChunkSize = 256;
    static constexpr unrx4::size_t PageSize = 4096;
    static constexpr unrx4::size_t Padding = sizeof(unrx4::size_t);

    bool checkInPages(void* ptr) const;
    bool checkFreed(void* ptr) const;
    unrx4::u8* allocateFromPage(unrx4::size_t size);

    struct Chunk
    {
        Chunk* next_;
    };

    struct Page
    {
        Page* next_;
        unrx4::size_t size_;
    };

    Chunk* chunkTable_[TableSize];
    Page* pages_;
};

//-------------------
template<class T, class... Args>
T* unrx4_construct(Args&&... args)
{
    void* ptr = unrx4_malloc(sizeof(T));
    return new(ptr) T(std::forward<Args>(args)...);
}

template<class T>
void unrx4_destruct(T* ptr)
{
    if(nullptr == ptr) {
        return;
    }
    ptr->~T();
    unrx4_free(ptr);
}

//-------------------
template<class T>
struct unrx4_delete
{
    constexpr unrx4_delete() noexcept = default;
    template<class U>
    unrx4_delete(const unrx4_delete<U>&) noexcept
    {
        //static_assert(std::is_convertible<U*, T*>::value, "U should be convertible to T.");
    }
    ~unrx4_delete() = default;

    void operator()(T* ptr) const;
};

template<class T>
void unrx4_delete<T>::operator()(T* ptr) const
{
    unrx4_destruct(ptr);
}

template<class T>
struct unrx4_delete<T[]>
{
    unrx4_delete() = delete;
};

template<class T>
using unrx4_unique_ptr = TUniquePtr<T, unrx4_delete<T>>;

template<class T, class... Args>
unrx4_unique_ptr<T> unrx4_make_unique(Args&&... args)
{
    return unrx4_unique_ptr<T>(unrx4_construct<T, Args...>(std::forward<Args>(args)...));
}

template<class T>
unrx4_unique_ptr<T> unrx4_make_unique(unrx4::size_t size) = delete;

//-------------------
template<class T>
class UNRX4Function;

template<class R, class... Args>
class UNRX4Function<R(Args...)>
{
public:
    using this_type = UNRX4Function<R(Args...)>;
    using result_type = R;

    UNRX4Function()
        : holder_(nullptr)
    {
    }

    UNRX4Function(this_type&& other)
        : holder_(other.holder_)
    {
        other.holder_ = nullptr;
    }

    template<class F>
    UNRX4Function(F f)
        : holder_(allocate(f))
    {
    }

    template<class T, class F>
    UNRX4Function(T* target, F f)
        : holder_(allocate(target, f))
    {
    }

    ~UNRX4Function()
    {
        deallocate();
    }

    this_type& operator=(this_type&& other);
    this_type& operator=(std::nullptr_t);

    explicit operator bool() const
    {
        return nullptr != holder_;
    }

    result_type operator()(Args... args) const;

    template<class T, class F>
    void bind(T* target, F f)
    {
        deallocate();
        holder_ = allocate(target, f);
    }

private:
    UNRX4Function(const UNRX4Function&) = delete;
    UNRX4Function& operator=(const UNRX4Function&) = delete;

    struct holder
    {
        virtual ~holder()
        {
        }

        virtual result_type invoke(Args...) = 0;
    };

    template<class F>
    struct function_holder: public holder
    {
        function_holder(F f)
            : f_(f)
        {
        }

        virtual result_type invoke(Args... args) override
        {
            return f_(std::forward<Args>(args)...);
        }

        F f_;
    };

    template<class R, class T, class... Args>
    struct member_holder: public holder
    {
        member_holder(T* target, R (T::*f)(Args...))
            : target_(target)
            , f_(f)
        {
        }

        virtual result_type invoke(Args... args) override
        {
            return (target_->*f_)(std::forward<Args>(args)...);
        }

        T* target_;
        R(T::*f_)
        (Args...);
    };

    template<class F>
    static holder* allocate(F f)
    {
        using function_holder_type = function_holder<F>;
        void* ptr = unrx4_malloc(sizeof(function_holder_type));
        return new(ptr) function_holder_type(f);
    }

    template<class R, class T, class... Args>
    static holder* allocate(T* target, R (T::*f)(Args...))
    {
        using member_holder_type = member_holder<R, T, Args...>;
        void* ptr = unrx4_malloc(sizeof(member_holder_type));
        return new(ptr) member_holder_type(target, f);
    }

    void deallocate();

    holder* holder_;
};

template<class R, class... Args>
R UNRX4Function<R(Args...)>::operator()(Args... args) const
{
    UNRX4_ASSERT(nullptr != holder_);
    return holder_->invoke(std::forward<Args>(args)...);
}

template<class R, class... Args>
UNRX4Function<R(Args...)>& UNRX4Function<R(Args...)>::operator=(this_type&& other)
{
    if(this == &other) {
        return *this;
    }
    deallocate();
    holder_ = other.holder_;
    other.holder_ = nullptr;
    return *this;
}

template<class R, class... Args>
UNRX4Function<R(Args...)>& UNRX4Function<R(Args...)>::operator=(std::nullptr_t)
{
    if(this != &other) {
        deallocate();
    }
    return *this;
}

template<class R, class... Args>
void UNRX4Function<R(Args...)>::deallocate()
{
    unrx4_destruct(holder_);
    holder_ = nullptr;
}

using UNRX4Action = UNRX4Function<void()>;

/**
 * @brief Function type for reacting to something
 * @tparam T 
*/
template<class T>
using UNRX4ReactFunc = UNRX4Function<void(T)>;

/**
 * @brief Function type for receiving error
*/
using UNRX4ErrorFunc = UNRX4Function<void(unrx4::error_code_type)>;

/**
 * @brief Function type for receiving a completed message
*/
using UNRX4CompleltedFunc = UNRX4Function<void()>;

//-------------------
template<class T>
class UNRX4Handle;

template<class T>
class UNRX4Handle<T*>
{
public:
    UNRX4Handle()
        : pointer_(nullptr)
    {
    }

    UNRX4Handle(T* pointer)
        : pointer_(pointer)
    {
    }

    ~UNRX4Handle() = default;

    T* operator->()
    {
        return pointer_;
    }

    T& operator*()
    {
        return pointer_;
    }

private:
    T* pointer_;
};
