#include "watcher.h"
#include "sbuffer.h"

   watcher::watcher(int _fd, uint32_t _event, int _priority, std::function<void(watcher *w)> _cb, std::string &name) :
    fd(_fd), event(_event), priority(_priority), cb(_cb), next(NULL), name_(name)
    ,buf_(new sbuffer(1024)) {
        printf("watcher fd %d evt %d\n", _fd, _event);
        if (priority > PRI_MAX) priority = PRI_MAX;
        if (priority < PRI_MIN) priority = PRI_MIN;
    }