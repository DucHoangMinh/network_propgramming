// Viết chương trình tcp_server, đợi kết nối ở cổng xác định bởi tham số dòng lệnh. 
// Mỗi khi có client kết nối đến, thì gửi xâu chào được chỉ ra trong một tệp tin xác định, sau đó ghi toàn bộ nội dung client 
// gửi đến vào một tệp tin khác được chỉ ra trong tham số dòng lệnh
// tcp_server <cổng> <tệp tin chứa câu chào> <tệp tin lưu nội dung client gửi đến>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    // tcp_server 9000 hello.txt record.txt

    int inputPort = atoi(argv[2]);
    char * helloFileName = argv[3];
    char * recordFileName = argv[4];
    printf("Cổng nhập vào: %d\n", inputPort);
    printf("Tệp tin chứa xâu chào: %s\n", helloFileName);
    printf("Tệp tin ghi nội dung client gửi đến: %s\n", recordFileName);

    //Thực hiện mở và đọc file
    FILE *f = fopen(helloFileName, "r");
    if (f == NULL) {
        printf("Lỗi mở file %s!\n", helloFileName);
        return 1;
    }
    char helloMessage[1024];
    fgets(helloMessage, sizeof(helloMessage), f);
    printf("Nội dung file %s : %s\n",helloFileName, helloMessage);
    fclose(f);

    // Tạo socker cho kết nối
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }
    // Khai báo địa chỉ server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(inputPort);

    // Gán socket với cấu trúc địa chỉ
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyển socket sang trạng thái chờ kết nối
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }
    printf("Đang chờ kết nối...\n");

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    while (1){
        int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("Có kết nối mới: %d - IP: %s - Port: %d\n", client, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Buffer to receive data from client
        char buf[256];

        // Receive data from client
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            close(client);
            continue;
        }
        buf[ret] = 0;
        //Mở file để ghi nội dung client gửi tới vào:
        FILE *fp = fopen(recordFileName, "a");
        if(fp == NULL){
            perror("fopen() failed");
            return 1;
        }
        printf("Lời nhắn từ client: %s\n", buf);
        fflush(fp);
        int written = fprintf(fp,"%s\n", buf);
        if (written < 0) {
            perror("fprintf() failed");
            close(client);
            fclose(fp);
            return 1;
        }
        printf("Đã ghi nội dung trên vào file %s\n", recordFileName);
        fclose(fp);
        int ret2 = send(client, helloMessage, strlen(helloMessage), 0);
        if (ret2 < 0) {
            perror("send() failed");
            close(client);
            return 1;
        }
        close(client);
    }
    fclose(fp);
    fclose(f);
    close(listener);
    return 0;
}