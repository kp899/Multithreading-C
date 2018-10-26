#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

//define variables
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10
#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

// create an enum for playing, display or quit,
typedef enum {
    _,
    PLAY_MINESWEEPER,
    DISPLAY_LEADERBOARD,
    QUIT
} Menu;

// create a struct for the tile gird
typedef struct Tile {
	// the option of (R, P, Q)
	char *option;
	// the coords chosen after for reveal or place flag
	char row;
	int column;
	// the number from how far the adjacent mines are
	int adjacent_mines;
	// to check if it the tile should be revealed
	// when meeting all conditions
	bool revealed;
	// A boolean true or false if a mine is on that 
	// coord
	bool is_mine;
	// the size of the tiles grid by creating a true or 
	// false for yes to create a grid that will use either
	// + for a flag or * for if hitting a mine
	bool grid[int x][int y] = false;
	
	// the coord the client inputs
	bool ** coord[int x][int y];
	// when creating to check if a mine is on that 
	// coord or not, if false, then turn to true for that coord.
	// create the tile board constructor
	//void tiles[int x][int];

	

} Tile;

// The game state currently on the tiles
typedef struct GameState
{
	// 
	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
}

Tile *newGrid( const int rows, const int columns){
	
	
	Tile *t = malloc(sizeof(Tile));
	
	//allocate the memory
	t.coords = malloc(sizeof(char*));
	t.coords[0] = calloc(sizeof(char), (size_t) rows + 1);
	t.coords[1] = calloc(sizeof(char), (size_t) columns + 1);
	
	// Creates a grid variable based on the length of the rows up to a max of 9.
	char left[rows];
	char grid_left = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	
	// creates column grid variable up to a max of 8
	int top[columns];
	int grid_top[columns] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	// for loop to add them to the array
	for ( int i = 0; i < rows; i++){
		left[i] = grid_left[i];
		
	}
	for ( int i = 0; i < columns; i++){
		top[i] = grid_top[i];
	}
	// A for loop to create the grid.  
	int x, y;
	// print remaining mines 
	// printf("Remaining mines" + remainingMines);
	printf("   ");
	for ( int i = 0; i < 9; i++)
	{
		printf("%d ", grid_top[i]);
	}
	printf("\n");	
	for ( int i = 0; i < 25; i++)
	{
	    	printf("-");
	}
	printf("\n");
	for ( int i = 0; i < 9; i++)
	{
		char supp = grid_left[i];
	    	printf("%c |\n", supp);
	}
	
	return t;
}

void Game_initliase();
void Game_play_minesweeper();
void place_mines();
void tile_contains_mine(char x, int y);
void Free_tile(Tile **t);
void _display_grid();
char _get_option(char option);
char _get_coordinate();
void _print_remaining_mines();
void _display_mine();
void _display_results();
void _display_time();
void _empty_leaderboard();

void Game_choice(const char choice);

#endif // MINESWEEPER_GAME_H

/** 
	Creates a new minesweeper game given the specified inputs. 
	
	@param rows the length of the vertical side of the grid
	@param columns the length of the horizontal side of the grid
	#param option the choice of reveal, place or quit. 
	
	*/
Tile *newGrid( const int rows, const int columns, const char option){
	Tile *t = malloc(sizeof(Tile));
	
	//allocate the memory
	t.coords = malloc(sizeof(char*));
	t.coords[0] = calloc(sizeof(char), (size_t) rows + 1);
	t.coords[1] = calloc(sizeof(char), (size_t) columns + 1);
	
	// Creates a grid variable based on the length of the rows up to a max of 9.
	char left[rows];
	char grid_left = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	
	// creates column grid variable up to a max of 8
	int top[columns];
	int grid_top[columns] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	// for loop to add them to the array
	for ( int i = 0; i < rows; i++){
		left[i] = grid_left[i];
		
	}
	for ( int i = 0; i < columns; i++){
		top[i] = grid_top[i];
	}
	// A for loop to create the grid.  
	int x, y;
	// print remaining mines 
	// printf("Remaining mines" + remainingMines);
	printf("   ");
	for ( int i = 0; i < 9; i++)
	{
		printf("%d ", grid_top[i]);
	}
	printf("\n");	
	for ( int i = 0; i < 25; i++)
	{
	    	printf("-");
	}
	printf("\n");
	for ( int i = 0; i < 9; i++)
	{
		char supp = grid_left[i];
	    	printf("%c |\n", supp);
	}
	
	return t;
}
void Game_play_minesweeper(){
	
}
char _get_option(){
	char input;
	char option;
	bool valid;
	do{
		printf("Option (R, P, Q): ");
		// check input to be valid
		scanf("%s", input);
		switch (input){
			case "R":
			case "r":
				option = input;
				valid = true;
				break;
			case "P":
			case "p":
				option = input;
				valid = true;
				break;
			case "Q":
			case: "q":
				option = input;
				valid = true;
				break;
			default:
				printf("Please input a valid option.");
				valid = false;
		}
		
	} while (valid != true)
	return option;
}
int char_to_int(char c){
	int n = -1; 
	static const char * const alphabet = "ABCEDFGHI";
	char *p = strchr(alphabet, toupper((unsigned char)c));
	if (p)
	{
		n = p - alphabet;
	}
	return n;
	
}
bool _get_coordinate(){
	// This function asks for entering the tile coordinate
	// parses the value to be valid
	// and stores it in a variable coord
	int coord_size, i;
	do {
		printf("Enter tile coordinates: ");
		scanf("%ld", &coord_size);
	} while(coord_size !=2);
	
	char row[coord_size];
	int column[coord_size];
	for ( i = 0; i < coord_size; i++)
	{
		scanf("%c", &row[i]);
		scanf("%ld", &column[i]);
	}
	
	// row[0] should equal the char value
	// column[1] should equal the int value
	// turn the row into an integer for confirming 
	// the coords are at this location
	tiles.coord[char_to_int(row[0])][column[1]] = true;
	// stored into the tiles with the coord the player
	// chose 
	
}

void _chosen_option(){
	// depending on the option
	// it will either reveal, place or quit. 
	char option = _get_option();
	
	
	switch (option){
		case "R":
		case "r":
			// reveal_func();
		// case P, p, and case Q, q, default is quit. 
		// place_func()
		// quit_func()
		
	}
}


bool tile_contains_mine(int x, int y)
{
	// return if the tile on the grid at x and y is 
	return tiles.grid[x][y];
	
}

void place_mines(){
	for (int i = 0; i < NUM_MINES; i++){
		int x, y;
		do
		{
			x = rand() % NUM_TILES_X;
			y = rand() % NUM_TILES_Y
		} while ( tile_contains_mine(x, y) != true);
		tiles.grid[x][y] = true;
	}
}
