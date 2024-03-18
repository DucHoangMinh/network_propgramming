#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

struct file_info {
  char *name;
  long size;
};

struct file_info *files;

int main() 
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }
    
    //Lấy ra tên thư mục hiện tại
    char current_foler[1024];
    getcwd(current_foler, sizeof(current_foler));
    printf("Thư mục hiện tại: %s\n", current_foler);

    //Lấy ra danh sách file trong thư mục hiện tại
    printf("1");
    int num_files = 0;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    printf("2");
    // Mở thư mục hiện tại
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    printf("3");
    // Duyệt qua danh sách file
    while ((entry = readdir(dir)) != NULL) {
    // Lấy thông tin về file
        stat(entry->d_name, &statbuf);

        // Thêm thông tin vào mảng
        files[num_files].name = strdup(entry->d_name);
        files[num_files].size = statbuf.st_size;
        num_files++;
    }

    send(client, files, sizeof(files[0]) * num_files, 0);
    for (int i = 0; i < num_files; i++) {
        free(files[i].name);
    }
    free(files);
    close(client);

    return 0;
}