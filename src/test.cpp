// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>
//#include <ostream>
// #include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdio_ext.h>
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
#include <axe/sync.h>
#include <axe/debug.h>

using namespace axe;

void workfun(int i) {
    print "work(%d) start" % i, std::time(0);
    
    //void (*func)() = nullptr;
    sleep(4);
    print "work(%d) end" % i, std::time(0);
    throw "foo";
}




int main() {
    print typeid(3);
    debug::init();
    sync::init();
/*    
    const S1 s;
    testr(S1());
    */
    
    
    
    sync::Chan<int> chan1;
    sync::Chan<int> chan2;
    
    sync::go([&]{
        print "Hello";
//         for (int i = 0; i < 10; i++) {
//             print "sending %d" % i;
//             //chan1.send(i);
//         }
        //chan1.close();
        //chan2.close();
        chan1.send(1);
        chan1.close();
        chan2.send(2);
        //chan2.close();
    });
    
    sleep(1);
    
repeat:
    int value;
    goto *select(&&label1, chan1, value, 
                 &&label2, chan2, value,
                 &&label3);
    
    
    print "nolabel";
    exit(0);
    
label1:
    print "label1", value;
    goto repeat;
    
label2:
    print "label2", value;
    goto repeat;
    
label3:
    print "label3", value;
    
    exit(0);
    
//     exit(0);
    
    
    sync::go([&]{
        for (int i = 0; i < 10; i++) {
            print "sending %d" % i;
            chan1.send(i);
        }
        chan1.close();
        chan2.close();
    });
    
    sleep(1);
    
    do {
        int value;
        while (chan1.poll(value)) {
            print "got from 1:", value;
        }
//         while (chan2.poll(value)) {
//             print "got from 2:", value;
//         }
    } while (sync::wait(chan1));
    
  
    print "goodbye";
    
    return 0;
}