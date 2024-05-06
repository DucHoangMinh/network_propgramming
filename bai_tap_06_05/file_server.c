#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

void* thread_proc(void *arg);

int main() 
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    pthread_t thread_id;

    while (1)
    {
        int client = accept(listener, NULL, NULL);
        if (client == -1)
            continue;
    
        int ret = pthread_create(&thread_id, NULL, thread_proc, (void *)&client);
        if (ret != 0) 
        {
            printf("Could not create thread!\n");   
        }

        pthread_detach(thread_id);
        sched_yield();
    }

    return 0;
}

void* thread_proc(void *arg)
{
    int client = *(int *)arg;
    char buf[2048];
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            sprintf(buf, "%s\r\n", dir->d_name);
            send(client, buf, strlen(buf), 0);
        }
        closedir(d);
    }

    // Add a loop to handle multiple requests from the client
    while (1)
    {
        // Receive file name from client
        int len = recv(client, buf, sizeof(buf), 0);
        if (len <= 0)
        {
            break;
        }
        buf[len] = 0;

        // Open the file
        int fd = open(buf, O_RDONLY);
        if (fd == -1)
        {
            sprintf(buf, "ERROR File not found \r\n");
            send(client, buf, strlen(buf), 0);
        }
        else
        {
            // Get file size
            struct stat st;
            fstat(fd, &st);
            int size = st.st_size;

            // Send file size to client
            sprintf(buf, "OK %d\r\n", size);
            send(client, buf, strlen(buf), 0);

            // Send file content to client
            while ((len = read(fd, buf, sizeof(buf))) > 0)
            {
                send(client, buf, len, 0);
            }

            close(fd);
        }
    }

    close(client);
    return NULL;
}
