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
//Khai báo địa chỉ trong ứng dụng server
int main(){

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //AF_INET là một hằng số được sử dụng trong lập trình socket để biểu thị họ địa chỉ IPv4.
    addr.sin_addr.s_addr = INADDR_ANY;
    // INADDR_ANY là một hằng số dùng trong lập trình socket để biểu thị địa chỉ bất kỳ, nghĩa là 
    // server sẽ lắng nghe kết nối từ tất cả các địa chỉ IP trên máy tính
    addr.sin_port = htons(9090);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    printf("Dang cho ket noi...\n");
    
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client moi ket noi: %d - IP: %s - Port: %d\n", 
        client, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // char msg[] = "Hello client\n";
    // int ret = send(client, msg, strlen(msg), 0);
    // printf("%d bytes da duoc gui\n", ret);

    // char buf[256];
    // for (int i = 0; i < 10; i++)
    //     buf[i] = i + 97;
    // ret = send(client, buf, 10, 0);
    // printf("%d bytes da duoc gui\n", ret);

    // float f = 3.14;
    // ret = send(client, &f, sizeof(f), 0);
    // printf("%d bytes da duoc gui\n", ret);


    char buf[16];
    memset(buf, 97, sizeof(buf));

    while (1) {
        printf("Dang cho du lieu...\n");
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            printf("Ket noi bi huy");
            return 1;
        }

        if (ret < sizeof(buf))
            buf[ret] = 0;

        printf("So bytes nhan duoc: %d\n", ret);
        printf("Du lieu nhan duoc: %s\n", buf);
    }
    

    return 0;
}