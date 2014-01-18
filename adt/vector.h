namespace axe {
    template <typename T>
    struct Vector {
        T         *data;
        size       len;
        size       cap;
        Allocator& alloc;
        
        Vector(Allocator& alloc)
          : data(nullptr)
          , len(0)
          , cap(0)
          , alloc(alloc) 
        {
            // empty
        }
        
        Vector(size count, Allocator &alloc)
          : len(count)
          , cap(count)
          , alloc(alloc) 
        {
            data = alloc.make_many<T>(count);
        }
        
        operator T* () {
            return data;
        }
        
        T* begin() {
            return data;
        }
        
        T* end() {
            return data+len;
        }
    } ;
    
    template <typename T>
    size len(Vector<T> const&)  {
        return vec.len;
    }
    
    template <typename T, size N>
    struct SmallVector {
        T         *arr;
        size       len;
        size       cap;
        Allocator& alloc;
        T          buf[N];
        
        SmallVector(Allocator& alloc)
          : arr(buf)
          , len(0)
          , cap(N)
          , alloc(alloc)
        {
                // empty
        }

    } ;
}