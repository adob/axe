#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <vector>

#include <axe/fmt.h>
#include <axe/debug.h>

#include "sync.h"

namespace axe { namespace sync {

using functype = std::function<void()>;
std::vector<pthread_t> worker_threads;

const int MaxThreads   = 100000;

Queue<functype> work_queue(MaxQueueSize) ;

Mutex::Mutex() {
    pthread_mutexattr_t attr;
    
    if (pthread_mutexattr_init(&attr)) {
        raise(errno);
    }

    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
        pthread_mutexattr_destroy(&attr);
        raise(errno);
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
        raise(errno);
    }

    pthread_mutexattr_destroy(&attr);
}

void Mutex::lock() {
    if (pthread_mutex_lock(&mutex))
        raise(errno);
}

void Mutex::unlock() {
    if (pthread_mutex_unlock(&mutex))
        raise(errno);
}
    
Mutex::~Mutex() {
    pthread_mutex_destroy(&mutex);
}

    
Lock::Lock(Mutex& mutex) : mutex(mutex), locked(true) {
    mutex.lock();
}

void Lock::relock() {
    if (locked)
        raise("Lock already locked");
    locked = true;
    mutex.lock();
}
void Lock::unlock() {
    if (!locked)
        raise("Lock already unlocked");
    
    mutex.unlock();
    locked = false;
}
    
Lock::~Lock() {
    if (locked)
        mutex.unlock();
}

    
Barrier::Barrier(unsigned count) {
    pthread_barrierattr_t attr;
    if (pthread_barrierattr_init(&attr))
        raise(errno);
    
    if (pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
        pthread_barrierattr_destroy(&attr);
        raise(errno);
    }
    
    if (pthread_barrier_init(&barr, &attr, count))
        raise(errno);
    
    pthread_barrierattr_destroy(&attr);
}
    
void Barrier::wait() {
    int ret = pthread_barrier_wait(&barr);
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
        raise(errno);
}

Barrier::~Barrier() {
    pthread_barrier_destroy(&barr);
}


Cond::Cond() {
    pthread_condattr_t attr;
    
    if (pthread_condattr_init(&attr))
        raise(errno);
    
    if (pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
        pthread_condattr_destroy(&attr);
        raise(errno);
    }
    
    if (pthread_cond_init(&cond, &attr)) {
        pthread_condattr_destroy(&attr);
        raise(errno);
    }
    
    pthread_condattr_destroy(&attr);
}
    
void Cond::wait(Mutex& mutex) {
    if (pthread_cond_wait(&cond, &mutex.mutex))
        raise(errno);
}
    
void Cond::signal() {
    if (pthread_cond_signal(&cond))
        raise(errno);
}
    
Cond::~Cond() {
    pthread_cond_destroy(&cond);
}

struct PThreadAttr {
    pthread_attr_t attr;
    
    PThreadAttr() {
        if (pthread_attr_init(&attr))
            raise(errno);
    }
    
    void setinheritedsched(int inheritedsched) {
        if (pthread_attr_setinheritsched(&attr, inheritedsched))
            raise(errno);
    }
    
    void setschedpolicy(int policy) {
        if (pthread_attr_setschedpolicy(&attr, policy))
            raise(errno);
    }
    
    void setschedparam(sched_param *param) {
        if (pthread_attr_setschedparam(&attr, param))
            raise(errno);
    }
    
    ~PThreadAttr() {
        pthread_attr_destroy(&attr);
    }
} ;

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
        raise(errno);
    
    return pthread;
}

struct WorkerThreadInfo {
    Barrier barrier;
    functype& workitem;
    
    WorkerThreadInfo(functype& workitem)
      : barrier(2)
      , workitem(workitem)
      {}
} ;

void *worker_thread_func(void *arg) {
    //print "worker_thread started";
    
    WorkerThreadInfo *info = (WorkerThreadInfo *) arg;
    functype workitem = info->workitem;
    info->barrier.wait();
    //print "worker_thread after barrier";
    
    while (true) {

        try {
            workitem();
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
        functype item = work_queue.dequeue();
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
    
    //print "init sync";
    
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
        exit(1);
        return err = errno;
    }
    
    // create scheduler thread
    create_thread(sched_thread_func, nullptr, 1);
}

void go(functype const& f) {
    work_queue.enqueue(f);
}
void go(functype && f) {
    work_queue.enqueue(std::move(f));
}
    
}}