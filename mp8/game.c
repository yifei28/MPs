#include "game.h"
/* The source file game.c implements the functionality of the 2048 game, allowing the user to interact with the program.
   Funtions implemented in this file including make_game, get_cell, and sliding operations are crucial.
   In make_game and remake_game function, struct of game is initialized and its pointer is returned.
   get_cell function provide access to individual cell in cells array by returning the pointer of the designated cell.
   Sliding operations implement w,a,s,d keys' functionality to slide the values on the board.
   Finally, Legal_check will check if the game on board is dead.
*/


game *make_game(int rows, int cols)
/*! Create an instance of a game structure with the given number of rows
and columns, initializing elements to -1 and return a pointer
to it.
*/
{
	// Dynamically allocate memory for game and cells, do not modify
	game *mygame = malloc(sizeof(game));
	mygame->cells = malloc(rows * cols * sizeof(cell));


	// Initialize all other variables in game struct
	mygame->cols = cols;
	mygame->rows = rows;
	mygame->score = 0;


	// Initialize all cells to -1
	for (int i = 0; i < (rows * cols); i++)
	{
		mygame->cells[i] = -1;
	}

	return mygame;
}


void remake_game(game ** _cur_game_ptr,int new_rows,int new_cols)
/*! Given a game structure that is passed by reference, change the
	game structure to have the given number of rows and columns. Initialize
	the score and all elements in the cells to -1. Make sure that any 
	memory previously allocated is not lost in this function.	
*/
{
	/*Frees dynamically allocated memory used by cells in previous game,
	 then dynamically allocates memory for cells in new game.  DO NOT MODIFY.*/
	free((*_cur_game_ptr)->cells);
	(*_cur_game_ptr)->cells = malloc(new_rows*new_cols*sizeof(cell));

	 //YOUR CODE STARTS HERE:  Re-initialize all other variables in game struct
	(*_cur_game_ptr)->cols = new_cols;
	(*_cur_game_ptr)->rows = new_rows;
	(*_cur_game_ptr)->score = 0;


	// Initialize all cells to -1
	for (int i = 0; i < (new_rows * new_cols); i++)
	{
		(*_cur_game_ptr)->cells[i] = -1;
	}

	return;	
}

void destroy_game(game * cur_game)
/*! Deallocate any memory acquired with malloc associated with the given game instance.
    This includes any substructures the game data structure contains. Do not modify this function.*/
{
    free(cur_game->cells);
    free(cur_game);
    cur_game = NULL;
    return;
}

// this is a function that swaps the values at two different cells
void swap(cell *first, cell *second)
{
    cell temp = *first;
    *first = *second;
    *second = temp;
}


cell * get_cell(game * cur_game, int row, int col)
/*! Given a game, a row, and a column, return a pointer to the corresponding
    cell on the game. (See game.h for game data structure specification)
    This function should be handy for accessing game cells. Return NULL
	if the row and col coordinates do not exist.
*/
{
    // check if the given row and col are valid
    if (row <= cur_game->rows && col <= cur_game->cols)
    {
        return (cur_game->cells) + (row * cur_game->cols + col); // if valid, return corresponding index in row_major 1-D array
    }

    return NULL;
}

int move_w(game * cur_game)
/*!Slides all of the tiles in cur_game upwards. If a tile matches with the 
   one above it, the tiles are merged by adding their values together. When
   tiles merge, increase the score by the value of the new tile. A tile can 
   not merge twice in one turn. If sliding the tiles up does not cause any 
   cell to change value, w is an invalid move and return 0. Otherwise, return 1. 
*/
{
    int is_valid = 0; // a flag to indicate whether this is a valid move

    for (int i = 0; i < cur_game->cols; i++)
    {
        int best_empty = cur_game->rows;     // record current first empty index
        int adjacent_index = cur_game->rows; // record the index with value that will be adjacent to the next non-empty value


        for (int j = 0; j < cur_game->rows; j++)
        {
            // if cell is empty and is the current first empty index in col, update best empty index
            if (*get_cell(cur_game, j, i) == -1)
            {
                if (j < best_empty)
                {
                    best_empty = j;
                }
            }
            else
            {
                // update the best value index if this index will be adjacent to the next non-empty value
                if (j < adjacent_index)
                {
                    adjacent_index = j;
                }


                // check if there is a valid adjacent index
                if (adjacent_index < j)
                {
                    // check if the current value and the adjacent value are equivalent
                    if (*get_cell(cur_game, j, i) == *get_cell(cur_game, adjacent_index, i))
                    {
                        // double the value at adjacent index and set the value of current index to -1
                        *get_cell(cur_game, adjacent_index, i) *= 2;
                        *get_cell(cur_game, j, i) = -1;

			cur_game->score += *get_cell(cur_game, adjacent_index, i); 	 // update score
                        j = adjacent_index;             				 // set loop counter j to adjacent_index
                        adjacent_index = cur_game->rows;				 // reset the adjacent index to get the next valid adjacent index
			is_valid = 1;			 				 // update flag
                    }
		    else // update the adjacent value to the current one
                    {
                        adjacent_index = j;
                    }

                }


                // check if there is a valid best empty index
                if (best_empty < j)
                {
                    // swap the next value with the current best empty index
                    swap(get_cell(cur_game, j, i), get_cell(cur_game, best_empty, i));
                    adjacent_index = best_empty; // update adjacent index value
                    j = best_empty;              // set loop counter j to best_empty
                    best_empty = cur_game->rows; // reset best empty index
                    is_valid = 1;                // update flag
                }
            }
        }
    }

    return is_valid;
};

int move_s(game * cur_game) //slide down
{
    int is_valid = 0; // a flag to indicate whether this is a valid move

    for (int i = 0; i < cur_game->cols; i++)
    {
        int best_empty = -1;     // record current last empty index
        int adjacent_index = -1; // record the index with value that will be adjacent to the next non-empty value


        for (int j = cur_game->rows - 1; j >= 0; j--)
        {
            // if cell is empty and is the current last empty index in col, update best empty index
            if (*get_cell(cur_game, j, i) == -1)
            {
                if (j > best_empty)
                {
                    best_empty = j;
                }
            }
            else
            {
                // update the best value index if this index will be adjacent to the next non-empty value
                if (j > adjacent_index)
                {
                    adjacent_index = j;
                }


                // check if there is a valid adjacent index
                if (adjacent_index > j)
                {
                    // check if the current value and the adjacent value are equivalent
                    if (*get_cell(cur_game, j, i) == *get_cell(cur_game, adjacent_index, i))
                    {
                        // double the value at adjacent index and set the value of current index to -1
                        *get_cell(cur_game, adjacent_index, i) *= 2;
                        *get_cell(cur_game, j, i) = -1;

                        cur_game->score += *get_cell(cur_game, adjacent_index, i);       // update score
                        j = adjacent_index;  						 // set loop counter j to adjacent_index
                        adjacent_index = -1; 						 // update adjacent index to get the next valid adjacent index
			is_valid = 1;	    						 // update flag
                    }
                    else // update the adjacent value to the current one
                    {
                        adjacent_index = j;
                    }
                }


                // check if there is a valid best empty index
                if (best_empty > j)
                {
                    // swap the next value with the current best empty index
                    swap(get_cell(cur_game, j, i), get_cell(cur_game, best_empty, i));
                    adjacent_index = best_empty; // update adjacent index value
                    j = best_empty;              // set loop counter j to best_empty
                    best_empty = -1;             // reset best empty index
                    is_valid = 1;                // update flag
                }
            }
        }
    }

    return is_valid;
};

int move_a(game * cur_game) //slide left
{
    int is_valid = 0; // a flag to indicate whether this is a valid move

    for (int i = 0; i < cur_game->rows; i++)
    {
        int best_empty = cur_game->cols;     // record current first empty index
        int adjacent_index = cur_game->cols; // record the index with value that will be adjacent to the next non-empty value

        for (int j = 0; j < cur_game->cols; j++)
        {
            // if cell is empty and is the current first empty index in row, update best empty index
            if (*get_cell(cur_game, i, j) == -1)
            {
                if (j < best_empty)
                {
                    best_empty = j;
                }
            }
            else
            {
                // update the best value index if this index will be adjacent to the next non-empty value
                if (j < adjacent_index)
                {
                    adjacent_index = j;
                }

                // check if there is a valid adjacent index
                if (adjacent_index < j)
                {
                    // check if the current value and the adjacent value are equivalent
                    if (*get_cell(cur_game, i, j) == *get_cell(cur_game, i, adjacent_index))
                    {
                        // double the value at adjacent index and set the value of current index to -1
                        *get_cell(cur_game, i, adjacent_index) *= 2;
                        *get_cell(cur_game, i, j) = -1;

			cur_game->score += *get_cell(cur_game, i, adjacent_index);       // update score
                        j = adjacent_index;             				 // set loop counter j to adjacent_index
                        adjacent_index = cur_game->cols;				 // update adjacent index to get the next valid adjacent index
			is_valid = 1; 							 // update flag
                    }
		    else // update the adjacent value to the current one
		    {
			adjacent_index = j;
		    }

                }

                // check if there is a valid best empty index
                if (best_empty < j)
                {
                    // swap the next value with the current best empty index
                    swap(get_cell(cur_game, i, j), get_cell(cur_game, i, best_empty));
                    adjacent_index = best_empty; // update adjacent index value
                    j = best_empty;              // set loop counter j to best_empty
                    best_empty = cur_game->cols; // reset best empty index
		    is_valid = 1;		 // update flag
                }
            }
        }
    }

    return is_valid;
};

int move_d(game * cur_game) //slide to the right
{
    int is_valid = 0; // a flag to indicate whether this is a valid move

    for (int i = 0; i < cur_game->rows; i++)
    {
        int best_empty = -1;     // record current last empty index
        int adjacent_index = -1; // record the index with value that will be adjacent to the next non-empty value

        for (int j = cur_game->cols - 1; j >= 0; j--)
        {
            // if cell is empty and is the current last empty index in row, update best empty index
            if (*get_cell(cur_game, i, j) == -1)
            {
                if (j > best_empty)
                {
                    best_empty = j;
                }
            }
            else
            {
                // update the best value index if this index will be adjacent to the next non-empty value
                if (j > adjacent_index)
                {
                    adjacent_index = j;
                }

                // check if there is a valid adjacent index
                if (adjacent_index > j)
                {
                    // check if the current value and the adjacent value are equivalent
                    if (*get_cell(cur_game, i, j) == *get_cell(cur_game, i, adjacent_index))
                    {
                        // double the value at adjacent index and set the value of current index to -1
                        *get_cell(cur_game, i, adjacent_index) *= 2;
                        *get_cell(cur_game, i, j) = -1;

                        cur_game->score += *get_cell(cur_game, i, adjacent_index);       // update score
                        j = adjacent_index; 						 // set loop counter j to adjacent_index
                        adjacent_index = -1;						 // update adjacent index to get the next valid adjacent index
			is_valid = 1;	    						 // update flag
                    }
                    else // update the adjacent value to the current one
                    {
                        adjacent_index = j;
                    }
                }

                // check if there is a valid best empty index
                if (best_empty > j)
                {
                    // swap the next value with the current best empty index
                    swap(get_cell(cur_game, i, j), get_cell(cur_game, i, best_empty));
                    adjacent_index = best_empty; // update adjacent index value
                    j = best_empty;              // set loop counter j to best_empty
                    best_empty = -1;             // reset best empty index
		    is_valid = 1;		 // update flag
                }
            }
        }
    }

    return is_valid;
};

int legal_move_check(game * cur_game)
/*! Given the current game check if there are any legal moves on the board. There are
    no legal moves if sliding in any direction will not cause the game to change.
	Return 1 if there are possible legal moves, 0 if there are none.
 */
{
    // check if there is empty space in cells array or possible operation
    // return 1 if yes, that means there is still operation that can change the board
    if (check_empty(cur_game) || check_adjacent(cur_game))
    {
        return 1;
    }

    return 0;
}

// check if there is any empty space in cells array
// if yes, return 1, otherwise 0
int check_empty(game * cur_game){
    for (int i = 0; i < cur_game->rows; i++)
        {
	for (int j = 0; j < cur_game->cols; j++)
	{
	    if (*get_cell(cur_game, i, j) == -1)
	    {
		return 1;
	    }
	}
    }

    return 0;
}

// check if there is any two adjacent cells that have the same value
// if yes, return 1, otherwise 0
int check_adjacent(game * cur_game){
    // vertical check
    for (int i = 0; i < cur_game->cols; i++)
    {
        int current_value = 0;
        for (int j = 0; j < cur_game->rows; j++)
        {
            // check if value at index (j, i) is not -1
            if (*get_cell(cur_game, j, i) != -1)
            {
                // if current value is 0, update
                if (current_value == 0)
                {
                    current_value = *get_cell(cur_game, j, i);
                }
                else // if current value is not 0, compare; if they are the same, return 1
                {
                    if (current_value == *get_cell(cur_game, j, i))
                    {
                        return 1;
                    }
                    else // otherwise update to the current value at index (j, i)
                    {
                        current_value = *get_cell(cur_game, j, i);
                    }
                }
            }
        }
    }

    // horizontal check
    for (int i = 0; i < cur_game->rows; i++)
    {
        int current_value = 0;
        for (int j = 0; j < cur_game->cols; j++)
        {
            // check if value at index (i, j) is not -1
            if (*get_cell(cur_game, i, j) != -1)
            {
                // if current value is 0, update
                if (current_value == 0)
                {
                    current_value = *get_cell(cur_game, i, j);
                }
                else // if current value is not 0, compare; if they are the same, return 1
                {
                    if (current_value == *get_cell(cur_game, i, j))
                    {
                        return 1;
                    }
                    else // otherwise update to the current value at index (i, j)
                    {
                        current_value = *get_cell(cur_game, i, j);
                    }
                }
            }
        }
    }

    return 0;

}



/*! code below is provided and should not be changed */

void rand_new_tile(game * cur_game)
/*! insert a new tile into a random empty cell. First call rand()%(rows*cols) to get a random value between 0 and (rows*cols)-1.
*/
{
    cell * cell_ptr;
    cell_ptr = 	cur_game->cells;
	
    if (cell_ptr == NULL){ 	
        printf("Bad Cell Pointer.\n");
        exit(0);
    }
	
	
	//check for an empty cell
	int emptycheck = 0;
	int i;
	
	for(i = 0; i < ((cur_game->rows)*(cur_game->cols)); i++){
		if ((*cell_ptr) == -1){
				emptycheck = 1;
				break;
		}		
        cell_ptr += 1;
	}
	if (emptycheck == 0){
		printf("Error: Trying to insert into no a board with no empty cell. The function rand_new_tile() should only be called after tiles have succesfully moved, meaning there should be at least 1 open spot.\n");
		exit(0);
	}
	
    int ind,row,col;
	int num;
    do{
		ind = rand()%((cur_game->rows)*(cur_game->cols));
		col = ind%(cur_game->cols);
		row = ind/cur_game->cols;
    } while ( *get_cell(cur_game, row, col) != -1);
        //*get_cell(cur_game, row, col) = 2;
	num = rand()%20;
	if(num <= 1){
		*get_cell(cur_game, row, col) = 4; // 1/10th chance
	}
	else{
		*get_cell(cur_game, row, col) = 2;// 9/10th chance
	}
}

int print_game(game * cur_game) 
{
    cell * cell_ptr;
    cell_ptr = 	cur_game->cells;

    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int i,j;
	
	printf("\n\n\nscore:%d\n",cur_game->score); 
	
	
	printf("\u2554"); // topleft box char
	for(i = 0; i < cols*5;i++)
		printf("\u2550"); // top box char
	printf("\u2557\n"); //top right char 
	
	
    for(i = 0; i < rows; i++){
		printf("\u2551"); // side box char
        for(j = 0; j < cols; j++){
            if ((*cell_ptr) == -1 ) { //print asterisks
                printf(" **  "); 
            }
            else {
                switch( *cell_ptr ){ //print colored text
                    case 2:
                        printf("\x1b[1;31m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 4:
                        printf("\x1b[1;32m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 8:
                        printf("\x1b[1;33m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 16:
                        printf("\x1b[1;34m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 32:
                        printf("\x1b[1;35m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 64:
                        printf("\x1b[1;36m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 128:
                        printf("\x1b[31m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 256:
                        printf("\x1b[32m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 512:
                        printf("\x1b[33m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 1024:
                        printf("\x1b[34m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 2048:
                        printf("\x1b[35m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 4096:
                        printf("\x1b[36m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 8192:
                        printf("\x1b[31m%04d\x1b[0m ",(*cell_ptr));
                        break;
					default:
						printf("  X  ");

                }

            }
            cell_ptr++;
        }
	printf("\u2551\n"); //print right wall and newline
    }
	
	printf("\u255A"); // print bottom left char
	for(i = 0; i < cols*5;i++)
		printf("\u2550"); // bottom char
	printf("\u255D\n"); //bottom right char
	
    return 0;
}

int process_turn(const char input_char, game* cur_game) //returns 1 if legal move is possible after input is processed
{ 
	int rows,cols;
	char buf[200];
	char garbage[2];
    int move_success = 0;
	
    switch ( input_char ) {
    case 'w':
        move_success = move_w(cur_game);
        break;
    case 'a':
        move_success = move_a(cur_game);
        break;
    case 's':
        move_success = move_s(cur_game);
        break;
    case 'd':
        move_success = move_d(cur_game);
        break;
    case 'q':
        destroy_game(cur_game);
        printf("\nQuitting..\n");
        return 0;
        break;
	case 'n':
		//get row and col input for new game
		dim_prompt: printf("NEW GAME: Enter dimensions (rows columns):");
		while (NULL == fgets(buf,200,stdin)) {
			printf("\nProgram Terminated.\n");
			return 0;
		}
		
		if (2 != sscanf(buf,"%d%d%1s",&rows,&cols,garbage) ||
		rows < 0 || cols < 0){
			printf("Invalid dimensions.\n");
			goto dim_prompt;
		} 
		
		remake_game(&cur_game,rows,cols);
		
		move_success = 1;
		
    default: //any other input
        printf("Invalid Input. Valid inputs are: w, a, s, d, q, n.\n");
    }

	
	
	
    if(move_success == 1){ //if movement happened, insert new tile and print the game.
         rand_new_tile(cur_game); 
		 print_game(cur_game);
    } 

    if( legal_move_check(cur_game) == 0){  //check if the newly spawned tile results in game over.
        printf("Game Over!\n");
        return 0;
    }
    return 1;
}
