
namespace axe {

    struct bufref;
    struct strref;

    namespace memory {
        
        struct LargeAllocHeader {
            LargeAllocHeader    *next;
            byte                 data[];
        };
        
        struct BlockHeader {
            byte                *start;
            byte                *end;
            BlockHeader         *next;
            uint8                failedcnt;
            byte                 data[];
        };
        
        struct Region {
            size                blocksize;
            size                max_alloc_size;
            //uint16               last_alloc_offset;
            
            BlockHeader         *first_block;
            BlockHeader         *current_block;
            //BlockHeader         *last_block;
            LargeAllocHeader    *large_alloc_list;
            
            Region(uint16 blocksze);
            
            void *alloc(size size, unsigned allignment=16);
            //void *extend(void *ptr, size oldsize, size newsize, unsigned allignment=16);
            void destroy();
            bool free(void *ptr); 
            
            void *alloc_block(size size, unsigned allignment);
            void *alloc_large(size size, unsigned allignment);
            
            ~Region();
        };
        
        Region *create_region(uint16 blocksize);
    }

    template <typename T>
    struct AllocatorRef {
        using value_type = T;
        memory::Region& region;
        
        AllocatorRef(memory::Region& region) : region(region) {}
        
        T *allocate(size n) {
            //print "allocate(%d)" % n;
            return (T*) region.alloc(n * sizeof(T), alignof(T));
        }
        
        bool deallocate(void *ptr, size n) {
            //print "deallocate(%x, %d)" % ptr, n;
            return region.free(ptr);
        }
    };


    struct Allocator {
        memory::Region region;
        
        Allocator(size sz=512);
        Allocator(Allocator const&) = delete;
        
        template<typename T>
        AllocatorRef<T> allocator() {
            return AllocatorRef<T>(region);
        }
        
        Allocator& operator = (Allocator const&) = delete;
        
        bufref operator () (size n);
        bufref operator () (strref s);
        
        template <typename T, typename... Args>
        T* make(Args&&... args);
        
        template <typename T, typename... Args>
        T* make_many(size count, Args&&... args);
        
        bufref copy_c_str(const char *charp);
    };

    #include "alloc.inlines.h"
}