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
 * @file UNRX4System.h
 * @author t-sakai
 */
// clang-format on
#include "UNRX4.h"

//-------------------
class UNRX4ImmediateScheduler;
class UNRX4CurrentThreadScheduler;

UNREACTIVE4_API
class UNRX4System
{
public:
    static UNRX4System& getInstance();

    void* allocate(unrx4::size_t size);
    void deallocate(void* ptr);

    UNRX4ImmediateScheduler& immediateScheduler();
    UNRX4CurrentThreadScheduler& currentThreadScheduler();

private:
    UNRX4System(const UNRX4System&) = delete;
    UNRX4System& operator=(const UNRX4System&) = delete;

    static UNRX4System instance_;

    UNRX4System();
    ~UNRX4System();

    UNRX4SmallAllocater allocator_;
};
