// Viết chương trình tcp_client, kết nối đến một máy chủ xác định bởi địa chỉ IP và cổng. 
// Sau đó nhận dữ liệu từ bàn phím và gửi đến server. 
// Tham số được truyền vào từ dòng lệnh có dạng
// tcp_client <địa chỉ IP> <cổng>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

//tcp_client 127.0.0.1 9000
int main(int argc, char *argv[]) 
{

    char *inputIP = argv[2];    
    int inputPort = atoi(argv[3]);

    printf("Địa chỉ IP nhập vào: %s\n", inputIP);
    printf("Cổng nhập vào: %d\n", inputPort);

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //127.0.0.1
    addr.sin_addr.s_addr = inet_addr(inputIP);
    addr.sin_port = htons(inputPort);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char *a = malloc(sizeof(char) * 1024);
    printf("Nhập lời nhắn tới server: ");
    scanf("%s", a);

    send(client, a, strlen(a), 0);

    // Nhận dữ liệu từ server
    char buf[256];
    int ret2 = recv(client, buf, sizeof(buf), 0);
    if (ret2 <= 0) {
        close(client);
        return 0;
    }
    buf[ret2] = 0;

    // In ra lời nhắn server gửi lại
    printf("Lời nhắn từ server: %s\n", buf);

    close(client);

    return 0;
}