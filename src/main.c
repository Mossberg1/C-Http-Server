#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "http.h"


#define PORT_NUMBER 8080
#define BACKLOG 10
#define BUFFER_SIZE 1024


char* root_directory = "/Users/mossberg/repos/c_webserver/test";
char* favicon_path = "/Users/mossberg/repos/c_webserver/static/favicon.ico";

const char* notfound_response = 
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char* forbidden_response = 
    "HTTP/1.1 403 Forbidden\r\n"
    "Content-Length: 0\r\n"
    "\r\n";


int main(void) {
    printf("Vällkommen till min webserver i C!\n");

    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd < -1) {
        perror("socket");
        return -1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUMBER);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, BACKLOG) < -1) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Lyssnar på port %d\n", PORT_NUMBER);

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        // Läs request.
        char buffer[BUFFER_SIZE];
        read(client_fd, buffer, BUFFER_SIZE - 1);

        // Parsa request.
        struct http_request request;
        if (http_parse(buffer, &request) == -1) {
            perror("http_parse");
            close(client_fd);
            continue;
        }

        size_t path_length = strlen(root_directory) + strlen(request.path) + 1;
        char path[path_length];

        strlcpy(path, root_directory, path_length);
        strlcat(path, request.path, path_length);

        // Skydda mot Directory Traversal
        char resolved_path[HTTP_PATH_MAXSIZE];
        if (realpath(path, resolved_path) == NULL) {
            send(client_fd, notfound_response, strlen(notfound_response), 0);
            close(client_fd);
            continue;
        }

        if (strncmp(resolved_path, root_directory, strlen(root_directory)) != 0) {
            send(client_fd, forbidden_response, strlen(forbidden_response), 0);
            close(client_fd);
            continue;
        }

        // Om webläsaren förfrågar en favicon, skicka den sen gå vidare till nästa request.
        if (strcmp(request.method, "GET") == 0 && strcmp(request.path, "/favicon.ico") == 0) {
            // Öppna favicon fil.
            int favicon_fd = open(favicon_path, O_RDONLY);
            if (favicon_fd == -1) {
                send(client_fd, notfound_response, strlen(notfound_response), 0);
                close(client_fd);
                continue;
            }

            // Hämta filstorlek
            struct stat favicon_stat;
            fstat(favicon_fd, &favicon_stat);
            long favicon_size = favicon_stat.st_size;

            // Läs favicon till buffer
            char favicon_buffer[favicon_size];
            read(favicon_fd, favicon_buffer, favicon_size);

            close(favicon_fd);

            // Skicka favion till webbläsaren.
            char headers[256];
            sprintf(
                headers,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: image/x-icon\r\n"
                "Content-Length: %ld\r\n"
                "\r\n",
                favicon_size
            );
            send(client_fd, headers, strlen(headers), 0);
            send(client_fd, favicon_buffer, favicon_size, 0);

            close(client_fd);
            continue;
        }

        if (strcmp(request.method, "GET") == 0) {
            struct dirent* directory_entry;
            const bool is_root = strcmp(resolved_path, root_directory) == 0;

            DIR* directory = opendir(resolved_path);
            if (directory) {
                while ((directory_entry = readdir(directory)) != NULL) {
                    if (strcmp(directory_entry->d_name, ".") == 0) {
                        continue;
                    }

                    if (is_root && strcmp(directory_entry->d_name, "..") == 0) {
                        continue;
                    }

                    printf("%s\n", directory_entry->d_name);
                }

                printf("\n");
                closedir(directory);
            }
        }

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 22\r\n"
            "\r\n"
            "<h1>Hello, World!</h1>";
        
        send(client_fd, response, strlen(response), 0);

        close(client_fd);
    }

    return 0;
}
