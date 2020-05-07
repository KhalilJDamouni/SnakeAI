/** snake API for C **/
// need to add the elongating of snek after nomming
#include "snek_api.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define max(a,b)            (((a) > (b)) ? (a) : (b))


int CURR_FRAME = 0;
int SCORE = 0;
int MOOGLE_FLAG = 0;
int MOOGLES_EATEN = 0;
int TIME_OUT = ((BOARD_SIZE * 4) - 4) * CYCLE_ALLOWANCE;


//Given
GameBoard* init_board()
{
	printf("init_board ");
	srand(time(0));
	GameBoard* gameBoard = (GameBoard*)(malloc(sizeof(GameBoard)));

	for (int i = 0; i < BOARD_SIZE; i++){
		for (int j = 0; j < BOARD_SIZE; j++){
			gameBoard->cell_value[i][j] = 0;
			gameBoard->occupancy[i][j] = 0;
		}
	}
	gameBoard->occupancy[0][0] = 1; //snake initialized
	gameBoard->snek = init_snek(0, 0);
	return gameBoard;
}

Snek* init_snek(int a, int b)
{
	printf("init_snek ");
	Snek* snek = (Snek *)(malloc(sizeof(Snek)));

	snek->head = (SnekBlock *)malloc(sizeof(SnekBlock));
	snek->head->coord[x] = a;
	snek->head->coord[y] = b;

	snek->tail = (SnekBlock *)malloc(sizeof(SnekBlock));
	snek->tail->coord[x] = a;
	snek->tail->coord[y] = b;

	snek->tail->next = NULL;
	snek->head->next = snek->tail;
	
	snek->length = 1;

	return snek;
}

int hits_edge(int axis, int direction, GameBoard* gameBoard)
{
	printf("hits_edge ");
	// Replaced gameBoard->snek->head[y] with gameBoard->snek->head->coord[y]
	if (((axis == AXIS_Y) && ((direction == UP   && gameBoard->snek->head->coord[y] + UP < 0)   || (direction == DOWN  && gameBoard->snek->head->coord[y] + DOWN  > BOARD_SIZE - 1)))
	   || (axis == AXIS_X && ((direction == LEFT && gameBoard->snek->head->coord[x] + LEFT < 0) || (direction == RIGHT && gameBoard->snek->head->coord[x] + RIGHT > BOARD_SIZE - 1))))
	{
		return 1;
	} 
	else 
	{
		return 0;
	}

}

int hits_self(int axis, int direction, GameBoard *gameBoard)
{
	printf("hits_self ");
	int new_x, new_y;
	if (axis == AXIS_X){
		new_x = gameBoard->snek->head->coord[x] + direction;
		new_y = gameBoard->snek->head->coord[y];
	} else if (axis == AXIS_Y){
		new_x = gameBoard->snek->head->coord[x];
		new_y = gameBoard->snek->head->coord[y] + direction;
	}
	return gameBoard->occupancy[new_y][new_x]; //1 if occupied
}

/*
int hits_food(int axis, int direction, GameBoard *gameBoard)
{
	GameBoard* board = copy_board(gameBoard);
	advance_frame_fake(axis, direction, board);
	if(is_failure_state(axis, direction, board));
		return
}*/

int time_out()
{
	printf("time_out ");
	return (MOOGLE_FLAG == 1 && CURR_FRAME > TIME_OUT);

}

int is_failure_state(int axis, int direction, GameBoard *gameBoard)
{
	printf("is_failure_state ");
	//Rewriting to be able to tell why it failed
	int ihits_self = hits_self(axis, direction, gameBoard);
	int ihits_edge = hits_edge(axis, direction, gameBoard);
	if(ihits_edge)
		printf("Died from hitting edge\n");
	if(ihits_self)
		printf("Died from hitting self\n");
	//Return true if is failure state.
	return (ihits_self || ihits_edge || time_out());
}

/*int is_failure_state2(int axis, int direction, GameBoard *gameBoard)
{
	//printf("is_failure_state ");
	//Rewriting to be able to tell why it failed
	int ihits_self = hits_self(axis, direction, gameBoard);
	int ihits_edge = hits_edge(axis, direction, gameBoard);
	int ihits_food = hits_food(axis, direction, gameBoard);
	//if(ihits_edge)
		//printf("Died from hitting edge\n");
	//if(ihits_self)
		//printf("Died from hitting self\n");
	//Return true if is failure state.
	return (ihits_self || ihits_edge || time_out());
}*/

void populate_moogles(GameBoard *gameBoard)
{
	printf("populate_moogles ");

	if (MOOGLE_FLAG == 0){
		int r1 = rand() % BOARD_SIZE;
		int r2 = rand() % BOARD_SIZE;
		
		int r3 = rand() % (BOARD_SIZE * 10);
		if (r3 == 0){
			gameBoard->cell_value[r1][r2] = MOOGLE_POINT * HARRY_MULTIPLIER;
			MOOGLE_FLAG = 1;
		} else if (r3 < BOARD_SIZE){
			gameBoard->cell_value[r1][r2] = MOOGLE_POINT;
			MOOGLE_FLAG = 1;
		}
	}
}

void eat_moogle(GameBoard* gameBoard, int head_x, int head_y) 
{
	printf("eat_moogle ");
	
	SCORE = SCORE + gameBoard->cell_value[head_y][head_x];
	gameBoard->cell_value[head_y][head_x] = 0;

	gameBoard->snek->length ++;
	MOOGLES_EATEN ++;
	MOOGLE_FLAG = 0;
	CURR_FRAME = 0;
}

int advance_frame(int axis, int direction, GameBoard *gameBoard)
{
	printf("advance_frame ");
	if (is_failure_state(axis, direction, gameBoard)){
		return 0;
	} else {
		// update the occupancy grid and the snake coordinates
		int head_x, head_y;
		// figure out where the head should now be
		if (axis == AXIS_X) {
			head_x = gameBoard->snek->head->coord[x] + direction;
			head_y = gameBoard->snek->head->coord[y];
		} else if (axis == AXIS_Y){
			head_x = gameBoard->snek->head->coord[x];
			head_y = gameBoard->snek->head->coord[y] + direction;
		}
		
		int tail_x = gameBoard->snek->tail->coord[x];
		int tail_y = gameBoard->snek->tail->coord[y];

		// update the occupancy grid for the head
		gameBoard->occupancy[head_y][head_x] = 1;

		if (gameBoard->snek->length > 1) { //make new head
			SnekBlock *newBlock = (SnekBlock *)malloc(sizeof(SnekBlock));
			newBlock->coord[x] = gameBoard->snek->head->coord[x];
			newBlock->coord[y] = gameBoard->snek->head->coord[y];
			newBlock->next = gameBoard->snek->head->next;

			gameBoard->snek->head->coord[x] = head_x;
			gameBoard->snek->head->coord[y] = head_y;
			gameBoard->snek->head->next = newBlock;
	
			if (gameBoard->cell_value[head_y][head_x] > 0){  //eat something
				eat_moogle(gameBoard, head_x, head_y);
			} else { //did not eat
				//delete the tail
				gameBoard->occupancy[tail_y][tail_x] = 0;
				SnekBlock *currBlock = gameBoard->snek->head;
				while (currBlock->next != gameBoard->snek->tail){
					currBlock = currBlock->next;
				} //currBlock->next points to tail

				currBlock->next = NULL;
				free(gameBoard->snek->tail);
				gameBoard->snek->tail = currBlock;
			}

		} else if ((gameBoard->snek->length == 1) && gameBoard->cell_value[head_y][head_x] == 0){ // change both head and tail coords, head is tail
			gameBoard->occupancy[tail_y][tail_x] = 0;
			gameBoard->snek->head->coord[x] = head_x;
			gameBoard->snek->head->coord[y] = head_y;
			gameBoard->snek->tail->coord[x] = head_x;
			gameBoard->snek->tail->coord[y] = head_y;
			
		} else { //snake is length 1 and eats something
			eat_moogle(gameBoard, head_x, head_y);
			gameBoard->snek->head->coord[x] = head_x;
			gameBoard->snek->head->coord[y] = head_y;
		}

		// update the score and board
		SCORE = SCORE + LIFE_SCORE;
		if (MOOGLE_FLAG == 1){
			CURR_FRAME ++;
		}

		// dequeueulate moogles
		populate_moogles(gameBoard);
		return 1;
	} 
}

void show_board(GameBoard* gameBoard) 
{
	printf("show_board ");
	char throw_away;
	fprintf(stdout, "\033[2J"); // clear terminal ANSI code
	fprintf(stdout, "\033[0;0H"); // reset cursor position
	char blank = 	'-';//32;
	char snek = 	42;
	char moogle = 	88;
	char head = 	79;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (gameBoard->cell_value[i][j] > 0 && gameBoard->cell_value[i][j] != 1) 
			{
				//there be a moogle
				fprintf(stdout, "%c", moogle);
			}
			else if (gameBoard->occupancy[i][j] == 1)
			{
				//snake is here
				if(j == gameBoard->snek->head->coord[x] && i == gameBoard->snek->head->coord[y])
					fprintf(stdout, "%c", head);
				else
					fprintf(stdout, "%c", snek);
			} 
			/*else if (gameBoard->cell_value[i][j] > 0) {
				//there be a moogle
				fprintf(stdout, "%c", moogle);
			}*/
			else 
			{
				//nothing to see here
				fprintf(stdout, "%c", blank);
			}
		} //new line
		fprintf(stdout, "\n");
		
	}

	fprintf(stdout, "\n\n");
	
	if (MOOGLE_FLAG == 1){
		fprintf(stdout, "!..ALERT, MOOGLE IN VICINITY..!\n\n");
	}
	fprintf(stdout, "SCORE: %d\n", SCORE);
	fprintf(stdout, "YOU HAVE EATEN %d MOOGLES\n\n", MOOGLES_EATEN);

	fprintf(stdout, "SNEK HEAD\t(%d, %d)\n", gameBoard->snek->head->coord[x], gameBoard->snek->head->coord[y]);
	fprintf(stdout, "SNEK TAIL\t(%d, %d)\n", gameBoard->snek->tail->coord[x], gameBoard->snek->tail->coord[y]);
	fprintf(stdout, "FOOD COORD\t(%d, %d)\n", get_food_x(gameBoard), get_food_y(gameBoard));
	fprintf(stdout, "LENGTH \t%d\n", gameBoard->snek->length);
	fprintf(stdout, "CURR FRAME %d vs TIME OUT %d\n", CURR_FRAME, TIME_OUT);

	////scanf(" %c", &throw_away);
	fflush(stdout);
}

int get_score() 
{
	printf("get_score ");
	return SCORE;
}

void end_game(GameBoard **board)
{
	printf("end_game ");
	//fprintf(stdout, "\033[2J");
	//fprintf(stdout, "\033[0;0H"); 
	fprintf(stdout, "\n\n\n--!!---GAME OVER---!!--\n\nYour score: %d\n\n\n\n", SCORE);
	fflush(stdout);
	// need to free all allocated memory
	// first snek
	SnekBlock **snekHead = &((*board)->snek->head);
	SnekBlock *curr;
	SnekBlock *prev;
	while ((*snekHead)->next != NULL) {
		curr = *snekHead;
		while (curr->next != NULL){
			prev = curr;
			curr = curr->next;
		}
		prev->next = NULL;
		free(curr);
	}
	free(*snekHead);
	free((*board)->snek);
	free(*board);
}

//Mine

//Note: Maybe change to only require the coordinate of the axis its going to chase. If time allows
queue* create_moves(GameBoard* gameBoard, int axis, int wanted_x, int wanted_y) //Called only if is_food positive.
{
	printf("create_moves ");
	//printf("Wanted: %i, %i\n", wanted_x, wanted_y);
	queue* moves = create_queue();
	int food_x = wanted_x;
	int food_y = wanted_y;

	int x_difference = food_x - gameBoard->snek->head->coord[x];
	int y_difference = food_y - gameBoard->snek->head->coord[y];
	//printf("%i ", x_difference);
	//printf("%i\n", y_difference);
	if(axis == AXIS_X)
	{
		if(x_difference > 0)
			for(int i = 0; i < x_difference; ++i)
				enqueue(moves, RIGHT);
		if(x_difference < 0)
			for(int i = 0; i < abs(x_difference); ++i)
				enqueue(moves, LEFT);
		//moves->size = abs(x_difference);
	}
	else if(axis == AXIS_Y)
	{
		if(y_difference > 0)
			for(int i = 0; i < y_difference; ++i)
				enqueue(moves, DOWN);
		if(y_difference < 0)
			for(int i = 0; i < abs(y_difference); ++i)
				enqueue(moves, UP);
		//moves->size = abs(y_difference);
	}
	
	return moves;
}

queue** create_moves_combined(GameBoard* gameBoard) //Called only if is_food positive.
{
	printf("create_moves_combined ");
	queue* moves_queue = create_queue();
	queue* axis_queue  = create_queue();
	queue** queues = malloc(2 * sizeof(queue*));
	queues[0] = moves_queue;
	queues[1] = axis_queue;

	int food_x = get_food_x(gameBoard);
	int food_y = get_food_y(gameBoard);

	int x_difference = food_x - gameBoard->snek->head->coord[x];
	int y_difference = food_y - gameBoard->snek->head->coord[y];
	//printf("%i ", x_difference);
	//printf("%i\n", y_difference);
	

	if(x_difference > 0)
	{
		for(int i = 0; i < x_difference; ++i)
		{
			enqueue(moves_queue, RIGHT);
			enqueue(axis_queue, AXIS_X);
		}
	}
	if(x_difference < 0)
	{
		for(int i = 0; i < abs(x_difference); ++i)
		{
			enqueue(moves_queue, LEFT);
			enqueue(axis_queue, AXIS_X);
		}
		moves_queue->size = abs(x_difference);
	}

	if(y_difference > 0)
	{
		for(int i = 0; i < y_difference; ++i)
		{
			enqueue(moves_queue, DOWN);
			enqueue(axis_queue, AXIS_Y);
		}
	}
	if(y_difference < 0)
	{
		for(int i = 0; i < abs(y_difference); ++i)
		{
			enqueue(moves_queue, UP);
			enqueue(axis_queue, AXIS_Y);
		}
		moves_queue->size += abs(y_difference);
	}
	
	return queues;
}

int get_food_x(GameBoard* gameBoard) //Get the x coordinate of the food
{
	printf("get_food_x ");
	for(int i = 0; i < BOARD_SIZE; ++i)
		for(int d = 0; d < BOARD_SIZE; ++d) //This is x
			if(gameBoard->cell_value[i][d] != 0)
				return d;
	
	return -1; //No Food
}

int get_food_y(GameBoard* gameBoard)
{
	printf("get_food_y ");
	for(int i = 0; i < BOARD_SIZE; ++i) //This is y
		for(int d = 0; d < BOARD_SIZE; ++d)
			if(gameBoard->cell_value[i][d] > 0)
				return i;
	
	return -1; //No Food
}


int safe_direction(int axis, GameBoard* board)
{
	printf("SAFE!!!!");
	//For x first 
	//Check if left and right are available.
	//If they are, check which can go the most up or down.

	int direction = 0;
	int possibilities = 0;
	if(axis == AXIS_X)
	{
		int right_total = 0;
		int left_total  = 0;

		if(!is_failure_state(AXIS_X, RIGHT, board)) //Check if right possible
		{
			printf("Here1\n");
			direction = RIGHT;
			possibilities += 1;
			GameBoard* RIGHT_board = copy_board(board);
			
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_board);
			right_total += 1;
			while(! is_failure_state(AXIS_X, RIGHT, RIGHT_board))
			{
				right_total += 1;
				advance_frame_fake(AXIS_X, RIGHT, RIGHT_board);
			}
            printf("RIGHT_board");
			clean_board(&RIGHT_board);
		}
		if(!is_failure_state(AXIS_X, LEFT, board)) //Check if left possible
		{
			printf("Here2\n");
			direction = LEFT;
			possibilities += 1;
			GameBoard* LEFT_board = copy_board(board);
			
			advance_frame_fake(AXIS_X, LEFT, LEFT_board);
			left_total += 1;
			while(! is_failure_state(AXIS_X, LEFT, LEFT_board))
			{
				left_total += 1;
				advance_frame_fake(AXIS_X, RIGHT, LEFT_board);
			}
            printf("LEFT_board");
			clean_board(&LEFT_board);
		}
		if(possibilities == 2) //Both left and right are possible
		{
			
			printf("Here3\n");
			//Check right and up
			GameBoard* RIGHT_UP_board = copy_board(board);
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_UP_board);
			int right_up = 0;

			while(! is_failure_state(AXIS_Y, UP, RIGHT_UP_board))
			{
				right_up += 1;
				advance_frame_fake(AXIS_Y, UP, RIGHT_UP_board);
			}
            printf("RIGHT_UP_board");
			clean_board(&RIGHT_UP_board);

			//Check right and down
			GameBoard* RIGHT_DOWN_board = copy_board(board);
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_DOWN_board);
			int right_down = 0;

			while(! is_failure_state(AXIS_Y, DOWN, RIGHT_DOWN_board))
			{
				right_down += 1;
				advance_frame_fake(AXIS_Y, UP, RIGHT_DOWN_board);
			}
            //printf("Am i getting here???");
            printf("RIGHT_DOWN_board");
			clean_board(&RIGHT_DOWN_board);
            //scanf(" %c\n", &throw_away);

			right_total += fmax(right_up, right_down);
			

			//Check left and up
			GameBoard* LEFT_UP_board = copy_board(board);
			advance_frame_fake(AXIS_X, LEFT, LEFT_UP_board);
			int left_up = 0;
			while(! is_failure_state(AXIS_Y, UP, LEFT_UP_board))
			{
				left_up += 1;
				advance_frame_fake(AXIS_Y, UP, LEFT_UP_board);
			}
            printf("RIGHT_DOWN_board");
            clean_board(&LEFT_UP_board);

			//Check left and down
			GameBoard* LEFT_DOWN_board = copy_board(board);
			advance_frame_fake(AXIS_X, LEFT, LEFT_DOWN_board);
			int left_down = 0;
			while(! is_failure_state(AXIS_Y, DOWN, LEFT_DOWN_board))
			{
				left_down += 1;
				advance_frame_fake(AXIS_Y, DOWN, LEFT_DOWN_board);
			}
            printf("LEFT_DOWN_board");
            clean_board(&LEFT_DOWN_board);

			left_total += fmax(left_up, left_down);

			if(left_total > right_total)
				direction = LEFT;
			else
				direction = RIGHT;	
		}
	}


	//For y next 
	//Check if up and down are available.
	//If they are, check which can go the most right or left.

	if(axis == AXIS_Y)
	{	
		int up_total = 0;
		int down_total  = 0;
		if(!is_failure_state(AXIS_Y, UP, board)) //Check if up possible
		{
			printf("Here4\n");
			direction = UP;
			possibilities += 1;
			GameBoard* UP_board = copy_board(board);
			
			advance_frame_fake(AXIS_Y, UP, UP_board);
			up_total += 1;
			while(! is_failure_state(AXIS_Y, UP, UP_board))
			{
				up_total += 1;
				advance_frame_fake(AXIS_Y, UP, UP_board);
			}
			clean_board(&UP_board);
		}
		if(!is_failure_state(AXIS_Y, DOWN, board)) //Check if down possible
		{
			printf("Here5\n");
			direction = DOWN;
			possibilities += 1;
			GameBoard* DOWN_board = copy_board(board);
			
			advance_frame_fake(AXIS_Y, DOWN, DOWN_board);
			down_total += 1;
			while(! is_failure_state(AXIS_Y, DOWN, DOWN_board))
			{
				down_total += 1;
				advance_frame_fake(AXIS_Y, DOWN, DOWN_board);
			}
			clean_board(&DOWN_board);
		}
		if(possibilities == 2) //Both up and down are possible
		{
			printf("Here6\n");
			//Check up and right
			GameBoard* UP_RIGHT_board = copy_board(board);
			advance_frame_fake(AXIS_Y, UP, UP_RIGHT_board);
			int up_right = 0;

			while(! is_failure_state(AXIS_X, RIGHT, UP_RIGHT_board))
			{
				up_right += 1;
				advance_frame_fake(AXIS_X, RIGHT, UP_RIGHT_board);
			}
			clean_board(&UP_RIGHT_board);

			//Check up and left
			GameBoard* UP_LEFT_board = copy_board(board);
			advance_frame_fake(AXIS_X, UP, UP_LEFT_board);
			int up_left = 0;

			while(! is_failure_state(AXIS_Y, LEFT, UP_LEFT_board))
			{
				up_left += 1;
				advance_frame_fake(AXIS_Y, LEFT, UP_LEFT_board);
			}
			clean_board(&UP_LEFT_board);

			up_total += fmax(up_left, up_right);
			

			//Check down and right
			GameBoard* DOWN_RIGHT_board = copy_board(board);
			advance_frame_fake(AXIS_X, DOWN, DOWN_RIGHT_board);
			int down_right = 0;
			while(! is_failure_state(AXIS_Y, RIGHT, DOWN_RIGHT_board))
			{
				down_right += 1;
				advance_frame_fake(AXIS_Y, RIGHT, DOWN_RIGHT_board);
			}
			clean_board(&DOWN_RIGHT_board);

			//Check down and left
			GameBoard* DOWN_LEFT_board = copy_board(board);
			advance_frame_fake(AXIS_X, DOWN, DOWN_LEFT_board);
			int down_left = 0;
			while(! is_failure_state(AXIS_Y, LEFT, DOWN_LEFT_board))
			{
				down_left += 1;
				advance_frame_fake(AXIS_Y, LEFT, DOWN_LEFT_board);
			}
			clean_board(&DOWN_LEFT_board);

			down_total += fmax(down_right, down_left);

			if(down_total > up_total)
				direction = DOWN;
			else
				direction = UP;	
		}
	}

	return direction;
}

/*
int safe_direction(int axis, GameBoard* board)
{
	printf("SAFE!!!!");
	//For x first 
	//Check if left and right are available.
	//If they are, check which can go the most up or down.

	int direction = 0;
	int possibilities = 0;
	if(axis == AXIS_X)
	{
		int right_total = 0;
		int left_total  = 0;

		if(!is_failure_state(AXIS_X, RIGHT, board)) //Check if right possible
		{
			printf("Here1\n");
			direction = RIGHT;
			possibilities += 1;
			GameBoard* RIGHT_board = copy_board(board);
			
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_board);
			right_total += 1;
			while(! is_failure_state(AXIS_X, RIGHT, RIGHT_board))
			{
				right_total += 1;
				advance_frame_fake(AXIS_X, RIGHT, RIGHT_board);
			}
			clean_board(&RIGHT_board);
		}
		if(!is_failure_state(AXIS_X, LEFT, board)) //Check if left possible
		{
			printf("Here2\n");
			direction = LEFT;
			possibilities += 1;
			GameBoard* LEFT_board = copy_board(board);
			
			advance_frame_fake(AXIS_X, LEFT, LEFT_board);
			left_total += 1;
			while(! is_failure_state(AXIS_X, LEFT, LEFT_board))
			{
				left_total += 1;
				advance_frame_fake(AXIS_X, RIGHT, LEFT_board);
			}
			clean_board(&LEFT_board);
		}
		if(possibilities == 2) //Both left and right are possible
		{
			
			printf("Here3\n");
			//Check right and up
			GameBoard* RIGHT_UP_board = copy_board(board);
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_UP_board);
			int right_up = 0;

			while(! is_failure_state(AXIS_Y, UP, RIGHT_UP_board))
			{
				right_up += 1;
				advance_frame_fake(AXIS_Y, UP, RIGHT_UP_board);
			}
			
			clean_board(&RIGHT_UP_board);
			//Check right and down
			GameBoard* RIGHT_DOWN_board = copy_board(board);
			advance_frame_fake(AXIS_X, RIGHT, RIGHT_DOWN_board);
			int right_down = 0;

			while(! is_failure_state(AXIS_Y, DOWN, RIGHT_UP_board))
			{
				right_down += 1;
				advance_frame_fake(AXIS_Y, UP, RIGHT_UP_board);
			}

			clean_board(&RIGHT_DOWN_board);

			right_total += fmax(right_up, right_down);
			

			//Check left and up
			GameBoard* LEFT_UP_board = copy_board(board);
			advance_frame_fake(AXIS_X, LEFT, LEFT_UP_board);
			int left_up = 0;
			while(! is_failure_state(AXIS_Y, UP, LEFT_UP_board))
			{
				left_up += 1;
				advance_frame_fake(AXIS_Y, UP, LEFT_UP_board);
			}

			//Check left and down
			GameBoard* LEFT_DOWN_board = copy_board(board);
			advance_frame_fake(AXIS_X, LEFT, LEFT_DOWN_board);
			int left_down = 0;
			while(! is_failure_state(AXIS_Y, DOWN, LEFT_DOWN_board))
			{
				left_down += 1;
				advance_frame_fake(AXIS_Y, DOWN, LEFT_DOWN_board);
			}

			left_total += fmax(left_up, left_down);

			if(left_total > right_total)
				direction = LEFT;
			else
				direction = RIGHT;	
		}
	}


	//For y next 
	//Check if up and down are available.
	//If they are, check which can go the most right or left.

	if(axis == AXIS_Y)
	{	
		int up_total = 0;
		int down_total  = 0;
		if(!is_failure_state(AXIS_Y, UP, board)) //Check if up possible
		{
			printf("Here4\n");
			direction = UP;
			possibilities += 1;
			GameBoard* UP_board = copy_board(board);
			
			advance_frame_fake(AXIS_Y, UP, UP_board);
			up_total += 1;
			while(! is_failure_state(AXIS_Y, UP, UP_board))
			{
				up_total += 1;
				advance_frame_fake(AXIS_Y, UP, UP_board);
			}
			clean_board(&UP_board);
		}
		if(!is_failure_state(AXIS_Y, DOWN, board)) //Check if down possible
		{
			printf("Here5\n");
			direction = DOWN;
			possibilities += 1;
			GameBoard* DOWN_board = copy_board(board);
			
			advance_frame_fake(AXIS_Y, DOWN, DOWN_board);
			down_total += 1;
			while(! is_failure_state(AXIS_Y, DOWN, DOWN_board))
			{
				down_total += 1;
				advance_frame_fake(AXIS_Y, DOWN, DOWN_board);
			}
			clean_board(&DOWN_board);
		}
		if(possibilities == 2) //Both up and down are possible
		{
			printf("Here6\n");
			//Check up and right
			GameBoard* UP_RIGHT_board = copy_board(board);
			advance_frame_fake(AXIS_Y, UP, UP_RIGHT_board);
			int up_right = 0;

			while(! is_failure_state(AXIS_X, RIGHT, UP_RIGHT_board))
			{
				up_right += 1;
				advance_frame_fake(AXIS_X, RIGHT, UP_RIGHT_board);
			}
			clean_board(&UP_RIGHT_board);

			//Check up and left
			GameBoard* UP_LEFT_board = copy_board(board);
			advance_frame_fake(AXIS_X, UP, UP_LEFT_board);
			int up_left = 0;

			while(! is_failure_state(AXIS_Y, LEFT, UP_LEFT_board))
			{
				up_left += 1;
				advance_frame_fake(AXIS_Y, LEFT, UP_LEFT_board);
			}
			clean_board(&UP_LEFT_board);

			up_total += fmax(up_left, up_right);
			

			//Check down and right
			GameBoard* DOWN_RIGHT_board = copy_board(board);
			advance_frame_fake(AXIS_X, DOWN, DOWN_RIGHT_board);
			int down_right = 0;
			while(! is_failure_state(AXIS_Y, RIGHT, DOWN_RIGHT_board))
			{
				down_right += 1;
				advance_frame_fake(AXIS_Y, RIGHT, DOWN_RIGHT_board);
			}
			clean_board(&DOWN_RIGHT_board);

			//Check down and left
			GameBoard* DOWN_LEFT_board = copy_board(board);
			advance_frame_fake(AXIS_X, DOWN, DOWN_LEFT_board);
			int down_left = 0;
			while(! is_failure_state(AXIS_Y, LEFT, DOWN_LEFT_board))
			{
				down_left += 1;
				advance_frame_fake(AXIS_Y, LEFT, DOWN_LEFT_board);
			}
			clean_board(&DOWN_LEFT_board);

			down_total += fmax(down_right, down_left);

			if(down_total > up_total)
				direction = DOWN;
			else
				direction = UP;	
		}
	}

	return direction;
}*/

queue** combine_queues(queue* x_moves, queue* y_moves, GameBoard* board)
{
	printf("combine_queues ");
	//Create Fake Board
	GameBoard* fake_board = copy_board(board);

	//Creating the combined queue.
	queue* moves_queue = create_queue();
	queue* axis_queue  = create_queue();
	queue** queues = malloc(2 * sizeof(queue*));
	int direction;
	queues[0] = moves_queue;
	queues[1] = axis_queue;
	
	//Combined the two, account for running into itself or edge
	int queued = 1;
	while((x_moves->size > 0 || y_moves->size > 0) && queued != 0) //Check which is the better move x or y
	{
		int old_size = moves_queue->size;
		old_size = -1;

		queued = next_move(x_moves, y_moves, moves_queue, axis_queue, fake_board, x);
		printf("Got qeueud %i\n",queued);

		//if(queued == 0)
			//scanf(" %c\n", &throw_away);
		/*
		if(!queued && (x_moves->size > 0 || y_moves->size > 0))
		{
			//Try the next safest
			direction = -1 * safe_direction(AXIS_X, fake_board);
			if(direction == 0 || is_failure_state(AXIS_X, direction, fake_board)) 
			{
				direction = -1 * safe_direction(AXIS_Y, fake_board);
				if(direction == 0 || is_failure_state(AXIS_Y, direction, fake_board)) //Going to die
				{
					//Try to get as many points as possible.
					DEAD = 1;
					enqueue(moves_queue, 1);
					enqueue(axis_queue, 1) ;
					advance_frame_fake(1, 1, fake_board);
					queued = 1;
					return queues;
				}
				else if (direction != 0 && !is_failure_state(AXIS_Y, direction, fake_board))
				{
					DEAD = 0;
					enqueue(moves_queue, direction);
					enqueue(axis_queue, AXIS_Y) ;
					enqueue(y_moves, -1 * direction);
					advance_frame_fake(AXIS_Y, direction, fake_board);
				}
			}
			else if(direction != 0 && !is_failure_state(AXIS_X, direction, fake_board))
			{
				enqueue(moves_queue, direction);
				enqueue(axis_queue, AXIS_X);
				enqueue(x_moves, -1 * direction);
				advance_frame_fake(AXIS_X, direction, fake_board);
			}
		}*/

		/*if(!queued) //Neither direct directions work. This is the part that is not working.
		{
			printf("Going a safe direction\n");
			//scanf(" %c", &throw_away);
			//Try X
			direction = safe_direction(AXIS_X, fake_board);
			if(direction != 0) //This works
			{
				printf("Took a safe way X ");
				advance_frame_fake(AXIS_X, direction, fake_board);
				show_board(fake_board);
				enqueue(x_moves, -1 * direction);
				enqueue(moves_queue, direction);
				enqueue(axis_queue, AXIS_X);
				queued = 1;

			}
			if(direction == 0) //X Didnt Work, Try Y
			{
				direction = safe_direction(AXIS_Y, fake_board);
				if(direction != 0)
				{			
					printf("Took a safe way Y ");
					advance_frame_fake(AXIS_Y, direction, fake_board);
					show_board(fake_board);
					enqueue(y_moves, -1 * direction);
					enqueue(moves_queue, direction);
					enqueue(axis_queue, AXIS_Y);
					queued = 1;
				}
			}
			if(direction == 0) //Nothing works, going to die, exiting to avoid inf loop.
			{
				printf("KMS\n");
				DEAD = 1;
				enqueue(moves_queue, 1);
				enqueue(axis_queue, AXIS_Y);
				return queues;
			}
		}*/
		//scanf(" %c", &throw_away);
	}
	
	if(queued == 0) //No safe directions, exit to avoid inf loop
	{
		DEAD = 1;
		enqueue(moves_queue, 1);
		enqueue(axis_queue, AXIS_Y);
		clean_board(&fake_board);

		printf("AAAH IM HERE\n\n\n\n\n\n\n\n\n");
		delete_queue(x_moves);
		delete_queue(y_moves);
		free(x_moves);
		free(y_moves);

		return queues;
	}

	//Take the snake to the wall in a safe way

	clean_board(&fake_board);
	delete_queue(x_moves);
	delete_queue(y_moves);
	free(x_moves);
	free(y_moves);
	return queues;
}

int next_move(queue* x_moves, queue* y_moves, queue* moves_queue, queue* axis_queue, GameBoard* board, int last)
{
	printf("next_move\n");
	int queued = 0;
	int direction = 0;
	int axis = 0;

	show_board(board);
	display_queue(moves_queue);
	display_queue(x_moves);
	display_queue(y_moves);
	//scanf(" %c", &throw_away);

	GameBoard* instance_board = copy_board(board); //HEY U SHOULD JUST BE WAKING UP I THINK THE PROBLEM IS WITH THE TABLE!!!!
	if(x_moves->size > 0 && !is_failure_state(AXIS_X, x_moves->top->value, board))
	{
		printf("Adding a x\n");
		enqueue(moves_queue, dequeue(x_moves));
		enqueue(axis_queue , AXIS_X);
		advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

		queued = next_move(x_moves, y_moves, moves_queue ,axis_queue, board, x);

		if(!queued) //Undo this block
		{
			printf("Undoing\n");
			undo(x_moves, pop(moves_queue));
			pop(axis_queue);
			
			board = instance_board;
			
			show_board(board);
			display_queue(moves_queue);
			display_queue(x_moves);
			display_queue(y_moves);
		}
	}

	else if(!queued && x_moves->size > 0 && !is_failure_state(AXIS_X, -1 * x_moves->top->value, board))
	{
		enqueue(moves_queue, -1 * x_moves->top->value);
		enqueue(x_moves, x_moves->top->value);
		enqueue(axis_queue , AXIS_X);
		advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

		queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

		if(!queued) //Undo this block
		{
			printf("Undoing\n");
			pop(moves_queue);
			pop(x_moves);
			pop(axis_queue);
			
			board = instance_board;

			show_board(board);
			display_queue(moves_queue);
			display_queue(x_moves);
			display_queue(y_moves);

		}
	}

	//instance_board = copy_board(board);
	else if(y_moves->size != 0 && is_failure_state(AXIS_Y, y_moves->top->value, board) && is_failure_state(AXIS_Y, -1 * y_moves->top->value, board))// && !queued)
	{
		direction = safe_direction(AXIS_X, board);

		if(direction != 0)
		{
			enqueue(moves_queue, direction);
			enqueue(x_moves, -1 * direction);
			enqueue(axis_queue , AXIS_X);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);
			
			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(x_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}
		else if(direction == 0)
		{
			clean_board(&instance_board);
			return queued;
		}

		/*
		if(!is_failure_state(AXIS_X, 1, board))
		{
			enqueue(moves_queue, 1);
			enqueue(x_moves, 1);
			enqueue(axis_queue , AXIS_X);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(x_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}
		else if(!is_failure_state(AXIS_X, -1, board))
		{
			enqueue(moves_queue, -1);
			enqueue(x_moves, -1);
			enqueue(axis_queue , AXIS_X);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(x_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}*/
		
	}
	
	//clean_board(&instance_board);
	//instance_board = copy_board(board);
	if(y_moves->size > 0 && !is_failure_state(AXIS_Y, y_moves->top->value, board) && !queued)
	{
		printf("Adding a y\n");
		enqueue(moves_queue, dequeue(y_moves));
		enqueue(axis_queue , AXIS_Y);
		advance_frame_fake(AXIS_Y, moves_queue->bottom->value, board);

		queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, y);

		if(!queued) //Undo this block
		{
			printf("Undoing\n");
			undo(y_moves, pop(moves_queue));
			pop(axis_queue);
			
			board = instance_board;

			show_board(board);
			display_queue(moves_queue);
			display_queue(x_moves);
			display_queue(y_moves);

		}
	}

	else if(!queued && y_moves->size > 0 && !is_failure_state(AXIS_Y, -1 * y_moves->top->value, board) && !queued)
	{
		enqueue(moves_queue, -1 * y_moves->top->value);
		enqueue(y_moves, y_moves->top->value);
		enqueue(axis_queue , AXIS_Y);
		advance_frame_fake(AXIS_Y, moves_queue->bottom->value, board);

		queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

		if(!queued) //Undo this block
		{
			printf("Undoing\n");
			pop(moves_queue);
			pop(y_moves);
			pop(axis_queue);
			
			board = instance_board;

			show_board(board);
			display_queue(moves_queue);
			display_queue(x_moves);
			display_queue(y_moves);

		}
	}

	//instance_board = copy_board(board);
	else if(x_moves->size != 0 && is_failure_state(AXIS_X, x_moves->top->value, board) && is_failure_state(AXIS_X, -1 * x_moves->top->value, board) && !queued)// && !queued)
	{

		direction = safe_direction(AXIS_Y, board);

		if(direction != 0)
		{
			enqueue(moves_queue, direction);
			enqueue(y_moves, -1 * direction);
			enqueue(axis_queue , AXIS_Y);
			advance_frame_fake(AXIS_Y, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);
			
			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(y_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}
		else if(direction == 0)
		{
			clean_board(&instance_board);
			return queued;
		}
		/*direction = safe_direction(AXIS_Y, board);
	
		if(direction != 0)
		{
			enqueue(moves_queue, direction);
			enqueue(x_moves, 1);
			enqueue(axis_queue , AXIS_Y);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);
		}*/
		/*
		if(!is_failure_state(AXIS_Y, 1, board)) //Try using safe direction here instead.
		{
			enqueue(moves_queue, 1);
			enqueue(x_moves, 1);
			enqueue(axis_queue , AXIS_Y);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(y_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}

		else if(!is_failure_state(AXIS_Y, -1, board))
		{
			enqueue(moves_queue, -1);
			enqueue(x_moves, -1);
			enqueue(axis_queue , AXIS_Y);
			advance_frame_fake(AXIS_X, moves_queue->bottom->value, board);

			queued = next_move(x_moves, y_moves, moves_queue, axis_queue, board, x);

			if(!queued) //Undo this block
			{
				printf("Undoing\n");
				pop(moves_queue);
				pop(y_moves);
				pop(axis_queue);
				
				board = instance_board;

				show_board(board);
				display_queue(moves_queue);
				display_queue(x_moves);
				display_queue(y_moves);

			}
		}*/
	
	}

	/*
	if(! queued) //Neither direct directions work. This is the part that is not working.
	{
		printf("Going a safe direction\n");
		//scanf(" %c", &throw_away);
		//Try X
		{
			direction = safe_direction(AXIS_X, board);
			if(direction != 0) //This works
			{
				printf("Took a safe way X ");
				advance_frame_fake(AXIS_X, direction, board);
				show_board(board);
				enqueue(x_moves, -1 * direction);
				enqueue(moves_queue, direction);
				enqueue(axis_queue, AXIS_X);
				queued = 1;
			}
		}
		if(direction == 0) //X Didnt Work, Try Y
		{
			direction = safe_direction(AXIS_Y, board);
			if(direction != 0)
			{			
				printf("Took a safe way Y ");
				advance_frame_fake(AXIS_Y, direction, board);
				show_board(board);
				enqueue(y_moves, -1 * direction);
				enqueue(moves_queue, direction);
				enqueue(axis_queue, AXIS_Y);
				queued = 1;
			}
		}
		if(direction == 0) //Nothing works, going to die, exiting to avoid inf loop.
		{				
			printf("KMS\n");
			DEAD = 1;
			enqueue(moves_queue, 1);
			enqueue(axis_queue, AXIS_Y);
			return 0;
		}	
		
		//scanf(" %c", &throw_away);
	}*/

	clean_board(&instance_board);

	if(x_moves->size == 0 && y_moves->size == 0)
		return 1;

	else 
		return queued;
}


int advance_frame_fake(int axis, int direction, GameBoard *gameBoard)
{	
	printf("advance_frame_fake ");
	printf("Axis| %i\tDirection| %i", axis, direction);
	// update the occupancy grid and the snake coordinates
	int head_x, head_y;
	// figure out where the head should now be
	if (axis == AXIS_X) {
		head_x = gameBoard->snek->head->coord[x] + direction;
		head_y = gameBoard->snek->head->coord[y];
	} 
	else if (axis == AXIS_Y)
	{
		head_x = gameBoard->snek->head->coord[x];
		head_y = gameBoard->snek->head->coord[y] + direction;
	}
		
	int tail_x = gameBoard->snek->tail->coord[x];
	int tail_y = gameBoard->snek->tail->coord[y];

	// update the occupancy grid for the head
	gameBoard->occupancy[head_y][head_x] = 1;

	if (gameBoard->snek->length > 1) 
	{   //make new head
		SnekBlock *newBlock = (SnekBlock *)malloc(sizeof(SnekBlock));
		newBlock->coord[x] = gameBoard->snek->head->coord[x];
		newBlock->coord[y] = gameBoard->snek->head->coord[y];
		newBlock->next = gameBoard->snek->head->next;

		gameBoard->snek->head->coord[x] = head_x;
		gameBoard->snek->head->coord[y] = head_y;
		gameBoard->snek->head->next = newBlock;
	
		if(gameBoard->cell_value[head_y][head_x] > 0)
		{
			int ignore = 0;
		}
		else 
		{ 		//did not eat
				//delete the tail
			gameBoard->occupancy[tail_y][tail_x] = 0;
			SnekBlock *currBlock = gameBoard->snek->head;
			while (currBlock->next != gameBoard->snek->tail)
			{
				currBlock = currBlock->next;
			} 	//currBlock->next points to tail

			currBlock->next = NULL;
			free(gameBoard->snek->tail);
			gameBoard->snek->tail = currBlock;
		}

	} 
	else if ((gameBoard->snek->length == 1) && gameBoard->cell_value[head_y][head_x] == 0)
	{ // change both head and tail coords, head is tail
		gameBoard->occupancy[tail_y][tail_x] = 0;
		gameBoard->snek->head->coord[x] = head_x;
		gameBoard->snek->head->coord[y] = head_y;
		gameBoard->snek->tail->coord[x] = head_x;
		gameBoard->snek->tail->coord[y] = head_y;
		
	}
	usleep(WAIT_FAKE);
	return 1;
} 

GameBoard* copy_board(GameBoard* board)
{
	printf("copy_board ");
	//Copy Board
	GameBoard* fake_board = (GameBoard*)(malloc(sizeof(GameBoard)));
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			fake_board->cell_value[i][j] = board->cell_value[i][j];
			fake_board->occupancy[i][j] = board->occupancy[i][j];
		}
	}
	
	//Copy Snake
	//fake_board->occupancy[board->snek->head->coord[x]][board->snek->head->coord[y]] = 1;
	fake_board->snek = init_snek(board->snek->head->coord[x],board->snek->head->coord[y]);
	if(board->snek->length > 1)
		free(fake_board->snek->tail);

	fake_board->snek->length = board->snek->length;
	
	SnekBlock* real_current = board->snek->head;
	SnekBlock* fake_current = fake_board->snek->head;

	SnekBlock* next_fake_block;
	
	for(int i = 1; i < board->snek->length; ++i)
	{	
		real_current = real_current->next;

		SnekBlock* next_fake_block = (SnekBlock *)malloc(sizeof(SnekBlock));
		fake_current->next = next_fake_block;
		fake_current = fake_current->next;
		fake_current->next = NULL;

		fake_current->coord[x] = real_current->coord[x];
		fake_current->coord[y] = real_current->coord[y];
	}
	fake_board->snek->tail = fake_current;

	return fake_board;
}

int hits_self_2(int axis, int direction, GameBoard *gameBoard) //Looks two in advance.
{
	printf("hits_self_2 ");
	int new_x, new_y;
	if (axis == AXIS_X){
		new_x = gameBoard->snek->head->coord[x] + direction + 1;
		new_y = gameBoard->snek->head->coord[y];
	} else if (axis == AXIS_Y){
		new_x = gameBoard->snek->head->coord[x];
		new_y = gameBoard->snek->head->coord[y] + direction + 1;
	}
	return gameBoard->occupancy[new_y][new_x]; //1 if occupied
}

//Queue Operations
queue* create_queue(void)
{	
	printf("create_queue ");
	queue* temp = malloc(sizeof(queue));
	if(temp == NULL)
		return NULL;
	else
	{
		temp->size = 0;
		temp->top     = NULL;
		temp->bottom  = NULL;
		return temp;
	}
}

void delete_queue(queue* q)
{
	printf("delete_queue ");
	
	if(q == NULL)
	{
		free(q);
		return;
	}
	if(q->size == 0)
	{
		//free(q);
	}
	if(q->size > 0)
	{
		queue_entry* what = q->top;
		queue_entry* huh;
		while(what->before != NULL)
		{
			huh = what->before;
			free(what);
			what = huh;
		}
		free(what);
	}
}

int dequeue(queue* q)
{
	printf("dequeue\n");
	if(q == NULL)
		return 0;

	float return_value = 0;
	
	if(q->size == 0)
		return 0;
	
	else if(q->size == 1)
    {
        return_value = q->top->value;
        free(q->top);
        q->bottom = NULL;
        q->size = 0;
    }

	else
	{
		return_value = q->top->value;
        queue_entry* temp = q->top->before;
		temp->next = NULL;
        free(q->top);
        q->top = temp;
        q->top->next = NULL;
        q->size -= 1;
	}
	return return_value;
}

//Note: Add into account that mallocing may return NULL, and add those cases into the code.
int enqueue(queue* q, double e)
{
	printf("enqueue ");
	queue_entry* new_entry = malloc(sizeof(queue_entry));
	
	new_entry->value = e;
	new_entry->next = NULL;
    new_entry->before = NULL;

	if(q->bottom == NULL)
	{
		q->bottom = new_entry;
		q->top = new_entry;
		q->size += 1;
	}
	else
	{
		new_entry->next = q->bottom;
		q->bottom->before = new_entry;
		q->bottom = new_entry;
		q->size += 1;
	}
	return 0;
}

int undo(queue* q, double e) //Returns the removed element back to the top
{
	printf("undo ");
	queue_entry* new_entry = malloc(sizeof(queue_entry));
	
	new_entry->value = e;
	new_entry->next = NULL;
    new_entry->before = NULL;

	if(q->bottom == NULL)
	{
		q->bottom = new_entry;
		q->top = new_entry;
		q->size += 1;
	}
	else
	{
		new_entry->next = q->bottom;
		q->bottom->before = new_entry;
		q->bottom = new_entry;
		q->size += 1;
	}
	return 0;
}

void display_queue(queue* q)
{
	printf("display_queue ");
	queue_entry* entry;// = malloc(sizeof(queue_entry));
	entry = (q->bottom);
	printf("Size: %i |", q->size);
	while(entry)
	{
		printf("%i ", entry->value);
		entry = entry->next;
	}
	printf("\n");
	return;
}

//Stack Operations, done on a queue, pushing popping from bottom
int push(queue* q, double e)
{
	printf("push\n");
	queue_entry* new_entry = malloc(sizeof(queue_entry));
	
	new_entry->value = e;
	new_entry->next = NULL;
    new_entry->before = NULL;

	if(q->bottom == NULL)
	{
		q->bottom = new_entry;
		q->top = new_entry;
		q->size += 1;
	}
	else
	{
		new_entry->next = q->bottom;
		q->bottom->before = new_entry;
		q->bottom = new_entry;
		q->size += 1;
	}
	return 0;
}

int pop(queue* q) //From bottomg
{
	printf("pop\n");
	if(q == NULL)
		return 0;

	float return_value = 0;
	
	if(q->size == 0)
		return 0;

	else if(q->size == 1)
    {
        return_value = q->bottom->value;
		free(q->top);
        q->top = NULL;
        q->bottom = NULL;
        q->size = 0;
    }

	else
	{
		printf("Coming here\n");
		return_value = q->bottom->value;
		queue_entry* temp = q->bottom;
        q->bottom = q->bottom->next;
        free(temp);
        q->bottom->before = NULL;
        q->size -= 1;
	}
	return return_value;
}


//Memory clean up
void clean_board(GameBoard **board)
{
	printf("\nclean_board\n");
	printf("Here??");
	SnekBlock **snekHead = &((*board)->snek->head);
	SnekBlock *curr = (*board)->snek->head;
	SnekBlock *temp; 
	
	//SnekBlock *prev;
	while (curr->next != NULL) 
	{
		printf("At block %i,%i",curr->coord[x], curr->coord[y]);
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(curr);
	//scanf(" %c", &throw_away);
	//free(*snekHead);
	free((*board)->snek);
	free(*board);
}

