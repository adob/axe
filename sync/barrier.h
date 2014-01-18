namespace axe {
    namespace sync {
        struct Barrier {
            pthread_barrier_t barr;
            
            Barrier(unsigned count);
            void wait();
            ~Barrier();
        } ;
    }
}