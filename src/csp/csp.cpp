#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <condition_variable>
#include <list>

#include <axe/fmt.h>
#include <axe/debug.h>

#include "csp.h"

namespace axe { namespace csp {

std::vector<pthread_t> worker_threads;

const int MaxThreads   = 100000;
const int MaxQueueSize = 100000;


struct PThreadMutex {
    pthread_mutex_t mutex;
    
    PThreadMutex() {
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr)) {
            throw error(errno);
        }
        
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
            pthread_mutexattr_destroy(&attr);
            throw error(errno);
        }
        
//         if (pthread_mutexattr_settype&attr, PTHREAD_MUTEX_DEFAULT) {
//             pthread_mutexattr_destory(&attr);
//             throw error(errno);
//         }
//         
//         if (pthread_mutexattr_setprotocol&attr, PTHREAD_PRIO_NONE)) {
//             pthread_mutexattr_destory(&attr);
//             throw error(errno);
//         }
    
        if (pthread_mutex_init(&mutex, &attr)) {
            pthread_mutexattr_destroy(&attr);
            throw error(errno);
        }
        
        pthread_mutexattr_destroy(&attr);
    }
    
    ~PThreadMutex() {
        pthread_mutex_destroy(&mutex);
    }
} ; 

struct MutexLocker {
    PThreadMutex &mutex;
    
    MutexLocker(PThreadMutex& mutex) : mutex(mutex) {
        if (pthread_mutex_lock(&mutex.mutex))
            throw error(errno);
    }
    
    ~MutexLocker() {
        if (pthread_mutex_unlock(&mutex.mutex))
            throw error(errno);
    }
} ;

struct PThreadBarrier {
    pthread_barrier_t barr;
    
    PThreadBarrier(unsigned count) {
        pthread_barrierattr_t attr;
        if (pthread_barrierattr_init(&attr))
            throw error(errno);
        
        if (pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
            pthread_barrierattr_destroy(&attr);
            throw error(errno);
        }
        
        if (pthread_barrier_init(&barr, &attr, count))
            throw error(errno);
        
        pthread_barrierattr_destroy(&attr);
    }
    
    void wait() {
        int ret = pthread_barrier_wait(&barr);
        if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
            throw error(errno);
    }
    
    ~PThreadBarrier() {
        pthread_barrier_destroy(&barr);
    }
    
} ;



struct PThreadCond {
    pthread_cond_t cond;
    
    PThreadCond() {
        pthread_condattr_t attr;
        
        if (pthread_condattr_init(&attr))
            throw error(errno);
        
        if (pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
            pthread_condattr_destroy(&attr);
            throw error(errno);
        }
        
        if (pthread_cond_init(&cond, &attr)) {
            pthread_condattr_destroy(&attr);
            throw error(errno);
        }
        
        pthread_condattr_destroy(&attr);
    }
    
    void wait(PThreadMutex& mutex) {
        if (pthread_cond_wait(&cond, &mutex.mutex))
            throw error(errno);
    }
    
    void signal() {
        if (pthread_cond_signal(&cond))
            throw error(errno);
    }
    
    ~PThreadCond() {
        pthread_cond_destroy(&cond);
    }
};

struct PThreadAttr {
    pthread_attr_t attr;
    
    PThreadAttr() {
        if (pthread_attr_init(&attr))
            throw error(errno);
    }
    
    void setinheritedsched(int inheritedsched) {
        if (pthread_attr_setinheritsched(&attr, inheritedsched))
            throw error(errno);
    }
    
    void setschedpolicy(int policy) {
        if (pthread_attr_setschedpolicy(&attr, policy))
            throw error(errno);
    }
    
    void setschedparam(sched_param *param) {
        if (pthread_attr_setschedparam(&attr, param))
            throw error(errno);
    }
    
    ~PThreadAttr() {
        pthread_attr_destroy(&attr);
    }
} ;

struct WorkQueue {
    struct Item {
        functype func;
    } ;
    
    std::queue<Item>         queue;
    PThreadCond              enqcond;
    PThreadCond              deqcond;
    PThreadMutex             mutex;
    
    void enqueue(functype func) {
        {
            MutexLocker lock(mutex);
            
            while (queue.size() > MaxQueueSize) {
                enqcond.wait(mutex);
            }
            
            queue.push({func});
        }
        
        deqcond.signal();
    }
    
    Item dequeue() {
        MutexLocker lock(mutex);
        
        while (queue.empty()) {
            deqcond.wait(mutex);
        }
        
        Item item = queue.front();
        queue.pop();
        
        if (queue.size() == MaxQueueSize - 1) {
            enqcond.signal();
        }
        
        return item;
        
    }
    
} work_queue ;

pthread_t create_thread(void *(*func) (void *), void *arg, int prio) {
    PThreadAttr attr;
    pthread_t pthread;
    
    attr.setinheritedsched(PTHREAD_EXPLICIT_SCHED);
    attr.setschedpolicy(SCHED_FIFO);
    
    sched_param param {
        sched_priority: prio
    };
    
    attr.setschedparam(&param);
    
    if (pthread_create(&pthread, &attr.attr, func, arg))
        throw error(errno);
    
    return pthread;
}

struct WorkerThreadInfo {
    PThreadBarrier barrier;
    WorkQueue::Item& workitem;
    
    WorkerThreadInfo(WorkQueue::Item& workitem)
      : barrier(2)
      , workitem(workitem)
      {}
} ;

void *worker_thread_func(void *arg) {
    //print "worker_thread started";
    
    WorkerThreadInfo *info = (WorkerThreadInfo *) arg;
    WorkQueue::Item workitem = info->workitem;
    info->barrier.wait();
    //print "worker_thread after barrier";
    
    while (true) {

        try {
            workitem.func();
        } catch (Exception ex) {
            print "Exception while working on item:", ex.msg;
            debug::print_backtrace(ex.backtrace);
        } catch (...) {
            print "WorkItem threw some exception";
        }
        
        workitem = work_queue.dequeue();
    }
    
    
    return nil;
}

void *sched_thread_func(void *) {
    //print "sched_thread started";
    while (true) {
        WorkQueue::Item item = work_queue.dequeue();
        //print "sched_thread got item from workque";
        
        WorkerThreadInfo info(item);
        
        pthread_t pthread = create_thread(worker_thread_func, &info, 2);
        worker_threads.push_back(pthread);
        //print "sched_thread created worker thread number", worker_threads.size();
        info.barrier.wait();
        //print "sched_thread worker thread inited";
        
        if (worker_threads.size() >= MaxThreads) {
            break;
        }
    }
    //print "sched_thread done";
    return nil;
}


void init(errorparam err) {
    int ret;
    int num_cpus;
    pid_t mytid;
    
    //print "init csp";
    
    mytid = syscall(SYS_gettid);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    sched_getaffinity(mytid, sizeof(cpuset), &cpuset);
    num_cpus = CPU_COUNT(&cpuset);
    if (num_cpus == 0) {
        return err = "Problem determining number of CPUs.";
    }
    
    //print "%d CPU(s) detected" % num_cpus;
    
    // upgrade ourselves to realtime priority 2
    struct sched_param schedp = { 2 };
    ret = sched_setscheduler(mytid, SCHED_FIFO, &schedp);
    if (ret) {
        return err = errno;
    }
    
    // create scheduler thread
    create_thread(sched_thread_func, nullptr, 1);
}

void go(functype f) {
    work_queue.enqueue(f);
}
    
}}