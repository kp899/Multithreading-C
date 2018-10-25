#include <stdio.h>

//define variables
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

typedef struct Tile
{
	int adjacent_mines;
	bool revealed;
	bool is_mine;
	bool tiles[][];
	bool tile_contains_mine(int x, int y)
	{
		
	bool mine_tile[][];
	

} Tile;
	
typedef struct GameState
{
	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
} GameState;

void place_mines()
{
	for (int i = 0; i < NUM_MINES; i++)
	{
		do
		{
			int x = rand() % NUM_TILES_X;
			int y = rand() % NUM_TILES_Y;
		} while (tile_contains_mine(x, y));
		mine_tile[x][y];
	}
}
int main()
{
	
	char rows[9] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
	int columns[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; 
	int x, y;
	// print remaining mines 
	// printf("Remaining mines" + remainingMines);
	printf("   ");
	for ( int i = 0; i < 9; i++)
	{
		printf("%d ", columns[i]);
	}
	printf("\n");	
	for ( int i = 0; i < 25; i++)
	{
	    	printf("-");
	}
	printf("\n");
	for ( int i = 0; i < 9; i++)
	{
		char supp = rows[i];
	    	printf("%c |\n", supp);
	}
	return 0;
}