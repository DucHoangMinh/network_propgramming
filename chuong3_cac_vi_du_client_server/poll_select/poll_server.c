#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#define MAX_FDS 2048
#define MAX_NAME 256

char client_names[MAX_FDS][MAX_NAME];

int main() {
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

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

    struct pollfd fds[MAX_FDS];
    int nfds = 0;

    // Gan socket listener vao tap tham do
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    nfds++;

    char buf[256];

    while (1) {
        int ret = poll(fds, nfds, 0);
        if (ret == -1) {
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == listener) {
                    // Co ket noi moi
                    // Co ket noi moi
                int client = accept(listener, NULL, NULL);
                if (client >= MAX_FDS) {
                    // Vuot qua so luong toi da
                    close(client);
                } else {
                    // Them vao mang tham do su kien
                    fds[nfds].fd = client;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    printf("New client connected: %d\n", client);

                    // Gui yeu cau nhap ten va id cho client moi
                    char *message = "Please enter your id and name in the format 'client_id: client_name'\n";
                    send(client, message, strlen(message), 0);
                }
                } else {
                    // Co du lieu tu client truyen den
                    int client = fds[i].fd;
                    ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0) {
                        // Ket noi bi dong => Xoa khoi mang
                    } else {
                        // Xu ly du lieu nhan duoc
                        buf[ret] = 0;
                        if (strncmp(buf, "client_id: ", 11) == 0) {
                            strncpy(client_names[client], buf + 11, MAX_NAME);
                            printf("Client %d is now known as %s\n", client, client_names[client]);
                        } else {
                            printf("Received from %d: %s\n", client, buf);

                            // Gui tin nhan den cac client khac
                            for (int j = 1; j < nfds; j++) {
                                if (fds[j].fd != client) {
                                    char message[512];
                                    time_t now = time(NULL);
                                    struct tm *t = localtime(&now);
                                    sprintf(message, "%04d/%02d/%02d %02d:%02d:%02d %s: %s",
                                            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                                            t->tm_hour, t->tm_min, t->tm_sec,
                                            client_names[client], buf);
                                    send(fds[j].fd, message, strlen(message), 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
