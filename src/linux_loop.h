#ifndef slinux_loop_h
#define slinux_loop_h

#include "loop.h"
#include "unistd.h"
#include "sys/epoll.h"

class linux_loop : loop
{
public:
    virtual void epoll_init() {
        backend_fd = epoll_create(FDSIZE);        
        events = (epoll_event*)malloc(sizeof(epoll_event) * EVENT_MAX);
    }

    void register_watcher(int fd, int event)
    {
        epoll_event ev;
        ev.data.fd = w->__fd();
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_ADD, fd, &ev);
    }
    virtual remove_watcher(int fd, int event) 
    {
        epoll_event ev;
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_DEL, fd, &ev);
    }

    virtual void backend_epoll() {
        int event_cnt = epoll_wait(backend_fd, events, EVENT_MAX, -1);
        if (event_cnt < 0 && event_cnt != -1) {
            perror("epoll_wait");
            exit(1);
        }
        LOG("event_cnt %d", event_cnt);
        for (int i = 0; i < event_cnt; ++i) {
            int fd = events[i].data.fd;
            uint32_t event = events[i].events;
            
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
private:
    int backend_fd; // 后端epoll fd
    epoll_event* events; // epoll 事件
}