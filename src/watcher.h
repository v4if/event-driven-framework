//
// Created by root on 7/15/17.
//

#ifndef MINI_REACTOR_WATCHER_H
#define MINI_REACTOR_WATCHER_H


#include <functional>
#include <stdint.h>
#include <string>
#include "slog.h"
#define PRI_MAX 2
#define PRI_MIN 0

class socket_buffer;

class watcher {
public:
  watcher(int _fd, uint32_t _event, int _priority, std::function<void(watcher *w)> _cb, std::string &name);

    int __fd() {return fd;}
    uint32_t get_event() {return event;}
    int __priority() {return priority;}
    void __cb() {
        cb(this);
    }
    std::string &get_name() { return name_; }
    socket_buffer* get_buffer() { return buf_; }
    void clear();

  private:
    int fd;  // 监听的fd
    uint32_t event;  // 监听的事件
    int priority;  // 优先级
    std::function<void(watcher* w)> cb;  // handler
    std::string name_;
    socket_buffer* buf_;
};

#endif //MINI_REACTOR_WATCHER_H
