#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "hello_server.h"

#define PORT 8080

void trim(char *str)
{
    char *start = str;
    char *end;

    // Trim leading whitespace
    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    // If the string is all whitespace
    if (*start == '\0')
    {
        *str = '\0';
        return;
    }

    // Trim trailing whitespace
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))
    {
        end--;
    }

    // Write new null terminator
    *(end + 1) = '\0';

    // Shift trimmed string to the original buffer
    if (start != str)
    {
        memmove(str, start, end - start + 2);
    }
}

void handle_client(int client_socket)
{
    printf("Client connected\n");
    char buffer[1024];
    int bytes_read = recv(client_socket, buffer, 1024 - 1, 0);

    if (bytes_read < 0)
    {
        perror("recv");
    }
    else
    {
        buffer[bytes_read] = '\0';
        trim(buffer);
        fprintf(stdout, "Received: %s\n", buffer);
        char response[1024];
        snprintf(response, 1024, "Hello %.100s!\n", buffer);
        printf("Responding: %s\n", response);
        send(client_socket, response, strlen(response), 0);
    }
    printf("Closing connection to client\n");
    close(client_socket);
}

void hello_server()
{
    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr =
        {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(PORT)};

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        printf("Waiting for connections\n");

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }

        handle_client(client_socket); // Handle communication
    }
}

int main()
{
    hello_server();
}