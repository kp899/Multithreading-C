#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>

#define MAXIMUM_MES_SIZE 2000
#define MAXIMUM_CLIENTS 10

char * process_login(char * input){}

pthread_t consumer_threads[MAXIMUM_CLIENTS], producer_thread;
pthread_mutex_t leader_board_lock, request_que_lock;
pthread_cond_t cond_leader_board;
//Socket variables
int socket_desc, client_socket;
struct sockaddr_in server, client;
// struct to use size for inialisation of client socekt
socklen_t client_addr_size;
// Authorized user Data
char * users;
char * passwords;

int que_length = 0, current_high_scores = 0, file_size = 0, shutdown_threads = 0, users_connected = 0, read_leaderboard = 0;
// Id variables to monitor what thread is doing what
int id[MAXIMUM_CLIENTS];
//delay function
void wait(int ms){
  clock_t start_time = clock();
  while(clock()<start_time + ms){
    ;
  }
}
// Utility function for sending messages to a client socket
void send_message (int socket, char * str){
    if(send(socket , str , MAXIMUM_MES_SIZE , 0) < 0) {
        puts("Send failed");
    }
}

struct request * first_request = NULL;
struct request *last_request = NULL;
struct request{
  int request_id;
  int socket; //client socket descriptor
  struct request * next;
};
// Get a request from the que
struct request * get_request(){
    //lock que
    pthread_mutex_lock(&request_que_lock);

    struct request * a_request;

    if (que_length > 0) {
        a_request = first_request;
        first_request = a_request->next;

        if (first_request == NULL) {
            last_request = NULL;
        }
        que_length --;
    } else {
        a_request = NULL;
    }
    pthread_mutex_unlock(&request_que_lock);
    return a_request;
}
// Add a request to the que
void add_request(int request_id, int socket) {
    struct request * a_request = (struct request*)malloc(sizeof(struct request));
    a_request->request_id = request_id;
    a_request->socket = socket;
    a_request->next = NULL;

    if (que_length == 0) {
        first_request = a_request;
        last_request = a_request;
    } else {
        last_request->next = a_request;
        last_request = a_request;
    }
    que_length ++;
}
void *consumer_handler(void* args){
  int id = *((int *)args);
  int read_size, socket;
  int game_state = -1, finished_sending_list = 0, got_request =0;
  char * user = NULL;
  char * msg = (char *)malloc(1000 * sizeof(char));
  char * client_msg = (char *)malloc(MAXIMUM_MES_SIZE * sizeof(char));
  struct request * client = NULL;
  struct request * temp_request = NULL;
  sprintf(msg,"Thread %i waiting for work",id);
  puts(msg);
  //Run a while loop unless
  while(shutdown_threads == 0){
    temp_request = get_request();
    if(got_request == 0){
      client = (struct request*)malloc(sizeof(struct request));
                client = temp_request;
                socket = client->socket;
                sprintf(msg,"Thread %i handling client %i", id, client->request_id);
                puts(msg);
                got_request = 1;
    }
    if(got_request == 1){
      if((read_size = recv(socket , client_msg , MAXIMUM_MES_SIZE , 0)) > 0 ) {
    // Login mode
    if (game_state == -1) {
      //  char * temp_user = process_login(client_msg);
        /*
        process_login will return a NULL when there is no match if it returns
        a NULL the client is sent a -3 which instructs it to disconnect if the
        return value isn't null then the clients name will be copied to the threads
        memory and the game state will be moved to 0 (menu)
        */
      /*  if (temp_user != NULL) {
            user = (char *)malloc(strlen(temp_user) * sizeof(char));
            strcpy(user, temp_user);
            send_message(socket, "0");
            game_state = 0;
        } else {
            send_message(socket, "-3");
      */  }
      }
    }

  sprintf(msg, "Cleaning thread %i....", id);
  puts(msg);
  shutdown(socket, 2);
  free(user);
  free(msg);
  free(client_msg);
  free(client);
  wait(100000);
  return NULL;
}
}
void *producer_handler(void * args){
  int client_id = 0;
  puts("Producer thread waiting for connections...");
  while(1){
    if(shutdown_threads == 1){
      puts("Cleaning producer thread..");
      break;
    }
    if((client_socket = accept(socket_desc, (struct sockaddr *)&client,&client_addr_size)) > 0) {
        if (users_connected < MAXIMUM_CLIENTS) {
            send_message(client_socket, "-1");
            puts("Connection Accepted");
            add_request(client_id, client_socket);
            client_id ++;
            users_connected ++;
        } else {
            shutdown(client_socket, 2);
            puts("Server full disconnecting client");
            }
      }
  }
  return NULL;
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
// handle ctr+c event
void sigint_handler(int signo) {
    if (signo == SIGINT) {
        shutdown_threads = 1;
        printf("\nTerminate signal detected, closing program\n");

        shutdown(client_socket, 2);
        shutdown(socket_desc, 2);

        for (int i = 0; i < MAXIMUM_CLIENTS; i++ ) {
            if ((pthread_join(consumer_threads[i], NULL)) == 0) {
                wait(10000);
                printf("Worker thread %i successfully terminated\n", i);
            } else {
                printf("Worker thread %i could not terminated\n", i);
            }
        }

        if ((pthread_join(producer_thread, NULL)) == 0) {
            puts("Producer thread successfully terminated\n");
        } else {
            puts("Producer thread could not terminate");
        }

        printf("Exiting application\n");
        exit(EXIT_SUCCESS);
    }
}
void setup_threads() {
    pthread_create(&producer_thread, NULL, &producer_handler, NULL);

    for (int i = 0; i < MAXIMUM_CLIENTS; i++)  {
        id[i] = i;
        pthread_create(&consumer_threads[i], NULL, &consumer_handler, (void*)&id[i]);
    }
}

int main(int argc, char*argv[]) {
    srand(time(NULL));
    signal(SIGINT, sigint_handler);
    setup_socket(get_port_no(argv[1]));
    setup_threads();
    while(1) {
    }
    return 0;
}
