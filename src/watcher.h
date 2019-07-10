//
// Created by root on 7/15/17.
//

#ifndef MINI_REACTOR_WATCHER_H
#define MINI_REACTOR_WATCHER_H


#include <functional>
#include <stdint.h>
#include <string>
#define PRI_MAX 2
#define PRI_MIN 0

class sbuffer;

class watcher {
public:
  watcher(int _fd, uint32_t _event, int _priority, std::function<void(watcher *w)> _cb, std::string &name);

  watcher *watcher_list_add(watcher *w)
  {
      watcher *head = this;
      if (head == NULL)
      {
          return w;
      }

      w->next = head->next;
      head->next = w;
      return head;
    }

    watcher* watcher_list_remove(watcher* w) {
        watcher* head = this;
        if (head == w) return head->next;

        watcher* dummy = head->next;
        watcher* pre = head;
        while (dummy) {
            if (dummy == w) {
                pre->next = dummy->next;
            }
        }
        return head;
    }

    int __fd() {return fd;}
    uint32_t __event() {return event;}
    watcher* __next() {return next;}
    int __priority() {return priority;}
    void __cb(watcher* w) {
        cb(w);
    }
    std::string &get_name() { return name_; }
    sbuffer* get_buffer() { return buf_; }

  private:
    int fd;  // 监听的fd
    uint32_t event;  // 监听的事件
    int priority;  // 优先级
    std::function<void(watcher* w)> cb;  // handler
    watcher* next;  // 监听队列
    std::string name_;
    sbuffer* buf_;
};

#endif //MINI_REACTOR_WATCHER_H
