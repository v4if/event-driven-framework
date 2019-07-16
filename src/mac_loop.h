#ifndef mac_loop_h
#define mac_loop_h

#include "loop.h"
#include "unistd.h"
#include <sys/event.h>

class mac_loop : loop
{
public:
    virtual void epoll_init() {
        backend_fd = kqueue();
        events = (kevent*)malloc(sizeof(kevent) * EVENT_MAX);
    }

    void register_watcher(int fd, int event)
    {
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);  // 赋值
        kevent(backend_fd, &ev, 1, NULL, 0, NULL);    // 添加
    }
    virtual remove_watcher(int fd, int event) 
    {
        epoll_event ev;
        ev.events = w->__event();
        epoll_ctl(backend_fd, EPOLL_CTL_DEL, fd, &ev);
    }

    virtual void backend_epoll() {
        int event_cnt = kevent(backend_fd, NULL, 0, events, EVENT_MAX, NULL);
        if (event_cnt < 0 && event_cnt != -1) {
            perror("epoll_wait");
            exit(1);
        }
        LOG("event_cnt %d", event_cnt);
        for (int i = 0; i < event_cnt; ++i) {
            int fd = (int) events[i].ident;  // 监听描述符
            uint32_t event = events[i].filter;
            if (events[i].flags & EV_EOF){
                close(fd);
                LOG("close fd %d\n", fd);
                continue;
            }
            
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
    kevent* events; // epoll 事件
}