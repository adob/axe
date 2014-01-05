// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>
//#include <ostream>
// #include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <functional>
#include <tuple>
#include <utility>
#include <array>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <sched.h>
#include <math.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <condition_variable>

#include <axe/core.h>
#include <axe/unicode/utf8.h>
#include <axe/strconv.h>
#include <axe/fmt.h>
#include <axe/net.h>
#include <axe/time.h>
#include <axe/print.h>
#include <axe/csp.h>
#include <axe/debug.h>

using namespace axe;

void workfun(int i) {
    print "work(%d) start" % i, std::time(0);
    
//     int128 now = time::monotonic();
//     int128 end = now + 5 * std::nano::den;
//     
//     while (now < end) {
//         now = time::monotonic();
//     }
    
    void (*func)() = nullptr;
    //func();
    sleep(4);
    //print a;
    print "work(%d) end" % i, std::time(0);
    throw "foo";
}

// struct MyStruct {
//     char buf[4] = { 'x', 'y', 'z', '!' };
//     
//     std::function<char&(bool&)> generator() {
//         char * cur = buf+0;
//         char * end = buf+sizeof(buf);
//         return [=](bool& valid) mutable -> char& {
//             if (cur >= end)
//                 return valid=false, * (char*) 0;
// 
//             return valid=true, *(cur++);
//         };
//     }
// } ;
// 
// int ints[] = { 1, 2, 3 };
// 
// #include <type_traits>
// 
// template <typename T>
// struct Iterator {
//     using GeneratorType  = decltype( std::declval<T>().generator() );
//     using ResultType = decltype( std::declval<GeneratorType>()( std::declval<bool&>() ) );
//     
//     
//     GeneratorType gen;
//     ResultType curr;
//     
//     
//     bool valid;
//     
//     Iterator(T & t) : gen(t.generator()), curr(gen(valid)) {}
//     
//     //Iterator() : blank1(true), blank2(true) {}
//     
//     ResultType operator * () {
//         return curr;
//     }
//     
//     void operator ++ () {
//         curr = gen(valid);
//     }
//     
//     bool operator != (Iterator<T> const&) {
//         return valid;
//     }
//     
//     Iterator(const Iterator<T>& other) = delete;
//     ~Iterator() {
//         print "destructor", this;
//     }
// } ;

// int foo() {
//     return 3;
// }
// 
// struct Foo {
//     int operator() () { return 3; }
// } ;
// 
// template <typename T>
// Iterator<T> begin(T & t) {
//     return Iterator<T>(t);
// }
// 
// template <typename T>
// Iterator<T>& end(T &) {
//     return * (Iterator<T>*) 0;
//     //return Iterator<T>();
// }

// #include <signal.h>
// #include <stdio.h>
// 
// int *a;
// 
// class FoobarException
// {
//     int thingy;
// };
// 
// void signal_handler(int signum, siginfo_t *info, void *)
// {
//     print "got exception";
//     FoobarException *f = new FoobarException;
//     throw f;
// }
// 
// int main()
// {
//     struct sigaction act;
//     
//     act.sa_sigaction = signal_handler;
//     sigemptyset (&act.sa_mask);
//     act.sa_flags = SA_SIGINFO;
//     sigaction (11, &act, NULL);
//     
//     try
//     {
//         //void (*func)() = nullptr;
//         const char *str = 0;
//         char c = *str;
//         //printf("%d\n", *a);
//     }
//     catch (FoobarException *f)
//     {
//         printf("Hello!\n");
//     }
// }

int main() {
    debug::init();
    //csp::init();
    //MyStruct ms;
    
    try {
        //void (*func)() = nullptr;
        //func();
        //int i = strlen(nullptr);
        //int i = 3/0;
        const char * s = 0;
        char c = *s;
        print c;
    } catch (...) {
        print "got exception";
    }
    
    
    print "hello";
//     csp::go([]{
//         workfun(7);
//     });
    
    print "goodbye";
    
    sleep(500);
    
    return 0;
}