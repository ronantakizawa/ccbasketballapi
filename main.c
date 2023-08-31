#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Read JSON response from file
    FILE *mens = fopen("mens.json", "r");
    if (mens== NULL) {
            perror("Error opening JSON file");
            return 1;
    }
    fseek(mens, 0, SEEK_END);
    long mens_json_size = ftell(mens);
    fseek(mens, 0, SEEK_SET);
    char *mens_response = (char *)malloc(mens_json_size + 1);
    fread(mens_response, 1, mens_json_size, mens);
    fclose(mens);
    mens_response[mens_json_size] = '\0';

    FILE *womens = fopen("womens.json", "r");
    if (womens== NULL) {
            perror("Error opening JSON file");
            return 1;
    }
    fseek(womens, 0, SEEK_END);
    long womens_json_size = ftell(womens);
    fseek(womens, 0, SEEK_SET);
    char *womens_response = (char *)malloc(womens_json_size + 1);
    fread(womens_response, 1, womens_json_size, womens);
    fclose(womens);
    womens_response[womens_json_size] = '\0';

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        return 1;
    }

    printf("Server is listening on port 3000...\n");

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue;
        }

        // Receive request from the client
        char request_buffer[BUFFER_SIZE];
        ssize_t bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            perror("Error receiving request");
            close(client_socket);
            continue;
        }

        // Null-terminate the received data to work with it as a string
        request_buffer[bytes_received] = '\0';

        // Check if the request matches the specified URL
        if (strstr(request_buffer, "GET /getMens") != NULL) {
            // Send modified JSON response
            char *http_response;
            asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", mens_response);
            send(client_socket, http_response, strlen(http_response), 0);
            free(http_response);
        } 
        else if(strstr(request_buffer, "GET /getWomens") != NULL){
            // Send modified JSON response
            char *http_response;
            asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s", womens_response);
            send(client_socket, http_response, strlen(http_response), 0);
            free(http_response);
        }
        
        else {
            // Send original JSON response
            char *http_response;
            asprintf(&http_response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Colorado College Basketball API</h1> <h4>Methods</h4> <p>/getMens: Get Mens Team Roster</p> <p>/getWomens: Get Womens Team Roster</p></body></html>");
            send(client_socket, http_response, strlen(http_response), 0);
            free(http_response);
        }

        // Close client socket
        close(client_socket);
    }

    // Free JSON response memory
    free(mens_response);

    // Close server socket
    close(server_socket);

    return 0;
}
