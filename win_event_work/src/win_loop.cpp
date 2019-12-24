#include "win_loop.h"

win_loop::win_loop() : loop()
{
    epoll_init();
}

void win_loop::epoll_init()
{
    FD_ZERO(&ReadSet);
    FD_ZERO(&WriteSet);
}
void win_loop::register_watcher(int fd, int events)
{
    FD_SET(fd, &ReadSet);
}

void win_loop::remove_watcher(int fd, int events)
{
    FD_CLR(fd, &ReadSet);
}

void win_loop::backend_epoll()
{
    select(0, &ReadSet, &WriteSet, NULL, NULL);
    for (int i = 0; i < wlist.size(); ++i)
    {
        watcher* head = wlist[i];
        if (NULL == head)
        {
            continue;
        }

        int fd = head->__fd();
        int event = head->get_event();
        if (FD_ISSET(fd, &ReadSet))
        {
            pending[head->__priority()] = head;
        }
    }
}
