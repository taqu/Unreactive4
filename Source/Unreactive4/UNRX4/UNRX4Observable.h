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
 * @file UNRX4Observable.h
 * @author t-sakai
 */
// clang-format on
#include "UNRX4Container.h"
#include "UNRX4IObservable.h"
#include "UNRX4IObserver.h"

//-------------------
template<class T>
class UNRX4ObservableOnce: public UNRX4IObservable<T>
{
public:
    UNRX4ObservableOnce(T value);
    virtual ~UNRX4ObservableOnce() {}
    virtual void subscribe(UNRX4IObserver<T>* observer) override;
    virtual void unsubscribe(UNRX4IObserver<T>* /*observer*/) override{}
    virtual void next(T) override {}
    virtual void error(unrx4::error_code_type /*errorCode*/) override {}
    virtual void completed() override {}

private:
    T value_;
};

template<class T>
UNRX4ObservableOnce<T>::UNRX4ObservableOnce(T value)
    : value_(value)
{
}

template<class T>
void UNRX4ObservableOnce<T>::subscribe(UNRX4IObserver<T>* observer)
{
    observer->next(value_);
    observer->completed();
}

//-------------------
template<class T>
class UNRX4ObservableRepeat: public UNRX4IObservable<T>
{
public:
    UNRX4ObservableRepeat(unrx4::u32 count, T value);
    virtual ~UNRX4ObservableRepeat() {}
    virtual void subscribe(UNRX4IObserver<T>* observer) override;
    virtual void unsubscribe(UNRX4IObserver<T>* /*observer*/) override{}
    virtual void next(T) override {}
    virtual void error(unrx4::error_code_type /*errorCode*/) override {}
    virtual void completed() override {}

private:
    unrx4::u32 count_;
    T value_;
};

template<class T>
UNRX4ObservableRepeat<T>::UNRX4ObservableRepeat(unrx4::u32 count, T value)
    : count_(count)
    , value_(value)
{
}

template<class T>
void UNRX4ObservableRepeat<T>::subscribe(UNRX4IObserver<T>* observer)
{
    for(unrx4::u32 i = 0; i < count; ++i) {
        observer->next(value_);
    }
    observer->completed();
}

//-------------------
template<class... Args>
class UNRX4ObservableFromEvent: public UNRX4IObservable<Args...>
{
public:
    using this_type = UNRX4ObservableFromEvent<Args...>;
    using delegate_type = UNRX4Function<void(Args...)>;
    using observer_type = UNRX4IObserver<Args...>;

    UNRX4ObservableFromEvent(UNRX4Function<void(Args...)>& handler)
    {
        handler.bind(this, &this_type::next);
    }

    virtual ~UNRX4ObservableFromEvent() {}

    virtual void subscribe(UNRX4IObserver<Args...>* observer) override;
    virtual void unsubscribe(UNRX4IObserver<Args...>* observer) override;
    virtual void next(Args... args) override;
    virtual void error(unrx4::error_code_type errorCode) override;
    virtual void completed() override;

private:
    UNRX4Array<observer_type*> observers_;
};

template<class... Args>
void UNRX4ObservableFromEvent<Args...>::subscribe(UNRX4IObserver<Args...>* observer)
{
    observers_.push_back(observer);
}

template<class... Args>
void UNRX4ObservableFromEvent<Args...>::unsubscribe(UNRX4IObserver<Args...>* observer)
{
    observers_.remove(observer);
}

template<class... Args>
void UNRX4ObservableFromEvent<Args...>::next(Args... args)
{
    for(observer_type* observer: observers_) {
        observer->next(std::forward<Args>(args)...);
    }
}

template<class... Args>
void UNRX4ObservableFromEvent<Args...>::error(unrx4::error_code_type errorCode)
{
    for(observer_type* observer: observers_) {
        observer->error(errorCode);
    }
}

template<class... Args>
void UNRX4ObservableFromEvent<Args...>::completed()
{
    for(observer_type* observer: observers_) {
        observer->completed();
    }
}

//-------------------
class UNRX4Observable
{
public:
    template<class T>
    static unrx4_unique_ptr<UNRX4IObservable<T>> once(T value);

    template<class T>
    static unrx4_unique_ptr<UNRX4IObservable<T>> repeat(unrx4::u32 count, T value);

    template<class... Args>
    static unrx4_unique_ptr<UNRX4IObservable<Args...>> fromEvent(UNRX4Function<void(Args...)>& eventHandler);
};

template<class T>
unrx4_unique_ptr<UNRX4IObservable<T>> UNRX4Observable::once(T value)
{
    return unrx4_make_unique<UNRX4ObservableOnce<T>>(value);
}

template<class T>
unrx4_unique_ptr<UNRX4IObservable<T>> UNRX4Observable::repeat(unrx4::u32 count, T value)
{
    return unrx4_make_unique<UNRX4ObservableRepeat<T>>(count, value);
}

template<class... Args>
unrx4_unique_ptr<UNRX4IObservable<Args...>> UNRX4Observable::fromEvent(UNRX4Function<void(Args...)>& eventHandler)
{
    return unrx4_make_unique<UNRX4ObservableFromEvent<Args...>>(eventHandler);
}
