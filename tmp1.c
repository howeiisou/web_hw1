#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    char buf[2048];
    int fdimg;
    int on = 1;

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(fd_server, 10);

    while (1)
    {
        fd_client = accept(fd_server, (struct sockaddr *)&client_addr, &sin_len);
        //printf("got connection......\n");
        if (!fork())
        {
            //child process
            close(fd_server);
            memset(buf, 0, 2048);
            read(fd_client, buf, 2047);

            printf("%s\n", buf);

            if (!strncmp(buf, "GET /dong.JPG HTTP/1.1", 22))
            {
                fdimg = open("dong.JPG", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 93000);
                close(fdimg);
            }
            else if (!strncmp(buf, "GET /favicon.ico HTTP/1.1", sizeof("GET /favicon.ico HTTP/1.1") - 1))
            {
                fdimg = open("fav.jpg", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 2000);
                close(fdimg);
            }
            else if (!strncmp(buf, "POST", 4))
            {
                write(fd_client, "HTTP/1.1 100 continue\r\n", 23);
            }
            else /*if(!strncmp(buf, "GET / HTTP/1.1", sizeof("GET / HTTP/1.1") -1))*/
            {
                write(fd_client, "HTTP/1.1 200 OK\r\n", 17);
                fdimg = open("index.html", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 200);
                close(fdimg);
            }
            close(fd_client);
            exit(0);
        }
        close(fd_client);
    }
    return 0;
}
