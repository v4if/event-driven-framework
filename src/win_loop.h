#ifndef win_loop_h
#define win_loop_h

#include "loop.h"

#include "watcher.h"
#include <winsock2.h>

class win_loop : loop
{
public:
    virtual void epoll_init() 
    {
      FD_ZERO(&ReadSet);
      FD_ZERO(&WriteSet);
    }

    void register_watcher(int fd, int event)
    {
        FD_SET(fd, &ReadSet);
    }
    virtual remove_watcher(int fd, int event) 
    {
        FD_CLR (fd, &ReadSet);
    }

    virtual void backend_epoll() {
        select(0, &ReadSet, &WriteSet, NULL, NULL);
        for (int i = 0; i < wlist.size(); ++i) 
        {
            watcher* head = wlist[fd];
            if (NULL == head) {
                LOG("null watcher fd %d", fd);
                continue;
            }

            int fd = head->__fd();
            int event = head->__event();
            if (FD_ISSET(fd, &ReadSet){
                pending[head->__priority()] = head;
            }
        }
    }
private:
   FD_SET WriteSet;
   FD_SET ReadSet;
}