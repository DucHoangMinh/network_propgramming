#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
struct in_addr {
    in_addr_t s_addr; /* địa chỉ IPv4 32 bit */
    /* network byte ordered – big-endian */
};
struct sockaddr_in {
    uint8_t sin_len; /* độ dài cấu trúc địa chỉ (16 bytes) */
    sa_family_t sin_family; /* họ địa chỉ IPv4 - AF_INET */
    in_port_t sin_port; /* giá trị cổng */
    /* network byte ordered */
    struct in_addr sin_addr; /* 32 bit địa chỉ */
    /* network byte ordered */
    char sin_zero[8]; /* không sử dụng */
};
int main(){

}
