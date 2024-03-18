#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
// struct in_addr {
//     in_addr_t s_addr; /* địa chỉ IPv4 32 bit */
//     /* network byte ordered – big-endian */
// };
// struct sockaddr_in {
//     uint8_t sin_len; /* độ dài cấu trúc địa chỉ (16 bytes) */
//     sa_family_t sin_family; /* họ địa chỉ IPv4 - AF_INET */
//     in_port_t sin_port; /* giá trị cổng */
//     /* network byte ordered */
//     struct in_addr sin_addr; /* 32 bit địa chỉ */
//     /* network byte ordered */
//     char sin_zero[8]; /* không sử dụng */
// };
int main () {

    int client = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Hàm inet_addr() chuyển địa chỉ IP sang dạng chuỗi số nguyên
    addr.sin_port = htons(9090);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1){
        perror("connect() failed");
        return 1;
    } 

    char message[] = "Hello server\n";
    send(client, message, strlen(message), 0);

    char buf[256];
    while (1) {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        printf("%d bytes received: %s\n", ret, buf);
    }

    close(client); 
}