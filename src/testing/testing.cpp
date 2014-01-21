#include <vector>
#include <unistd.h>
#import <axe/print.h>
#import <axe/sync.h>

#import "testing.h"
using namespace axe;

std::vector<testing::TestCase*> testcases;

namespace axe {
    namespace testing {
        
        
        
        TestCase::TestCase(str name, testfunc func, str filename, int lineno)
          : func(func)
          , name(name)
          , filename(filename)
          , lineno(lineno) {
              
            testcases.push_back(this);
            
        }
        
        void T::fail() {
            failed = true;
        }
        
        void T::operator () (bool b) {
            if (!b) {
                errors.push_back({"Failed", debug::backtrace(2) });
                fail();
            }
        }
    }
}



int main() {
    debug::init();
    
    bool failed = false;
    
    for (testing::TestCase *testcase : testcases) {
        testing::T t;
        
        //print "running %q in %s:%d" % testcase->name, testcase->filename, testcase->lineno;
        testcase->func(t);
        
        if (t.failed) {
            failed = true;
            print "--- FAIL: %s in %s:%d" % testcase->name, testcase->filename, testcase->lineno;
            for (testing::T::ErrorInfo const& info : t.errors) {
                print "\t%s" % info.description;
                print info.backtrace;
            }
        } else {
            //print "ok\t%q in %s:%d" % testcase->name, testcase->filename, testcase->lineno; 
            fmt::printf("ok      %-30s%s:%s\n", testcase->name, testcase->filename, testcase->lineno);
        }
    }
    
    if (!failed) {
        print "PASS";
    }
}