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
 * @file UNRX4IObservable.h
 * @author t-sakai
 */
// clang-format on
#include "Unreactive4.h"

template<class... Args>
class UNRX4IObserver;

/**
 * @brief React to events which Observers generate
 */
template<class... Args>
class UNRX4IObservable
{
public:
    virtual ~UNRX4IObservable() {}
    virtual void subscribe(UNRX4IObserver<Args...>* observer) = 0;
    virtual void unsubscribe(UNRX4IObserver<Args...>* observer) = 0;
    virtual void next(Args... value) = 0;
    virtual void error(unrx4::error_code_type errorCode) = 0;
    virtual void completed() = 0;
protected:
    UNRX4IObservable(){}
};
