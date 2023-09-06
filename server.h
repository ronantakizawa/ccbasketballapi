#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 3000
#define maxPendingConnections 5

// Read JSON response from a file
char *readJSONFile(const char *filename);

// Create and bind the server socket
int createSocket();

// Handle client requests
void handleClientRequest(int client_socket, const char *mens_response, const char *womens_response);

#endif 