#include <iostream>
#include <functional>
#include "src/loop.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

static loop default_loop;
void print_data(watcher* w)
{
    char read_buf[1024];
    fgets(read_buf, 1024, stdin);
    default_loop.__set_loop_done(true);

    std::cout << read_buf << std::endl;
}

void print_timer(watcher* w)
{
    std::cout << "timer" << std::endl;
}

void handle_client_read(watcher* w)
{
    printf("heree!!!");
    if (NULL == w) {
        printf("numm watcher!!");
        return;
    }
    int client_fd = w->__fd();
    printf("client_fd %d\n", client_fd);
    char buffer[1024];
    int nread = read(client_fd, buffer, sizeof(buffer));
    if (nread < 1) {
        printf("client_fd %d error\n", client_fd);
        return;
    }
    std::string res(buffer, nread);
    printf("from client fd %d read %s\n", client_fd, res.c_str());
}

void handle_new_socket(watcher* w)
{
    int connfd = accept(w->__fd(), (struct sockaddr*)NULL, NULL);
    printf("get new conn %d\n", connfd);
    std::string name("handle_client_read");
    watcher client_watcher(connfd, EPOLLIN, 0, handle_client_read, name);
    default_loop.register_watcher(&client_watcher);
}

int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8889); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
    
    std::string name("handle_new_socket");
    watcher server_watcher(listenfd, EPOLLIN, 0, handle_new_socket, name);
    default_loop.register_watcher(&server_watcher);
    std::cout << "===== default loop start =====" << std::endl;
    default_loop.run();
    return 0;
}