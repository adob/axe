#include <malloc.h>
#import <axe/print.h>

#import "PKG.h"

using namespace axe;

namespace {
    byte* align(byte *d, size a) { return (byte*) (((size)(d + a - 1)) & ~(a - 1)); }
    constexpr size  align(size  d, size a) { return                 (d + a - 1)  & ~(a - 1); }
}

namespace axe {
namespace memory {

const size PageSize              = 4096;
const uint16 MaxAllocSize        = PageSize - 1;
const unsigned MaxFail           = 4;
const unsigned MaxLargeAllocFail = 3;

Region::Region(uint16 blocksize) {
    //print "Region::Region(%d)" % blocksize, this;
    this->blocksize              = blocksize;
    uint16 usable_size           = blocksize - align(offsetof(BlockHeader, data), 16);
    this->max_alloc_size         = std::min(usable_size, MaxAllocSize);
    //this->last_alloc_offset      = 0;
    this->first_block            = nullptr;
    this->current_block          = nullptr;
    this->large_alloc_list       = nullptr;
    //this->last_block             = nullptr;
}
    
    
void *Region::alloc(size allocsize, unsigned alignment) {
//     print "Region::alloc(%s, %s)" % allocsize, alignment;
    if (allocsize == 0) {
        return nullptr;
    }
    if (allocsize <= max_alloc_size) {
        BlockHeader *block = current_block;
        while (block) {
            byte *mem = align(block->start, alignment);
            
            if (mem + allocsize <= block->end) {
                //last_alloc_offset = align(block->offset, alignment);
                //last_block = block;
                block->start = mem + allocsize;
                return mem;
            }
            
            block = block->next;
        }
        
        return this->alloc_block(allocsize, alignment);
    }
    return this->alloc_large(allocsize, alignment);
}

// void *Region::extend(void *ptr, size oldsize, size newsize, unsigned alignment) {
//     //print "Region::extend()";
//     // check if last loc was a large alloc
//     // if so we can just call std::realloc()
//     if (last_alloc_offset == (uint16) -1) {
//         size padding = align(sizeof(LargeAllocHeader), alignment);
//         
//         if (((byte*)ptr) - padding != (byte*) large_alloc_list) {
//             assert(false);
//             goto alloc_mem;
//         }
//         
//         byte * mem = (byte*) std::realloc(large_alloc_list, newsize+padding);
//         //print "realloc(%d, %d) -> %x" % large_alloc_list, newsize+padding, mem;
//         if (mem == nullptr) {
//             return nullptr;
//         }
//         if (mem != (byte*) large_alloc_list) {
//             LargeAllocHeader *l = (LargeAllocHeader *) mem;
//             large_alloc_list = l;
//         }
//         return mem + padding;
//     } 
//     
//     // otherwise the last alloc was a regular alloc
//     
//     // check that ptr is the expected value
//     if (((byte*)last_block) + last_alloc_offset != ptr) {
//         //print "expected %x but got ptr=%x" % (((byte*)last_block) + last_alloc_offset), ptr;
//         assert(false);
//         goto alloc_mem;
//     }
//     
//     // check if we have enough room to extend
//     if ( (size) (blocksize - last_alloc_offset) >= newsize) {
//         ptrdiff_t extrasize = newsize-oldsize; // could be negative
//         last_block->offset += extrasize;
//         void *mem = ((byte*)last_block) + last_alloc_offset;
//         return mem;
//     }
//     
//     // did not fit, undo previous allocation
//     last_block->offset -= oldsize;    
//     
//     alloc_mem:
//     // new allocation
//     byte *newmem = (byte *) alloc(newsize, alignment);
//     memcpy(newmem, ptr, oldsize);
//     return newmem;
// }

void *Region::alloc_block(size allocsize, unsigned alignment) {
    //print "Region::alloc_block()";
//     this->blocksize              = 2 * blocksize;
//     uint16 usable_size           = blocksize - align(offsetof(BlockHeader, data), 16);
//     this->max_alloc_size         = std::min(usable_size, MaxAllocSize);
    
    byte *mem = (byte*) memalign(alignof(BlockHeader), blocksize);
    if (mem == nullptr) {
        return nullptr;
    }
    
    BlockHeader *newblock = (BlockHeader *) mem;
    
    newblock->end       = mem + blocksize;
    newblock->next      = nullptr;
    newblock->failedcnt = 0;
    
    mem = mem + offsetof(BlockHeader, data);
    mem = align(mem, alignment);
    
    newblock->start     = mem + allocsize;
    
    if (!first_block) {
        first_block = current_block = newblock;
    } else {
        BlockHeader *block;
        for (block = current_block; block->next != nullptr; block = block->next) {
            if (block->failedcnt++ > MaxFail) {
                current_block = block->next;
            }
        }
        
        block->next = newblock;
        if (!current_block) {
            current_block = newblock;
        }
        
    }
    //last_block        = newblock;
    //last_alloc_offset = align(offsetof(BlockHeader, data), alignment);

    
    return mem;

}

void *Region::alloc_large(size allocsize, unsigned alignment) {
    //print "Region::alloc_large()";
    size padding = align(sizeof(LargeAllocHeader), alignment); // bug
    byte *mem = (byte *) std::malloc(allocsize+padding);
    //print "malloc(%d) -> %x" % (allocsize+padding), mem;
    if (mem == nullptr) {
        return nullptr;
    }
    
    LargeAllocHeader *header = (LargeAllocHeader*) mem;
    
    header->next           = large_alloc_list;
    large_alloc_list       = header;
    
    //last_alloc_offset = (uint16) -1;
    return mem + padding;
}

void Region::destroy() {
    //print "Region::destroy()", this;
    LargeAllocHeader *l = large_alloc_list;
    while (l) {
        //print "freed large %x" % l;
        LargeAllocHeader *ll = l;
        l = l->next;
        std::free(ll);
    }
    
    BlockHeader *h = first_block;
    while (h) {
        BlockHeader *hh = h;
        h = h->next;
        //print "freed block %x" % hh;
        std::free(hh);
    };
    
}

bool Region::free(void *ptr) {
    //print "Region::free()";
    LargeAllocHeader *l = large_alloc_list;
    while (l) {
        void *userp = ((byte*) l) + offsetof(LargeAllocHeader, data);
        if (userp == ptr) {
            std::free(l);
            return true;
        }
        l = l->next;
    }
    return false;
}

Region::~Region() {
    destroy();
}

} // namespace memory

Allocator::Allocator(size sz) : region(sz) {
    //print "Allocator";
}

bufref Allocator::operator () (size n) {
    
    return bufref((char*)region.alloc(n, 1), n);
}

bufref Allocator::operator () (str s) {
    bufref b = (*this)(len(s));
    memcpy(b.data, s.data, s.len);
    return b;
}

bufref Allocator::copy_c_str(const char *charp) {
    if (!charp)
        return bufref();
    
    bufref b = (*this)(strlen(charp));
    memcpy(b.data, charp, b.len);
    return b;
}
} // namespace axe 