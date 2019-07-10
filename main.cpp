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
#include "src/sbuffer.h"

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

int send_max_num = 0;
const std::string ping_str = "ping";
const std::string pong_str = "ping";
void handle_client_read(watcher* w)
{
    if (NULL == w) {
        printf("numm watcher!!");
        return;
    }
    int client_fd = w->__fd();
    sbuffer* buf = w->get_buffer();
    printf("client_fd %d\n", client_fd);
    int nread = read(client_fd, buf->get_begin_data(), buf->get_left_length());
    if (nread < 1) {
        printf("client_fd %d error\n", client_fd);
        default_loop.remove_watcher(w);
        return;
    }

    buf->add_length(nread);

    std::string res(buf->get_data(), buf->get_data_length());
    printf("from client fd %d read : %s\n", client_fd, res.c_str());
    buf->reset();
    if (send_max_num ++ < 10) {
        if (ping_str == res){
            send(client_fd, pong_str.data(), pong_str.size());
        }
        else{
            send(client_fd, ping_str.data(), ping_str.size());
        }
    }
}

void handle_new_socket(watcher* w)
{
    int connfd = accept(w->__fd(), (struct sockaddr*)NULL, NULL);
    printf("get new conn %d\n", connfd);
    std::string name("handle_client_read");
    watcher* client_watcher = new watcher(connfd, EPOLLIN, 0, handle_client_read, name);
    default_loop.register_watcher(client_watcher);
}

enum run_type_input
{
    run_type_server = 1,
    run_type_client,
};

#define PORT 8889
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("pls input 1 server 2 client!\n");
        exit(1);
    }

    int run_type = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    if (run_type_server == run_type) {
        int listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(PORT); 
        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        listen(listenfd, 10);
        
        std::string name("handle_new_socket");
        watcher server_watcher(listenfd, EPOLLIN, 0, handle_new_socket, name);
        default_loop.register_watcher(&server_watcher);
        std::cout << "===== default loop start =====" << std::endl;
    }
    else {
        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(PORT);
        connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        send(client_fd, ping_str.data(), ping_str.size(), 0);
        watcher client_watcher(client_fd, EPOLLIN, 0, handle_client_read, ping_str);
        default_loop.register_watcher(&client_watcher);
        std::cout << "===== default loop start =====" << std::endl;
    }
    default_loop.run();
    return 0;
}