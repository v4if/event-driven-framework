//
// Created by root on 7/15/17.
//

#ifndef MINI_REACTOR_LOOP_H
#define MINI_REACTOR_LOOP_H


#include <vector>
#include <iostream>
#include "watcher.h"
#include "sys/epoll.h"
#include "unistd.h"
#include "stdio.h"

#define FDSIZE 1024
#define EVENT_MAX 64
class loop {
public:
    loop(): loop_done(false), wlist(FDSIZE, NULL), pending(PRI_MAX - PRI_MIN + 1, NULL) {
        epoll_init();
    }
    ~loop() {
        close(backend_fd);
        free(events);
    }

    bool register_watcher(watcher* w) {
        if (NULL == w) {
            printf("null watcher\n");
            return false;
        }
        const int fd = w->__fd();
        if (fd >= wlist.size()) {
            printf("fd %d > fdsize %u\n", fd, wlist.size());
            return false;
        }

        wlist[fd] = wlist[fd]->watcher_list_add(w);

        epoll_event ev;
        ev.data.fd = w->__fd();
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_ADD, fd, &ev);
        printf("add new event fd %d\n", fd);
        return true;
    }

    bool remove_watcher(watcher* w) {
        if (NULL == w) {
            printf("null watcher\n");
            return false;
        }
        const int fd = w->__fd();
        wlist[fd] = wlist[fd]->watcher_list_remove(w);

        epoll_event ev;
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_DEL, fd, &ev);
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
private:
    bool loop_done;
    std::vector<watcher*> wlist;  // 对应优先级上的handler队列
    std::vector<watcher*> pending;  // poll出来待处理的事件
    int backend_fd; // 后端epoll fd
    epoll_event* events; // epoll 事件

    void epoll_init() {
        backend_fd = epoll_create(FDSIZE);
        events = (epoll_event*)malloc(sizeof(epoll_event) * EVENT_MAX);
    }

    void backend_epoll() {
        int event_cnt = epoll_wait(backend_fd, events, EVENT_MAX, 0);
        if (event_cnt < 0) {
            perror("epoll_wait");
            exit(1);
        }

        for (int i = 0; i < event_cnt; ++i) {
            int fd = events[i].data.fd;
            uint32_t event = events[i].events;

            watcher* head = wlist[fd];
            for (; head; head = head->__next()) {
                if (head->__event() == event) {
                    pending[head->__priority()] = pending[head->__priority()]->watcher_list_add(head);
                }
            }
        }
    }

    void invoke_pending() {
        int pending_pri = PRI_MAX - PRI_MIN;
        for (; pending_pri >= 0; pending_pri--) {
            watcher* w = pending[pending_pri];
            while (w) {
                printf("do a watcher %s!\n", w->get_name().c_str());
                w->__cb(w);
                w = w->__next();
            }
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
