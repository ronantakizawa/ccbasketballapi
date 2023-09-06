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

// Read JSON response from a file
char *readJSONFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening JSON file");
        exit(1);
    }
    // Read through file until end
    fseek(file, 0, SEEK_END);
    long json_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    //Dynamically allocate memory for the response
    char *json_response = (char *)malloc(json_size + 1);
    fread(json_response, 1, json_size, file);
    fclose(file);
    // Null-terminate the received data to work with it as a string
    json_response[json_size] = '\0';
    return json_response;
}

// Create and bind the server socket
int createSocket() {
    //Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    // Set up server address
    //Hosted on IPv4, localhost 3000,
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }
    return server_socket;
}

// Handle Requests
void handleClientRequest(int client_socket, const char *mens_response, const char *womens_response) {
    char request_buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        perror("Error receiving request");
        close(client_socket);
        return;
    }
    //Null terminate request buffer
    request_buffer[bytes_received] = '\0';

    char *http_response = NULL;

    //Set http_response depending on the URL
    if (strstr(request_buffer, "GET /getMens") != NULL) {
        asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", mens_response);
    } else if (strstr(request_buffer, "GET /getWomens") != NULL) {
        asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", womens_response);
    } else {
        asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Colorado College Basketball API</h1> <h4>Methods</h4> <p>/getMens: Get Mens Team Roster</p> <p>/getWomens: Get Womens Team Roster</p></body></html>");
    }

    if (http_response != NULL) {
        send(client_socket, http_response, strlen(http_response), 0);
        free(http_response);
    }

    close(client_socket);
}