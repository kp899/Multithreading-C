#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

#define MAXIMUM_MES_SIZE 1000

//define game variables
int game_state = -1;
struct sockaddr_in server;
int socket_desc;
char * username;

//login Screen
void login(){
  //defining variables and assigning memory
  username = (char*)malloc(20 * sizeof(char));
  char * password = (char*)malloc(6 * sizeof(char));
  char * reply = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  char * message = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));

  printf("Please enter your username");
  fgets(username,MAXIMUM_MES_SIZE, stdin);
  printf("Please enter your password");
  fgets(password,MAXIMUM_MES_SIZE,stdin);

  if(send(socket_desc, message, MAXIMUM_MES_SIZE,0)<0){
    puts("Problem Communicating with the server\nApplication closing");
    close(socket_desc);
    exit(EXIT_SUCCESS);
  }
  if(recv(socket_desc, reply, MAXIMUM_MES_SIZE,0)<0){
    puts("recv failed");
  }
  if(strcmp(reply,"0") ==0){
    system("clear");
    printf("\n==============================================\n");
    printf("\nWelcome to the Online Minesweeper Gaming System");
    printf("\n===============================================\n\n");
    game_state=0;
  }
  if(strcmp(reply,"-3")==0){
    puts("Invalid log in details, disconnecting");
    close(socket_desc);
    exit(EXIT_SUCCESS);
  }
}
void menu(){
  char * reply = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  char * message = (char*)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  char ** response = malloc(sizeof(char *) * 1);

  while(1){
    printf("Please enter a selection\n\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n");
    printf("\nSelect an option 1-3 -> ");
    fgets(response, MAX_MESSAGE_SIZE, stdin);
    strcpy(message,(response));

    if ((strlen(message)) > 1) {
        puts("Invalid input");
        continue;
    }
    if(strcmp(reply,"1") == 0){
      system("clear");
      game_state = 1;
      break;
    }
    if((reply,"2") == 0){
      system("clear");
      game_state = 1;
    }
    if(strcmp(reply,"3") ==0){
      printf("\nClosing Gaming system\n");
      close(socket_desc);
      exit(EXIT_SUCCESS);
    }
  }
}

int get_port_no(char* input){
  int num = 0;
  if(input != NULL){
    //check port
    if(strlen(input) < 4 || strlen(input) > 5){
      puts("Invalid port");
    }
  }
}
