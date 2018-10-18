#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>

#define MAXIMUM_MES_SIZE 1000

//Socket variables
int socket_desc, client_socket;
struct sockaddr_in server, client;
// struct to use size for inialisation of client socekt
socklen_t client_addr_size;
// Authorized user Data
char * users;
char * passwords;

// Utility function for sending messages to a client socket
void send_message (int socket, char * str){
    if(send(socket , str , MAXIMUM_MES_SIZE , 0) < 0) {
        puts("Send failed");
    }  
}

// Assign port number
int get_port_no(char* input) {
    int num = 0;
    // Check if the array element is null
    if (input == NULL) {
        printf("\nPort number not entered, port number 12345 has been assigned\n");
        num = 12345;
    }
    if (input != NULL) {
        // check if the port number length is appropriate
        if (strlen(input) < 4 || strlen(input) > 5) {
            puts("Invalid port, number please use a number with a length between 4 and 5 digits");
            exit(EXIT_FAILURE);
        } else {
            // convert string to int
            for(int i = 0; i<strlen(input); i++){   
                num = num * 10 + ( input[i] - '0' );
            }
        }
    }
    return num;
}

void setup_socket(int port) {
    // set up socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    // create socket
    if (socket_desc < 0) {
        printf("Could not create socket\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket created\n");
    }
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET; // set up IPV4 address
    // broadcast on any available IP that belongs to the host machine
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);// set port address to program argument

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
        perror("setsockopt failed");
    }
    // Bind the server to the socket
    if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Error not bind socket\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Address bound to socket\n");
    }
    if(listen(socket_desc, 30) < 0) {
        printf("Error listening on server socket\n");
    } else {
        printf("Waiting for connections.......\n");
    }
}
int main(int argc, char*argv[]) {
    setup_socket(get_port_no(argv[1]));

    while(1) {
			send_message(socket_desc,"hey");
    }

    return 0;
}
