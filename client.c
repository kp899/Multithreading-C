#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

#define MAX_MESSAGE_SIZE 2000
#define NUM_MINES 10
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define BORDER_LENGTH 9
#define DEBUG 0
#define TILES_PER_ROW 9

typedef struct {
  int adjacent_mines;
  bool revealed;
  bool is_flagged;
} Tile;

typedef struct {
  Tile tiles[NUM_TILES_X][NUM_TILES_Y];
} GameView;
char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int game_state = -1;
int socket_desc;
struct sockaddr_in server;
char * username;
char * trim_new_line(char * input_str) {
    int len = strlen(input_str);
    input_str[len - 1] = '\0';
    return input_str;
}
int *recv_int_array(int connection, int size) {
    int number_of_bytes, i=0;
    uint32_t value;

	int *results = malloc(sizeof(int)*size);
	for (i=0; i < size; i++) {
		if ((number_of_bytes=recv(connection, &value, sizeof(uint32_t), 0))
		         == -1) {
			perror("recv");
			exit(1);

		}
		results[i] = ntohl(value);
	}
	return results;
}
void send_int_array(int connection, int *array, int size) {
	uint32_t value;
	for (int i = 0; i < size; i++) {
		value = htonl(array[i]);
		send(connection, &value, sizeof(uint32_t), 0);
	}
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
void draw_border() {
	for(int i=0; i < BORDER_LENGTH; i++) printf("========");
	printf("\n");
}
void clear_buffer() {
	while ((getchar()) != '\n');
}
//Converts an integer or
int char_to_int(char ch, char type, int limit) {
  	if (type == 'c') {
  		for(int i=0; alphabet[i]; i++) {
  			if (alphabet[i] == ch) {
  				if ((i % 26) < limit) return (i % 26);
  				return -1;
  			}
  		}
  	} else {
  		return atoi(&ch)-1;
  	}
  	return -1;
}
//Return the numerical value of the alphabet letter
int get_input_letter(int limit) {
	char val = '\0';
	scanf("%c", &val);
	clear_buffer();
	for(int i=0; alphabet[i]; i++) {
			if (alphabet[i] == val) {
				if ((i % 26) < limit) return (i % 26);
				return -1;
			}
	}
	return -1;
}
//takes a number char and return it's integer value-1
int get_input_number() {
	char val = '\0';
	scanf("%c", &val);
	clear_buffer();
	return atoi(&val)-1;
}
//Send a 3 int array to the server
void send_instruction(int connection, int a, int x, int y) {
	int array[3] = {a , x, y};
	send_int_array(connection, array, 3);
}
//Draws the game tiles in the mine field
int draw_gameview(int connection) {
	draw_border();

	//Receive game status from the server
	int *status = recv_int_array(connection, 3);
	if (DEBUG) printf("Recvd Status\n");
	//Get tile values
	int *field = recv_int_array(connection, TILES_PER_ROW*TILES_PER_ROW);
	if (DEBUG) printf("Recvd Tiles\n");

	//Displaying mines left and current game time
	printf("\nRemaining Mines: %d\n", status[1]);

	//Draw mine field
	printf("\n     "); //Print numbers at top
	for (int i=1; i<= TILES_PER_ROW; i++) {
		printf("%d ", i);
	}
	printf("\n "); //Printing the top margin
	for (int i=0; i< 2*TILES_PER_ROW+4; i++) {
		printf("-");
	}
	int val;
	for (int i=0; i< TILES_PER_ROW; i++) {
		printf("\n %c | ", alphabet[i]);
		for (int x=0; x < TILES_PER_ROW; x++) {
			val = field[x+(i*TILES_PER_ROW)];
			if (val == -1) {
				printf("  ");
			} else if (val == -2) {
				printf("+ ");
			} else if (val == -3) {
				printf("* ");
			} else {
				printf("%d ", val);
			}
		}
	}
	printf("\n");
	fflush(stdout);

	//If the game is over, check to see if player won
	if (status[0] == 0) {
		if (status[1] == 0) {
			printf("\nCongradulations! You found all the mines!\n");
			printf("You won in %d seconds!\n", status[2]);
		} else {
			printf("\nGame Over! You hit a mine.\n");
			printf("The game lasted %d seconds.\n", status[2]);
		}
		return 1; //Returns to the main menu
	}

	printf("\nChoose an Option:\n");
	printf("<R> Reveal a Tile\n");
	printf("<P> Place a Flag\n");
	printf("<Q> Quit Game\n");

	free(status);

	//Get input and action game menu
	int input;
		//Get the option selected
		do {
			printf("\nOption (R, P, Q): ");
			input = get_input_letter(26);
			if (DEBUG) printf("Imput value %d\n",input);

			if ((input < 15) || (input > 17)) {//Input is not Q R or P
				printf("Please enter one of the listed options.\n");
				input = -1;
			}
		} while (input == -1);

		//If P was selected - Place Flag
		if (input == 15) {
			int x,y;
			printf("\nPlacing a Flag!");
			do {
				printf("\nEnter the tile coordinates: ");
				char val[2];
				scanf("%2s", val);
				clear_buffer();
				y = char_to_int(val[0],'c',9);
				x = char_to_int(val[1],'n',9);
				if ((x == -1)||(y == -1)) printf("\nPlease enter the coordinates (e.g. A2)");
			} while ((x == -1)||(y == -1));

			//Place flag at x,y.
			send_instruction(connection, 2, x, y);
		}

		//If Q was selected - Quit Game
		if (input == 16) {
			printf("\nReturning to Main Menu...\n");
			return 1;
		}

		//If R was selected - Reveal Tile
		if (input == 17) {
			int x,y;
			printf("\nRevealing a Tile!");
			do {
				printf("\nEnter the tile coordinates: ");
				char val[2];
				scanf("%2s", val);
				clear_buffer();
				y = char_to_int(val[0],'c',9);
				x = char_to_int(val[1],'n',9);
				if ((x == -1)||(y == -1)) printf("\nPlease enter the coordinates (e.g. A2)");
			} while ((x == -1)||(y == -1));

			//Reveal tile at x,y.
			send_instruction(connection, 3, x, y);
		}

	return 2;
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
        if(game_state == 1){
          draw_gameview();
        }
    }
    close(socket_desc);
    return 0;
}
