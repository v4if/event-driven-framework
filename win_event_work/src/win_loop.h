#ifndef win_loop_h
#define win_loop_h
#include <WinSock2.h>
#include "loop.h"

#include "watcher.h"
#include <winsock2.h>

class win_loop : public loop
{
public:
    win_loop();
    virtual void epoll_init();

    virtual void register_watcher(int fd, int events);
    virtual void remove_watcher(int fd, int events);

    virtual void backend_epoll();
private:
   FD_SET WriteSet;
   FD_SET ReadSet;
};
#endif