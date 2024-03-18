// Viết chương trình sv_server, nhận dữ liệu từ sv_client, in ra màn hình 
// và đồng thời ghi vào file sv_log.txt.  Dữ liệu được ghi trên một dòng 
// với mỗi client, kèm theo địa chỉ IP và thời gian client đã gửi. Tham số 
// cổng và tên file log được nhập từ tham số dòng lệnh.
// Ví dụ dữ liệu trong file log:
// 127.0.0.1 2023-04-10 09:00:00 20201234 Nguyen Van A 2002-04-10 3.99
// 127.0.0.1 2023-04-10 09:00:10 20205678 Tran Van B 2002-08-18 3.50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

struct sinh_vien {
  int MSSV;
  char *hoten;
  char *ngaysinh;
  float diemtrungbinh;
};
typedef struct sinh_vien SinhVien;

int main(int argc, char *argv[]) {
    //Tham số dòng lệnh dạng: 9000 sv_log.txt
    int inputPort = atoi(argv[1]);
    char * logFileName = argv[2];
    printf("Cổng nhập vào: %d\n", inputPort);
    printf("Tệp tin ghi lại log: %s\n", logFileName);

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

    while(1){
        int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("Có kết nối mới: %d - IP: %s - Port: %d\n", client, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        SinhVien sv;
        sv.hoten = malloc(sizeof(char) * 100);
        sv.ngaysinh = malloc(sizeof(char) * 100);

        int size = sizeof(sv);
        char buf[size];
        int ret = recv(client, buf, size, 0);
        if (ret <= 0) {
            perror("recv() failed");
            close(client);
            continue;
        }
        //Sao chép dữ liệu từ buffer sang biến sv mình vừa tạo
        printf("----------");
        memcpy(&sv, buf, ret);
        // Ghi thông tin sinh viên vào file log
        FILE *fp = fopen(logFileName, "a");
        if (fp == NULL) {
            perror("fopen() failed");
            close(client);
            continue;
        }

        // Lấy thời gian hiện tại
        time_t currentTime;
        struct tm *timeInfo;
        time(&currentTime);
        timeInfo = localtime(&currentTime);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

        // Ghi dữ liệu vào file log
        fprintf(fp, "%s %d %s %s %.2f\n", inet_ntoa(clientAddr.sin_addr), sv.MSSV, sv.hoten, sv.ngaysinh, sv.diemtrungbinh);
        printf("Đã ghi dữ liệu vào file log\n");
        fclose(fp);

        // Đóng kết nối với client
        close(client);
    }
    close(listener);
    return 0;
}