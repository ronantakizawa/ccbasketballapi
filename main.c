#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server.h"

#define BUFFER_SIZE 1024
#define PORT 3000
#define maxPendingConnections 5

int main() {
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    //Get Mens and Womens JSON Fikle
    char *mens_response = readJSONFile("mens.json");
    char *womens_response = readJSONFile("womens.json");

    server_socket = createSocket();

    if (listen(server_socket, maxPendingConnections) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Server is listening on port 3000...\n");


    //Run continuously
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue;
        }
        handleClientRequest(client_socket, mens_response, womens_response);
    }

    // Free JSON response memory
    free(mens_response);
    free(womens_response);

    // Close server socket
    close(server_socket);

    return 0;
}
