// 3. Viết chương trình sv_client, cho phép người dùng nhập 
// dữ liệu là thông tin của sinh viên bao gồm MSSV, họ tên, 
// ngày sinh, và điểm trung bình các môn học. Các thông tin 
// trên được đóng gói và gửi sang sv_server. Địa chỉ và cổng 
// của server được nhập từ tham số dòng lệnh.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

//Tham số dòng lệnh có dạng: 127.0.0.1 9000
struct sinh_vien {
    int MSSV;
    char * hoten;
    char * ngaysinh;
    float diemtrungbinh;
};
typedef struct sinh_vien SinhVien;
int main(int argc, char *argv[]){


    char *inputIP = argv[1];    
    int inputPort = atoi(argv[2]);

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

    SinhVien sv;
    sv.hoten = malloc(sizeof(char) * 100);
    sv.ngaysinh = malloc(sizeof(char) * 100);
    printf("Nhap ma so sinh vien: ");scanf("%d", &sv.MSSV);
    printf("Nhap ho va ten: ");scanf("%s",sv.hoten);
    printf("Nhap ngay sinh: ");scanf("%s", sv.ngaysinh);
    printf("Nhap diem trung binh: ");scanf("%f", &sv.diemtrungbinh);

    printf("%d, %s, %s, %f", sv.MSSV, sv.hoten, sv.ngaysinh, sv.diemtrungbinh);

    //Để gửi được sang server, chuyển đổi dữ liệu SinhVien thành một mảng byte
    int size = sizeof(sv);
    char buf[size];
    memcpy(buf, &sv, size);

    int ret2 = send(client, buf, size, 0);
    if(ret2 <= 0){
        perror("send() faild");
        close(client);
        return 1;
    }
    close(client);
    return 0;
}