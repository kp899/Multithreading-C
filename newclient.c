//CAB 403 Minesweeper Gaming system
//Author Komalpreet Singh, James McIntyre

//Include Header files
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

#define MAXIMUM_MES_SIZE 2000

//Define program variables
int socket_desc; //Socket descriptor
int game_state = -1;
struct sockaddr_in server;
char * username;
// Trims a new line character
char * trim_new_line(char * input_str) {
    int len = strlen(input_str);
    input_str[len - 1] = '\0';
    return input_str;
}
void handle_signal(int  signo){
  if(signo == SIGINT){
    printf("\nSIGINT deteted, exiting program\n");
    close(socket_desc);
    exit(EXIT_FAILURE);
  }
}

//Function to get the port number
int port_number(char* port){
  int number = 0;
  if(port != NULL){
    //perform a check to confirm the format is valid length
    if(strlen(port)<4 || strlen(port)>5){
      printf("Invalid port number");
      exit(EXIT_FAILURE);
    }else{
      //if the length is appropriate convert input to int
      for(int i=0;i<strlen(port);i++){
        number = number * 10 + (port[i] - '0');
      }
    }
  }
  else if(port == NULL){
    printf("Port Number has not been provided\n");
    exit(EXIT_FAILURE);
  }
  return number;
}

//Develop game functions
void login(){
  char *login_msg = (char*)malloc(30 * sizeof(char));
  char *user = (char*)malloc(10* sizeof(char));
  char *password = (char*)malloc(6* sizeof(char));
  char *serverreply = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  char *response = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  char *message = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  printf("Please enter your username\n");
  fgets(username, MAXIMUM_MES_SIZE,stdin);
  printf("PLease enter your password");
  fgets(password, MAXIMUM_MES_SIZE,stdin);
  strcat(message,trim_new_line(username));
  strcat(message, ",");
  strcat(message,trim_new_line(password));
  printf("Please enter your password\n");
  if(send(socket_desc, message, MAXIMUM_MES_SIZE,0)<0){
    puts("Unable to reach server\n Closing");
    close(socket_desc);
    exit(EXIT_SUCCESS);
  }
  //recv reply from server
  if(recv(socket_desc, serverreply, MAXIMUM_MES_SIZE,0)<0){
    printf("recv failed\n");
  }
  if(strcmp(serverreply,"-3")==0){
    printf("Invalid username or password, disconnecting from server");
    close(socket_desc);
    exit(EXIT_SUCCESS);
  }
  //Use string compare function to determine the login
  if(strcmp(serverreply,"0")==0){
    system("clear");
    printf("\n==============================================\n");
    printf("\nWelcome to the Online Minesweeper Gaming System");
    printf("\n===============================================\n\n");
    game_state=0;
  }
}
void game(){
}
void leader_board(){
}

int main(int argc, char *argv[]){
	char * ip = argv[1];
	int port = port_number(argv[2]);
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
      char * reply = (char *)malloc(MAXIMUM_MES_SIZE * sizeof(char));
      read_size = recv(socket_desc , reply , MAXIMUM_MES_SIZE , 0);
      if (read_size <= 0) {
          puts("Server full, closing application");
          shutdown(socket_desc, 2);
          exit(EXIT_FAILURE);
      }
  }
    while(1){
      if(game_state == -1){
        //login();
        printf("%d",&game_state);
            }
      if(game_state==0){
        printf("logged in");
      }
    }
}
