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
 * @file UNRX4.cpp
 * @author t-sakai
 */
// clang-format on
#include "UNRX4.h"
#include "UNRX4System.h"

//-------------------
void* unrx4_malloc(size_t size)
{
    return UNRX4System::getInstance().allocate(size);
}

void unrx4_free(void* ptr)
{
    UNRX4System::getInstance().deallocate(ptr);
}

//-------------------
UNRX4SmallAllocater::UNRX4SmallAllocater()
    : chunkTable_{}
    , pages_{nullptr}
{
}

UNRX4SmallAllocater::~UNRX4SmallAllocater()
{
    Page* page = pages_;
    while(nullptr != page) {
        Page* nextPage = page->next_;
        unrx4_free(page);
        page = nextPage;
    }
}

void* UNRX4SmallAllocater::allocate(unrx4::size_t size)
{
    size += Padding;
    unrx4::size_t padded = (size + 31U) & ~31U;
    unrx4::u8* ptr;
    if(MaxChunkSize < padded) {
        ptr = reinterpret_cast<unrx4::u8*>(FMemory::Malloc(padded));
        *reinterpret_cast<unrx4::size_t*>(ptr) = padded;
        return ptr + Padding;
    }
    unrx4::size_t index = (padded >> MinChunkShift) - 1;
    UNRX4_ASSERT(index < TableSize);
    UNRX4_ASSERT(((index + 1) * MinChunkSize) == padded);
    if(nullptr != chunkTable_[index]) {
        ptr = reinterpret_cast<unrx4::u8*>(chunkTable_[index]);
        chunkTable_[index] = chunkTable_[index]->next_;
    } else {
        ptr = allocateFromPage(padded);
    }

    *reinterpret_cast<unrx4::size_t*>(ptr) = padded;
    ptr += Padding;
    return ptr;
}

void UNRX4SmallAllocater::deallocate(void* ptr)
{
    if(nullptr == ptr) {
        return;
    }
    unrx4::u8* ptru8 = reinterpret_cast<unrx4::u8*>(ptr);
    ptru8 -= Padding;
    unrx4::size_t size = *reinterpret_cast<unrx4::size_t*>(ptru8);
    if(MaxChunkSize < size) {
        FMemory::Free(ptru8);
        return;
    }
    UNRX4_ASSERT(checkInPages(ptr));
    UNRX4_ASSERT(checkFreed(ptr));
    unrx4::size_t index = (size >> MinChunkShift) - 1;
    UNRX4_ASSERT(index < TableSize);
    UNRX4_ASSERT(((index + 1) * MinChunkSize) == size);
    Chunk* chunk = reinterpret_cast<Chunk*>(ptru8);
    chunk->next_ = chunkTable_[index];
    chunkTable_[index] = chunk;
}

bool UNRX4SmallAllocater::checkInPages(void* ptr) const
{
    unrx4::uintptr_t address = reinterpret_cast<unrx4::uintptr_t>(ptr);
    Page* page = pages_;
    while(nullptr != page) {
        unrx4::uintptr_t start = reinterpret_cast<unrx4::uintptr_t>(page);
        unrx4::uintptr_t end = start + PageSize;
        if(start <= address && address < end) {
            return true;
        }
        page = page->next_;
    }
    return false;
}

bool UNRX4SmallAllocater::checkFreed(void* ptr) const
{
    unrx4::uintptr_t address = reinterpret_cast<unrx4::uintptr_t>(ptr);
    for(unrx4::size_t i = 0; i < TableSize; ++i) {
        unrx4::size_t size = MinChunkSize * (i + 1);
        Chunk* chunk = chunkTable_[i];
        while(nullptr != chunk) {
            unrx4::uintptr_t start = reinterpret_cast<unrx4::uintptr_t>(chunk);
            unrx4::uintptr_t end = start + size;
            if(start <= address && address < end) {
                return false;
            }
            chunk = chunk->next_;
        }
    }
    return true;
}

unrx4::u8* UNRX4SmallAllocater::allocateFromPage(unrx4::size_t size)
{
    unrx4::u8* ptr;
    Page* page = pages_;
    while(nullptr != page) {
        if(size <= page->size_) {
            ptr = reinterpret_cast<uint8_t*>(page) + PageSize - page->size_;
            page->size_ -= size;
            return ptr;
        }
        page = page->next_;
    }
    Page* newPage = reinterpret_cast<Page*>(FMemory::Malloc(PageSize));
    newPage->size_ = PageSize - MinChunkSize;
    newPage->next_ = pages_;
    pages_ = newPage;

    ptr = reinterpret_cast<unrx4::u8*>(newPage) + PageSize - newPage->size_;
    newPage->size_ -= size;
    return ptr;
}

