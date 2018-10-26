#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

#define MAX_MESSAGE_SIZE 2000

int game_state = -1;
int socket_desc;
struct sockaddr_in server;
char * username;
char * trim_new_line(char * input_str) {
    int len = strlen(input_str);
    input_str[len - 1] = '\0';
    return input_str;
}
void login_screen() {
    username = (char *)malloc(20 * sizeof(char));
    char * password = (char *)malloc(6 * sizeof(char));
    char * message = (char *)malloc(MAX_MESSAGE_SIZE * sizeof(char));
    char * reply = (char *)malloc(MAX_MESSAGE_SIZE * sizeof(char));

    printf("Please enter your username--> ");
    fgets(username, MAX_MESSAGE_SIZE, stdin);
    printf("Please enter your password--> ");
    fgets(password, MAX_MESSAGE_SIZE, stdin);
    strcat(message,trim_new_line(username));
    strcat(message, ",");
    strcat(message,trim_new_line(password));

    if( send(socket_desc , message , MAX_MESSAGE_SIZE , 0) < 0) {
        puts("Unable to reach server\nClosing application");
        close(socket_desc);
        exit(EXIT_SUCCESS);
    }

    if( recv(socket_desc , reply , MAX_MESSAGE_SIZE , 0) < 0) {
        puts("recv failed");
    }

    if (strcmp(reply, "-3") == 0) {
        puts("Invalid log in details, disconnecting");
        close(socket_desc);
        exit(EXIT_SUCCESS);
    }

    if (strcmp(reply, "0") == 0) {
        system("clear");
        printf("\n===============================================\n");
        printf("\nWelcome to the Online Minesweeper Gaming System\n");
        printf("\n===============================================\n\n\n");
        game_state = 0;
    }
}

void menu() {
    char * message = (char *)malloc(MAX_MESSAGE_SIZE * sizeof(char));
    char * reply = (char *)malloc(MAX_MESSAGE_SIZE * sizeof(char));
    char * response = malloc(sizeof(char *) * 1);

    while(1) {

        printf("Please enter a selection\n\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n");
        printf("\nSelect an option 1-3 -> ");
        fgets(response, MAX_MESSAGE_SIZE, stdin);
        strcpy(message, trim_new_line(response));

        if ((strlen(message)) > 1) {
            puts("Invalid input");
            continue;
        }

        if(send(socket_desc , message , MAX_MESSAGE_SIZE , 0) < 0) {
            puts("Unable to reach server\nClosing application");
            close(socket_desc);
            exit(EXIT_SUCCESS);
        }

        if(recv(socket_desc , reply , MAX_MESSAGE_SIZE , 0) < 0) {
            puts("recv failed");
        }

        if (strcmp(reply, "1") == 0) {
            system("clear");
            game_state = 1;
            break;
        }

        if (strcmp(reply, "2") == 0) {
            game_state = 2;
            break;
        }

        if (strcmp(reply, "3") == 0) {
            printf("\nClosing Minesweeper Gaming system\n");
            close(socket_desc);
            exit(EXIT_SUCCESS);
        }
    }
}

void handle_signit(int sig) { 
    printf("SIGINT detected %d\n", sig);
    printf("Exiting has begun.");
    close(socket_desc);
    exit(EXIT_SUCCESS);
} 

int get_port_no(char* input) {
    int num = 0;
    // Check if the array element is null
    if (input == NULL) {
        printf("Port number has not been provided\n");
        exit(EXIT_FAILURE);
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

int main(int argc, char * argv[]) {
    signal(SIGINT, handle_signit);

    char * ip = argv[1];
    int port = get_port_no(argv[2]);
    int read_size;

    if(ip == NULL) {
        printf("IP has not been provided\n");
        exit(EXIT_FAILURE);
    }

    if ((inet_pton(AF_INET, ip, &(server.sin_addr.s_addr))) == 0) {
        printf("IP is not a valid format\n");
        exit(EXIT_FAILURE);
    }
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
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);// set port address to program argument

    if (connect(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        exit(EXIT_FAILURE);
    } else {
        // Check if the connection to the server is open once connecting
        // if it isn't the server has disconnected from the client
        char * reply = (char *)malloc(MAX_MESSAGE_SIZE * sizeof(char));
        read_size = recv(socket_desc , reply , MAX_MESSAGE_SIZE , 0);
        if (read_size <= 0) {
            puts("Server full, closing application");
            shutdown(socket_desc, 2);
            exit(EXIT_FAILURE);
        }
    }


    while(1) {

        if (game_state == -1) {
            login_screen();
        }

        if (game_state == 0) {
            menu();
        }
    }
    close(socket_desc);
    return 0;
}
