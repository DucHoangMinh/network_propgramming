#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

// Hàm mã hóa xâu ký tự
void encode(char *message) {
    for (int i = 0; message[i]; i++) {
        if (isalpha(message[i])) {
            message[i] = tolower(message[i]) == 'z' ? 'a' : toupper(message[i]) == 'Z' ? 'A' : message[i] + 1;
        } else if (isdigit(message[i])) {
            message[i] = '9' - (message[i] - '0') + '0';
        }
    }
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_fd, MAX_CLIENTS);

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(listen_fd, &current_sockets);
    int num_of_clients = 0;
    while (1) {
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
            perror("Error on select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == listen_fd) {
                    int client_fd = accept(listen_fd, NULL, NULL);
                    FD_SET(client_fd, &current_sockets);
                    char message[BUFFER_SIZE];
                    num_of_clients ++;
                    sprintf(message, "Xin chào. Hiện có %d clients đang kết nối.\n", num_of_clients);
                    send(client_fd, message, strlen(message), 0);
                } else {
                    char message[BUFFER_SIZE];
                    int len = recv(i, message, BUFFER_SIZE - 1, 0);
                    message[len] = '\0';

                    if (strcmp(message, "exit\n") == 0) {
                        send(i, "Tạm biệt!\n", 10, 0);
                        num_of_clients --;
                        close(i);
                        FD_CLR(i, &current_sockets);
                    } else {
                        encode(message);
                        send(i, message, strlen(message), 0);
                    }
                }
            }
        }
    }

    return 0;
}
