#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

void handle_client(int client_fd) {
    char buf[256];
    int ret = recv(client_fd, buf, sizeof(buf), 0);
    if (ret <= 0)
        return;
    
    buf[ret] = 0;
    printf("Received from %d: %s\n", client_fd, buf);

    if (strncmp(buf, "GET_TIME ", 9) == 0) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char format[256];
        strcpy(format, buf + 9);

        if (strcmp(format, "dd/mm/yyyy") == 0) {
            sprintf(buf, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        } else if (strcmp(format, "dd/mm/yy") == 0) {
            sprintf(buf, "%02d/%02d/%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year % 100);
        } else if (strcmp(format, "mm/dd/yyyy") == 0) {
            sprintf(buf, "%02d/%02d/%d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
        } else if (strcmp(format, "mm/dd/yy") == 0) {
            sprintf(buf, "%02d/%02d/%02d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year % 100);
        } else {
            strcpy(buf, "Invalid format");
        }

        send(client_fd, buf, strlen(buf), 0);
    }

    close(client_fd);
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    for (int i = 0; i < 8; i++) {
        if (fork() == 0) {
            while (1) {
                int client = accept(listener, NULL, NULL);
                printf("New client accepted: %d\n", client);
                handle_client(client);
            }
            exit(0);
        }
    }

    getchar();
    killpg(0, SIGKILL);

    return 0;
}
