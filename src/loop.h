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

#ifdef __linux__
#include "unistd.h"
#include "sys/epoll.h"
typedef epoll_event new_event;
#elif __APPLE__
#include "unistd.h"
#include <sys/event.h>
typedef struct kevent new_event;
#else
#include <winscok2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

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
            LOG("null watcher");
            return false;
        }
        const int fd = w->__fd();
        if (fd >= wlist.size()) {
            return false;
        }

        wlist[fd] = w;
        new_event ev;
#ifdef __linux__
        ev.data.fd = w->__fd();
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_ADD, fd, &ev);
#else
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);  // 赋值
        kevent(backend_fd, &ev, 1, NULL, 0, NULL);    // 添加
#endif
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
        new_event ev;
#ifdef __linux__
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_DEL, fd, &ev);
#endif
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
    new_event* events; // epoll 事件

    void epoll_init() {
        #ifdef __linux__
        backend_fd = epoll_create(FDSIZE);        
        #else
        backend_fd = kqueue();
        #endif
        events = (new_event*)malloc(sizeof(new_event) * EVENT_MAX);
    }

    void backend_epoll() {
        #ifdef __linux__
        int event_cnt = epoll_wait(backend_fd, events, EVENT_MAX, -1);
        #else
        int event_cnt = kevent(backend_fd, NULL, 0, events, EVENT_MAX, NULL);
        #endif
        if (event_cnt < 0 && event_cnt != -1) {
            perror("epoll_wait");
            exit(1);
        }
        LOG("event_cnt %d", event_cnt);
        for (int i = 0; i < event_cnt; ++i) {
            #ifdef __linux__
            int fd = events[i].data.fd;
            uint32_t event = events[i].events;
            #else
            int fd = (int) events[i].ident;  // 监听描述符
            uint32_t event = events[i].filter;
            if (events[i].flags & EV_EOF){
                close(fd);
                LOG("close fd %d\n", fd);
                continue;
            }
            #endif
            
            watcher* head = wlist[fd];
            if (NULL == head) {
                LOG("null watcher fd %d", fd);
                continue;
            }
            if (head->__event() == event) {
                pending[head->__priority()] = head;
            }
        }
    }

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
