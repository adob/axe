#import <vector>
#import <axe/str.h>
#import <axe/fmt.h>
#import <axe/buffer.h>
#import <axe/backtrace.h>
#import <axe/debug/debug.h>

namespace axe {
    namespace testing {
        
        struct T {
            struct ErrorInfo {
                String description;
                Backtrace backtrace;
            };
            
            bool failed = false;
            std::vector<ErrorInfo> errors;
            
            template <typename... Args>
            void errorf(str format, Args&&... args) {
                String s;
                fmt::writef(s, format, std::forward<Args>(args)...);
                errors.push_back({std::move(s), debug::backtrace(2) });
                fail();
            }
            
            void fail();
            
            void operator () (bool);
        } ; 
        
        using testfunc = void (*) (T&);
        
        struct TestCase {
            testfunc func;
            str      name;
            str      filename;
            int      lineno;
            
            TestCase(str name, testfunc func, str filename, int lineno);
        } ;
    }
}

#define TESTCASE(NAME) \
  void test_ ## NAME (axe::testing::T&); \
  axe::testing::TestCase testcase_ ## NAME ( #NAME, test_ ## NAME , __FILE__, __LINE__); \
  void test_ ## NAME (axe::testing::T& test /*__attribute__((unused))*/)

