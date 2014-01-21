#import <axe/sync/chan.h>
#import <axe/io/file.h>
#import <axe/errorparam.h>

namespace axe {
    namespace io {;
        void notify_init(errorparam = {});
        
        struct Notifier {
            int fd;
            int op;
            sync::Chan<uint32> *chan;
            
            Notifier(io::File& file, int op, sync::Chan<uint32>& chan, errorparam = {});
            Notifier(int fd, int op, sync::Chan<uint32>& chan, errorparam = {});
            Notifier(Notifier const&) = delete;
            Notifier(Notifier&&);
            
            void modify(int op, sync::Chan<uint32>& chan, errorparam = {});
            void modify(int op, errorparam = {});
            void rearm(errorparam = {});
            void stop(errorparam = {});
            
            Notifier& operator = (Notifier const&) = delete;
            Notifier& operator = (Notifier     &&);
            
            ~Notifier();
        } ;
        
        void notify(io::File&, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
        void notify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
        
        void renotify(io::File&, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
        void renotify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
        
        void unnotify(io::File&, errorparam = {});
        void unnotify(int fd, errorparam = {});
    }
}
    