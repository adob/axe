#include <unistd.h>
#include <sys/syscall.h>
#import "queue.h"
#import "barrier.h"
#import "go.h"

namespace axe {
    namespace sync {
        using functype = Func<void()>;
        std::vector<pthread_t> worker_threads;
        
        const int MaxThreads   = 100000;
        
        Queue<functype> work_queue(MaxQueueSize) ;
        
        struct PThreadAttr {
            pthread_attr_t attr;
            
            PThreadAttr() {
                if (int code = pthread_attr_init(&attr))
                    raise(code);
            }
            
            void setinheritedsched(int inheritedsched) {
                if (int code = pthread_attr_setinheritsched(&attr, inheritedsched))
                    raise(code);
            }
            
            void setschedpolicy(int policy) {
                if (int code = pthread_attr_setschedpolicy(&attr, policy))
                    raise(code);
            }
            
            void setschedparam(sched_param *param) {
                if (int code = pthread_attr_setschedparam(&attr, param))
                    raise(code);
            }
            
            ~PThreadAttr() {
                if (int code = pthread_attr_destroy(&attr))
                    raise(code);
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
            
            if (int code = pthread_create(&pthread, &attr.attr, func, arg))
                raise(code);
            
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
            functype workitem = std::move(info->workitem);
            info->barrier.wait();
            //print "worker_thread after barrier";
            
            while (true) {
                
                //try {
                workitem();
                //} catch (Exception ex) {
                //    print "Exception while working on item:", ex.msg;
                //    debug::print_backtrace(ex.backtrace);
                //} catch (...) {
                    //    print "WorkItem threw some exception";
                //}
                workitem = nil;
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
            
            mytid = syscall(SYS_gettid);
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            
            sched_getaffinity(mytid, sizeof(cpuset), &cpuset);
            num_cpus = CPU_COUNT(&cpuset);
            if (num_cpus == 0) {
                return err = "Problem determining number of CPUs";
            }
            
            // upgrade ourselves to realtime priority 2
            struct sched_param schedp = { 2 };
            ret = sched_setscheduler(mytid, SCHED_FIFO, &schedp);
            if (ret) {
                return err = errno;
            }
            
            // create scheduler thread
            create_thread(sched_thread_func, nullptr, 1);
        }
        
        void go(Func<void()> const& f) {
            work_queue.enqueue(f);
        }
        void go(Func<void()> && f) {
            work_queue.enqueue(std::move(f));
        }
    }
}