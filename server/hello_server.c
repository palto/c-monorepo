#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include "trim.h"

#define PORT 8080

int server_socket;
sqlite3 *db; // SQLite database handle


// Function declarations
static void handle_signal(int signal);
static void handle_client(int client_socket);
static void hello_server(void);
static void initiate_database(void);
static int store_message(const char *message);

int main()
{
    initiate_database();
    hello_server();
    sqlite3_close(db);
    return 0;
}

static void initiate_database(void)
{
    // Open database
    if (sqlite3_open("server_messages.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "Unable to open database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    // Create table if it doesn't exist
    const char *sql = "CREATE TABLE IF NOT EXISTS messages ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "message TEXT NOT NULL, "
                      "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char *errmsg = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "Unable to create table: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

static void hello_server(void)
{
    // Register signal handler
    signal(SIGINT, handle_signal);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
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

        const int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }

        handle_client(client_socket); // Handle communication
    }
}

static void handle_client(int client_socket)
{
    printf("Client connected\n");
    char buffer[1024];
    ssize_t bytes_read = recv(client_socket, buffer, 1024 - 1, 0);

    if (bytes_read < 0)
    {
        perror("recv");
    }
    else
    {
        buffer[bytes_read] = '\0';
        trim(buffer);
        fprintf(stdout, "Received: %s\n", buffer);

        if (store_message(buffer) != 0 )
        {
            fprintf(stderr, "Failed to store message\n");
        }

        // Open a stream for the client socket
        FILE* client_stream = fdopen(client_socket, "w");
        if (client_stream == NULL)
        {
            perror("fdopen");
            close(client_socket);
            return;
        }

        fprintf(client_stream, "Hello %s!\n", buffer);
        fflush(client_stream); // Ensure the message is sent immediately

        printf("Responding: Hello %.100s!\n", buffer);
        fclose(client_stream); // This will also close the client socket
    }
    printf("Closing connection to client\n");
}

static int store_message(const char *message)
{
    // Save the message into the SQLite database
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO messages (message) VALUES (?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Bind the received message to the prepared statement
    if (sqlite3_bind_text(stmt, 1, message, -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind message: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to insert message: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("Message saved to database.\n");
    }

    sqlite3_finalize(stmt); // Cleanup
    return 0;

}

static void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        fprintf(stderr, "Shutting down server...\n");
        close(server_socket);
        exit(EXIT_SUCCESS);
    }
}
