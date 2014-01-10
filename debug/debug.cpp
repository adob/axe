#include <execinfo.h>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <axe/fmt.h>
#include <axe/debug.h>
#include <axe/print.h>

extern "C" {
    char*
    __cxa_demangle(const char* __mangled_name, char* __output_buffer,
                   size_t* __length, int* __status);
    
    std::type_info*
    __cxa_current_exception_type();
    
}

namespace axe { namespace debug {


void print_backtrace(int offset) {
    std::vector<void*> ptrs = backtrace(offset);
    print_backtrace(ptrs);
    
}

void print_backtrace(std::vector<void*> backtrace) {
    Allocator alloc;
    flockfile(stdout);
    auto unnlock_stdout = defer([]{ funlockfile(stdout); });
    
    for (void *ptr : backtrace) {
        LineInfo info = debug::addr2line(ptr);
        
        Str s(alloc);
        s += fmt::sprintf(alloc, "  %#.12x", (uintptr)ptr);
        if (info.funcname != "??") {
            s += fmt::sprintf(alloc, " in %s", info.funcname);
        }
        if (info.filepath != "??") {
            s += fmt::sprintf(alloc, " at %s:%d", info.filepath, info.lineno);
        }
        fmt::printf("%s\n", (str) s);
    }
}

void crash_handler() {
    
    fmt::printf("Terminated due to unhandled");
    
    std::exception_ptr excep = std::current_exception();
    try {
        std::rethrow_exception(excep);
    } catch (error e) {
        fmt::printf(" error: ");
        print e;
    } catch (Exception e) {
        fmt::printf(" exception: %s\n", e.msg);
        print_backtrace(e.backtrace);
        return;
    } catch (...) {
        std::type_info *t = __cxa_current_exception_type();
        std::string type = demangle(t->name());
        fmt::printf(" exception of type %s.\n", str(type));
        
    }
    
    print_backtrace(1);
}

void sigfpe_handler(int /*signum*/, siginfo_t */*siginfo*/, void*) {
    //printf("Terminating due to SIGFPE at %#zx\n", (uintptr) siginfo->si_addr);
    //Allocator alloc;
    //print_backtrace(1, alloc);
    fprintf(stderr, "SIGFPE\n");
    
    sigset_t x;
    sigemptyset (&x);
    sigaddset(&x, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &x, NULL);
    
    raise("SIGFPE");
    //abort();
}

void sigsegv_handler(int /*signum*/, siginfo_t *siginfo, void*) {
    print "SIGSEGV";
    print_backtrace(0);
    std::string msg = fmt::sprintf("Terminating due to SIGSEGV at %#x\n", (uintptr) siginfo->si_addr);
//     print msg;
//     throw "foo";
    sigset_t x;
    sigemptyset (&x);
    sigaddset(&x, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &x, NULL);
//     throw 3;
    raise(str(msg));
}

void init() {
    std::set_terminate(crash_handler);
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = sigfpe_handler;
    action.sa_flags = SA_SIGINFO;

    int err = sigaction(SIGFPE, &action, nil);
    if (err) {
        throw error(errno);
    }
    
    action.sa_sigaction = sigsegv_handler;    
    err = sigaction(SIGSEGV, &action, nil);
    if (err) {
        throw error(errno);
    }
}

std::string demangle(str mangled) {
    int status;
    char *charp = __cxa_demangle(mangled.tmp_c_str(), 0, 0, &status);
    
    if (status != 0)
        return (std::string) mangled;
    
    std::string demangled = charp;
    free(charp);
    return demangled;
    
}

std::vector<void*> backtrace(int offset) {
    std::vector<void*> ptrs(1024);
    int nptrs = ::backtrace(ptrs.data(), ptrs.size());
    if (offset != 0) {
        ptrs.erase(begin(ptrs), begin(ptrs)+offset);
    }
    ptrs.resize(nptrs);
    return ptrs;
}

}}