#include <iostream>
#include <functional>
#include "src/loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/slog.h"
#include "src/socket_buffer.h"
#include "src/win_loop.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define addr_type LPSOCKADDR

loop* default_loop = new win_loop();

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
        default_loop->remove_watcher(w);
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
}

void handle_new_socket(watcher* w)
{
    int connfd = accept(w->__fd(), (struct sockaddr*)NULL, NULL);
    LOG("get new conn %d", connfd);
    std::string name("handle_client_read");
    watcher* client_watcher = new watcher(connfd, 1, 0, handle_client_read, name);
    default_loop->register_watcher(client_watcher);

    socket_buffer* buf = w->get_buffer();
    sbuffer sb(1024);
    sb.write_int(ping_str.size());
    sb.write_data(ping_str.data(), ping_str.size());
    send(connfd, sb.get_data(), sb.get_data_length(), 0);
    LOG("send size %d", sb.get_data_length());
    buf->reset();
}

void client_send_data(watcher* w)
{
    if (NULL == w)
    {
        LOG("numm watcher!!");
        return;
    }
    int client_fd = w->__fd();
    socket_buffer* buf = w->get_buffer();
    LOG("client_fd %d", client_fd);
    sbuffer sb(1024);
    sb.write_int(ping_str.size());
    sb.write_data(ping_str.data(), ping_str.size());
    send(client_fd, sb.get_data(), sb.get_data_length(), 0);
    LOG("send size %d", sb.get_data_length());
    buf->reset();
    handle_client_read(w);
}
enum run_type_input
{
    run_type_server = 1,
    run_type_client,
};

#define PORT 9000
int g_client_num = 1;
bool is_server = true;
int main(int argc, char* argv[]) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (is_server)
    {
        struct sockaddr_in serv_addr;
        int reuse = 1;
        int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(PORT);
        bind(fd, (addr_type)&serv_addr, sizeof(serv_addr));
        listen(fd, 10);

        std::string name("handle_new_socket");
        watcher* server_watcher = new watcher(fd, 1, 0, handle_new_socket, name);
        default_loop->register_watcher(server_watcher);
        LOG("start");
    }
    else
    {
        for (int i = 0; i < g_client_num; ++i) {
            struct sockaddr_in serv_addr;
            int reuse = 1;
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            memset(&serv_addr, '0', sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            serv_addr.sin_port = htons(PORT);
            int res = connect(fd, (addr_type)&serv_addr, sizeof(serv_addr));
            LOG("connect res %d", res);
            std::string name("client");
            watcher* client_watcher = new watcher(fd, 1, 0, handle_client_read, name);
            default_loop->register_watcher(client_watcher);
        }
    }

    default_loop->run();
    return 0;
}