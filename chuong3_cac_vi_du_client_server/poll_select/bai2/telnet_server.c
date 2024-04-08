#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    fd_set master; 
    fd_set read_fds;  
    int fdmax;       

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(9000);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, MAX_CLIENTS);

    // add the listener to the master set
    FD_SET(sockfd, &master);

    // keep track of the biggest file descriptor
    fdmax = sockfd; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("ERROR on select");
            exit(1);
        }

        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == sockfd) {
                    socklen_t clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                    if (newsockfd < 0) {
                        perror("ERROR on accept");
                    } else {
                        FD_SET(newsockfd, &master); 
                        if (newsockfd > fdmax) {  
                            fdmax = newsockfd;
                        }
                        printf("New connection from %s on socket %d\n", inet_ntoa(cli_addr.sin_addr), newsockfd);
                    }
                } else {
                    // handle data from a client
                    if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
                        if (n == 0) {
                            printf("Socket %d hung up\n", i);
                        } else {
                            perror("ERROR on recv");
                        }
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        for(int j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                if (j != sockfd && j != i) {
                                    if (send(j, buffer, n, 0) == -1) {
                                        perror("ERROR on send");
                                    }
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
