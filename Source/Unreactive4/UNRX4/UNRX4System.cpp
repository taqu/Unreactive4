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
 * @file UNRX4System.cpp
 * @author t-sakai
 */
// clang-format on
#include "UNRX4System.h"
#include "UNRX4ImmediateScheduler.h"
#include "UNRX4CurrentThreadScheduler.h"

//-------------------
static UNRX4ImmediateScheduler unrx4_internal_immediateScheduler_;
static UNRX4CurrentThreadScheduler unrx4_internal_currentThreadScheduuler_;

UNRX4System UNRX4System::instance_;

UNRX4System::UNRX4System()
{
}

UNRX4System::~UNRX4System()
{
}

UNRX4System& UNRX4System::getInstance()
{
    return instance_;
}

void* UNRX4System::allocate(size_t size)
{
    return allocator_.allocate(size);
}

void UNRX4System::deallocate(void* ptr)
{
    allocator_.deallocate(ptr);
}

UNRX4ImmediateScheduler& UNRX4System::immediateScheduler()
{
    return unrx4_internal_immediateScheduler_;
}

UNRX4CurrentThreadScheduler& UNRX4System::currentThreadScheduler()
{
    return unrx4_internal_currentThreadScheduuler_;
}

