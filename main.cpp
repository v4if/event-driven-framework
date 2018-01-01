#include <iostream>
#include <functional>
#include "src/loop.h"

static loop default_loop;
int main() {
    watcher stdin_watcher(STDIN_FILENO, EPOLLIN, 0, [](watcher* w){
        int buf_size = 1024;
        char read_buf[buf_size];
        fgets(read_buf, 1024, stdin);
        default_loop.__set_loop_done(true);

        std::cout << read_buf << std::endl;
    });
    default_loop.register_watcher(&stdin_watcher);

    std::cout << "===== default loop start =====" << std::endl;
    default_loop.run();
    return 0;
}