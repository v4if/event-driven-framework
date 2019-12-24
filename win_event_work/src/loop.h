//
// Created by root on 7/15/17.
//

#ifndef MINI_REACTOR_LOOP_H
#define MINI_REACTOR_LOOP_H

#include <vector>
#include <iostream>
#include "watcher.h"
#include "stdio.h"
#include "slog.h"


#define FDSIZE 1024
#define EVENT_MAX 64
class loop {
public:
    loop(): loop_done(false), wlist(FDSIZE, NULL), pending(PRI_MAX - PRI_MIN + 1, NULL) {
    }
    virtual ~loop() {

    }

    virtual void register_watcher(int fd, int event) = 0;
    virtual void remove_watcher(int fd, int event) = 0;

    bool register_watcher(watcher* w) {
        if (NULL == w) {
            LOG("null watcher");
            return false;
        }
        const int fd = w->__fd();
        if (fd >= wlist.size()) {
            return false;
        }

        wlist[fd] = w;
        register_watcher(w->__fd(), w->get_event());
        LOG("add new event fd %d", fd);
        return true;
    }

    bool remove_watcher(watcher* w) {
        if (NULL == w) {
            LOG("null watcher\n");
            return false;
        }
        const int fd = w->__fd();
        wlist[fd] = NULL;
        remove_watcher(fd, w->get_event());
        printf("remove fd %d", fd);
        return true;
    }
    void run() {
        do {
            backend_epoll();
            invoke_pending();
            clear_pending();
        } while (!loop_done);
    }

    void __set_loop_done(bool done) {
        loop_done = done;
    }

    virtual void backend_epoll() = 0;

protected:
    bool loop_done;
    std::vector<watcher*> wlist;  // 对应优先级上的handler队列
    std::vector<watcher*> pending;  // poll出来待处理的事件

    void invoke_pending() {
        int pending_pri = PRI_MAX - PRI_MIN;
        for (; pending_pri >= 0; pending_pri--) {
            watcher* w = pending[pending_pri];
            if (NULL == w) {
                continue;
            }
            LOG("do a watcher %s!", w->get_name().c_str());
            w->__cb();
        }
    }

    void clear_pending() {
        int pending_pri = PRI_MAX - PRI_MIN;
        for (; pending_pri >= 0; pending_pri--) {
            pending[pending_pri] = NULL;
        }
    }
};


#endif //MINI_REACTOR_LOOP_H
