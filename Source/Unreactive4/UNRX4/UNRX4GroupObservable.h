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
 * @file UNRX4GroupObservable.h
 * @author t-sakai
 */
// clang-format on
#include "UNRX4Container.h"
#include "UNRX4IObservable.h"
#include "UNRX4IObserver.h"

/**
 */
template<class T>
class UNRX4GroupObservable: public UNRX4IObservable<T>
{
public:
    using pass_type = UNRX4IObservable<T>::pass_type;
    using observer_type = UNRX4IObserver<T>;

    UNRX4GroupObservable();
    UNRX4GroupObservable(UNRX4GroupObservable&& other);
    virtual ~UNRX4GroupObservable();
    virtual void subscribe(observer_type* observer) override;
    virtual void dispatch(pass_type value) override;
    virtual void dispatchError(unrx4::error_code_type errorCode) override;
    virtual void completed() override;

    UNRX4GroupObservable& operator=(UNRX4GroupObservable&& other);

private:
    UNRX4Array<observer_type*> observers_;
};

template<class T>
UNRX4GroupObservable<T>::UNRX4GroupObservable()
{
}

template<class T>
UNRX4GroupObservable<T>::UNRX4GroupObservable(UNRX4GroupObservable&& other)
    : observers_(std::move(other.observers_))
{
}

template<class T>
UNRX4GroupObservable<T>::~UNRX4GroupObservable()
{
}

template<class T>
void UNRX4GroupObservable<T>::subscribe(observer_type* observer)
{
    observers_.push_back(observer);
}

template<class T>
void UNRX4GroupObservable<T>::dispatch(pass_type value)
{
    for(observer_type* observer: observers_) {
        observer->next(value);
    }
}

template<class T>
void UNRX4GroupObservable<T>::dispatchError(unrx4::error_code_type errorCode)
{
    for(observer_type* observer: observers_) {
        observer->error(errorCode);
    }
}

template<class T>
void UNRX4GroupObservable<T>::completed()
{
    for(observer_type* observer: observers_) {
        observer->completed();
    }
}

template<class T>
UNRX4GroupObservable<T>& UNRX4GroupObservable<T>::operator=(UNRX4GroupObservable<T>&& other)
{
    if(this == &other) {
        return *this;
    }
    observers_ = std::move(other.observers_);
    return *this;
}
