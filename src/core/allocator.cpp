#include <malloc.h>

#include "builtin.h"
#include "allocator.h"

namespace {
    constexpr size allign(size d, size a) { return (d + a - 1) & ~(a - 1); }
}

namespace memory {
    const size PageSize     = 4096;
    const uint16 MaxAllocSize = PageSize - 1;
    const unsigned MaxFail  = 4;
    const unsigned MaxLargeAllocFail = 3;

Region *create_region(uint16 blocksize) {
    print "create_region(%s)" % blocksize;
    Region *r = (Region*) memalign(alignof(Region), blocksize);
    if (r == nullptr) {
        return nullptr;
    }
    
    r->first_block.offset     = sizeof(Region);
    r->first_block.next       = nullptr;
    r->first_block.failedcnt  = 0;
    
    r->blocksize              = blocksize;
    r->last_alloc_offset      = 0;
    r->current_block          = &r->first_block;
    r->large_alloc_list       = nullptr;
    r->last_block             = &r->first_block;
    
    uint16 usable_size   = blocksize - sizeof(Region);
    r->max_alloc_size    = std::min(usable_size, MaxAllocSize);
    
    return r;
}
    
    
void *Region::alloc(size allocsize, unsigned allignment) {
    print "Region::alloc(%s, %s)" % allocsize, allignment;
    if (allocsize <= max_alloc_size) {
        BlockHeader *block = current_block;
        do {
            void *mem = (void*) allign(((size) block) + block->offset, allignment);
            
            if (blocksize - block->offset >= allocsize) {
                last_alloc_offset = allign(block->offset, allignment);
                last_block = block;
                block->offset += allocsize;
                return mem;
            }
            
            block = block->next;
        } while (block);
        
        return this->alloc_block(allocsize, allignment);
    }
    return this->alloc_large(allocsize, allignment);
}

void *Region::extend(void *ptr, size oldsize, size newsize, unsigned allignment) {
    print "Region::extend()";
    // check if last loc was a large alloc
    // if so we can just call std::realloc()
    if (last_alloc_offset == (uint16) -1) {
        size padding = allign(sizeof(LargeAllocHeader), allignment);
        
        if (((byte*)ptr) - padding != (byte*) large_alloc_list) {
            assert(false);
            goto alloc_mem;
        }
        
        byte * mem = (byte*) std::realloc(large_alloc_list, newsize+padding);
        print "realloc(%d, %d) -> %x" % large_alloc_list, newsize+padding, mem;
        if (mem == nullptr) {
            return nullptr;
        }
        if (mem != (byte*) large_alloc_list) {
            LargeAllocHeader *l = (LargeAllocHeader *) mem;
            if (l->next)
                l->next->prev = l;
            if (l->prev)
                l->prev->next = l;
            large_alloc_list = l;
        }
        return mem + padding;
    } 
    
    // otherwise the last alloc was a regular alloc
    
    // check that ptr is the expected value
    if (((byte*)last_block) + last_alloc_offset != ptr) {
        print "expected %x but got ptr=%x" % (((byte*)last_block) + last_alloc_offset), ptr;
        assert(false);
        goto alloc_mem;
    }
    
    // check if we have enough room to extend
    if ( (size) (blocksize - last_alloc_offset) >= newsize) {
        ptrdiff_t extrasize = newsize-oldsize; // could be negative
        last_block->offset += extrasize;
        void *mem = ((byte*)last_block) + last_alloc_offset;
        return mem;
    }
    
    // did not fit, undo previous allocation
    last_block->offset -= oldsize;    
    
    alloc_mem:
    // new allocation
    byte *newmem = (byte *) alloc(newsize, allignment);
    memcpy(newmem, ptr, oldsize);
    return newmem;
}

void *Region::alloc_block(size allocsize, unsigned allignment) {
    print "Region::alloc_block()";
    void *mem = memalign(alignof(BlockHeader), blocksize);
    if (mem == nullptr) {
        return nullptr;
    }
    
    BlockHeader *newblock = (BlockHeader *) mem;
    
    newblock->offset    = sizeof(BlockHeader) + allocsize;
    newblock->next      = nullptr;
    newblock->failedcnt = 0;
    
    mem = ((byte*)mem) + sizeof(BlockHeader);
    mem = (void *) allign((size) mem, allignment);
    
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
    last_block        = newblock;
    last_alloc_offset = sizeof(BlockHeader);
    
    return mem;

}

void *Region::alloc_large(size allocsize, unsigned allignment) {
    print "Region::alloc_large()";
    size padding = allign(sizeof(LargeAllocHeader), allignment);
    byte *mem = (byte *) std::malloc(allocsize+padding);
    print "malloc(%d) -> %x" % (allocsize+padding), mem;
    if (mem == nullptr) {
        return nullptr;
    }
    
    LargeAllocHeader *header = (LargeAllocHeader*) mem;
    
    header->prev           = nullptr;
    header->next           = large_alloc_list;
    if (large_alloc_list) {
        large_alloc_list->prev = header;
    }
    large_alloc_list       = header;
    
    last_alloc_offset = (uint16) -1;
    return mem + padding;
}

void Region::destroy() {
    print "Region::destroy()";
    LargeAllocHeader *l = large_alloc_list;
    while (l) {
        print "freed large %x" % l;
        LargeAllocHeader *ll = l;
        l = l->next;
        std::free(ll);
    }
    
    BlockHeader *h = &first_block;
    do {
        BlockHeader *hh = h;
        h = h->next;
        print "freed block %x" % hh;
        std::free(hh);
    } while (h);
    
}

bool Region::free(void *ptr) {
    print "Region::free()";
    LargeAllocHeader *l = large_alloc_list;
    while (l) {
        void *userp = ((byte*) l) + sizeof(LargeAllocHeader);
        if (userp == ptr) {
            std::free(l);
            return true;
        }
        l = l->next;
    }
    return false;
}

}