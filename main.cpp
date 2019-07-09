#include <iostream>
#include <functional>
#include "src/loop.h"

static loop default_loop;
void print_data(watcher* w)
{
    char read_buf[1024];
    fgets(read_buf, 1024, stdin);
    default_loop.__set_loop_done(true);

    std::cout << read_buf << std::endl;
}
int main() {
    watcher stdin_watcher(STDIN_FILENO, EPOLLIN, 0, print_data);
    default_loop.register_watcher(&stdin_watcher);

    std::cout << "===== default loop start =====" << std::endl;
    default_loop.run();
    return 0;
}