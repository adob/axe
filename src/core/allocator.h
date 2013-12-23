#pragma once
#include "typedefs.h"
#include "print.h"
namespace memory {
    
    struct LargeAllocHeader {
        LargeAllocHeader    *prev;
        LargeAllocHeader    *next;
    };
    
    struct BlockHeader {
        uint                  offset;
        BlockHeader          *next;
        uint8                 failedcnt;
    };
    
    struct Region {
        BlockHeader          first_block;
        uint16               blocksize;
        uint16               max_alloc_size;
        uint16               last_alloc_offset;
        
        BlockHeader         *current_block;
        BlockHeader         *last_block;
        LargeAllocHeader    *large_alloc_list;
        
        
        void *alloc(size size, unsigned allignment=16);
        void *extend(void *ptr, size oldsize, size newsize, unsigned allignment=16);
        void destroy();
        bool free(void *ptr); 
        
        void *alloc_block(size size, unsigned allignment);
        void *alloc_large(size size, unsigned allignment);
        
        
    };
    
    Region *create_region(uint16 blocksize);
}

template <typename T>
struct AllocatorRef {
    using value_type = T;
    memory::Region& region;
    
    AllocatorRef(memory::Region& region) : region(region) {}
    
    T *allocate(size n) {
        print "allocate(%d)" % n;
        return (T*) region.alloc(n * sizeof(T), alignof(T));
    }
    
    bool deallocate(void *ptr, size n) {
        print "deallocate(%x, %d)" % ptr, n;
        return region.free(ptr);
    }
};


struct Allocator {
    memory::Region *region;
    
    Allocator(size sz=256) {
        print "Allocator";
        region = memory::create_region(sz);
    }
    
    Allocator(Allocator const&) = delete;
    
    template<typename T>
    AllocatorRef<T> allocator() {
        return AllocatorRef<T>(*region);
    }
    
    Allocator& operator = (Allocator const&) = delete;
    
    bufref operator () (size n) {
        return bufref((char*)region->alloc(n), n);
    }
    
    ~Allocator() {
        print "~Allocator";
        region->destroy();
    }
    
};