#include <iostream>
#include <functional>
#include "src/loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/slog.h"
#include "src/socket_buffer.h"
#ifdef _WIN32
#include <winscok2.h>
#pragma comment(lib,"ws2_32.lib")
#define addr_type LPSOCKADDR
#else
 #include <unistd.h>
 #include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#define addr_type struct sockaddr *
#endif // DEBUG

#ifdef __APPLE__
#define EPOLLIN EVFILT_READ
#endif

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
const int g_total_send_num = 1;
const std::string ping_str = "ping";
const std::string pong_str = "pong";
void handle_client_read(watcher* w)
{
    if (NULL == w) {
        LOG("numm watcher!!");
        return;
    }
    int client_fd = w->__fd();
    socket_buffer* buf = w->get_buffer();
    LOG("client_fd %d", client_fd);

    int nread = recv(client_fd, buf->get_begin_data(), buf->get_left_length(), 0);
    if (nread < 1) {
        LOG("client_fd %d error", client_fd);
        default_loop.remove_watcher(w);
        return;
    }

    buf->add_length(nread);

    if (!buf->is_header_decoded()) {
        buf->decode_msg_len();
        LOG("decode mgslen %d", buf->get_msg_len());
    }

    int msg_len = buf->get_msg_len();
    LOG("fd %d nread %d msg_len %d", client_fd, nread, msg_len);
    if (msg_len > buf->left_can_read()){
        LOG("fd %d need read", client_fd);
        return;
    }

    std::string res(buf->get_read_data(), msg_len);
    LOG("from client fd %d read : %s", client_fd, res.c_str());
    buf->reset();
    if (send_max_num ++ < g_total_send_num) {
        std::string send_str = ping_str == res ? pong_str : ping_str;
        sbuffer sb(1024);
        sb.write_int(send_str.size());
        sb.write_data(send_str.c_str(), send_str.size());
        send(client_fd, sb.get_data(), sb.get_data_length(), 0);
    }
}

void handle_new_socket(watcher* w)
{
    int connfd = accept(w->__fd(), (struct sockaddr*)NULL, NULL);
    LOG("get new conn %d", connfd);
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
    #ifdef _WIN32
        WORD sockVersion = MAKEWORD(2,2);
        WSADATA wsaData;
        if(WSAStartup(sockVersion, &wsaData)!=0)
        {
            return 0;
        }
    #endif

    int run_type = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    int reuse = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(int))==-1) {
        perror("setsockopt");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 
    if (run_type_server == run_type) {
        bind(fd, (addr_type)&serv_addr, sizeof(serv_addr));
        listen(fd, 10);
        
        std::string name("handle_new_socket");
        watcher server_watcher(fd, EPOLLIN, 0, handle_new_socket, name);
        default_loop.register_watcher(&server_watcher);
        LOG("start");
    }
    else {
        connect(fd, (addr_type)&serv_addr, sizeof(serv_addr));

        sbuffer sb(1024);
        sb.write_int(ping_str.size());
        sb.write_data(ping_str.data(), ping_str.size());
        send(fd, sb.get_data(), sb.get_data_length(), 0);
        LOG("send size %d", sb.get_data_length());
        std::string name("client");
        watcher client_watcher(fd, EPOLLIN, 0, handle_client_read, name);
        default_loop.register_watcher(&client_watcher);
        LOG("start");
    }
    default_loop.run();
    return 0;
}