#include <sys/epoll.h>

#import <axe/print.h>
#import "PKG.h"


namespace axe { namespace sync {
    
io::File pollfile;
    
void notify_init(errorparam err) {
    int fd = epoll_create(1);
    if (fd == -1) {
        err=errno;
    }
    pollfile = io::File(fd);
    
    go([] {
        epoll_event events[1024];
        
        for (;;) {
            int ret = epoll_wait(pollfile.fd, events, 1024, -1);
            if (ret == -1) {
                warn "epoll_wait returned -1:", error(errno);
            }
            
            for (int i = 0; i < ret; i++) {
                epoll_event& event = events[i];
                //print "GOT poll event", event.data;
                Chan<uint32> *chan = (Chan<uint32>*) event.data.ptr;
                chan->send(event.events);
            }
        }
        
    });
}

Notifier::Notifier(io::File& file, int op, sync::Chan<uint32>& chan, errorparam err)
  : Notifier(file.fd, op, chan, err)
  {}

Notifier::Notifier(int fd, int op, sync::Chan<uint32>& chan, errorparam err)
  : fd(fd)
  , op(op)
  , chan(&chan) {
    notify(fd, op, chan, err);
}

Notifier::Notifier(Notifier&& other)
  : fd(other.fd)
  , op(other.op)
  , chan(other.chan) {
      other.fd = -1;
}

void Notifier::rearm(errorparam err) {
    modify(this->op, *this->chan, err);
}
void Notifier::modify(int op, errorparam err) {
    modify(op, *this->chan, err);
}
void Notifier::modify(int op, Chan<uint32>& chan, errorparam err) {
    renotify(this->fd, op, chan, err);
}
void Notifier::stop(errorparam err) {
    unnotify(fd, err);
}

Notifier& Notifier::operator = (Notifier&& other) {
    if (fd != 1) {
        stop();
    }
    fd = other.fd;
    op = other.op;
    chan = other.chan;
    other.fd = -1;
    return *this;
}

Notifier::~Notifier() {
    if (fd == -1) {
        unnotify(fd);
    }
}

void notify(io::File& file, uint32 op, sync::Chan<uint32>& chan, errorparam err) {
    notify(file.fd, op, chan, err);
}
void notify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam err) {
    epoll_event event { op, (void*) &chan };
    int ret = epoll_ctl(pollfile.fd, EPOLL_CTL_ADD, fd, &event);
    if (ret == -1) {
        err = errno;
    }
}

void unnotify(io::File& file, errorparam err) {
    unnotify(file.fd, err);
}
void unnotify(int fd, errorparam err) {
    int ret = epoll_ctl(pollfile.fd, EPOLL_CTL_DEL, fd, nullptr);
    if (ret == -1) {
        err = errno;
    }
}

void renotify(io::File& file, uint32 op, sync::Chan<uint32>& chan, errorparam err) {
    renotify(file.fd, op, chan, err);
}
void renotify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam err) {
    epoll_event event { op, (void*) &chan };
    int ret = epoll_ctl(pollfile.fd, EPOLL_CTL_MOD, fd, &event);
    if (ret == -1) {
        err = errno;
    }
}
}}