#include "snek_api.h"
#include <unistd.h>

int play_game()
{
	
	GameBoard* board = init_board();
	show_board(board);
	int axis = AXIS_INIT;
	int direction = DIR_INIT;
	
	int play_on = 1;
	int coord[2];

	//queue* x_moves;
	//queue* y_moves;

	queue** queues;// = malloc(2*sizeof(queue*));
	queue* x_moves;// = malloc(sizeof(queue));
	queue* y_moves;// = malloc(sizeof(queue));
	

	while(play_on)
	{
		coord[x] = board->snek->head->coord[x];
		coord[y] = board->snek->head->coord[y];
		unsigned short go_x = (axis == AXIS_Y && direction == 1 && coord[y] == (BOARD_SIZE - 1)) || (axis == AXIS_Y && direction == -1 && coord[y] == 0);
		unsigned short go_y = (axis == AXIS_X && direction == 1 && coord[x] == (BOARD_SIZE - 1)) || (axis == AXIS_X && direction == -1 && coord[x] == 0);

		if(MOOGLE_FLAG && get_food_x(board) != coord[x] && get_food_y(board) != coord[y]) //Food on Board
		{
			DEAD = 0;
			//printf("Food\n");
			printf("The foods at %i, %i",get_food_x(board), get_food_y(board));
			printf("The heads at %i, %i",board->snek->head->coord[x],board->snek->head->coord[y]);
			//free(x_moves);
			//free(y_moves);
			x_moves = create_moves(board, AXIS_X, get_food_x(board), get_food_y(board));
			y_moves = create_moves(board, AXIS_Y, get_food_x(board), get_food_y(board));
			queues  = combine_queues(x_moves, y_moves, board);   
			//delete_queue(x_moves);
			//delete_queue(y_moves);
			//printf("Crashing here?");
			while(queues[0]->size > 0 && get_food_x(board) != -1)
			{
				printf("In the while loop\n");
				show_board(board);
				display_queue(queues[0]);
				display_queue(queues[1]);

				direction = dequeue(queues[0]);
				axis = dequeue(queues[1]);
				

				printf("About to play_on\n");
				play_on = advance_frame(axis, direction, board);
				usleep(WAIT);
			}
			
			if(DEAD && is_failure_state(axis, direction, board))
		    	play_on = advance_frame(axis, direction, board);
			

			printf("Getting here\n");
			delete_queue(queues[0]);
			free(queues[0]);
			printf("Getting here\n");
			delete_queue(queues[1]);
			free(queues[1]);
			free(queues);

		}
		else //No Food on Board
		{
			printf("No food\n");
			if(go_x)
			{
				axis = AXIS_X;
				if (coord[x] < (int)(BOARD_SIZE / 2))
				{ 
					direction = safe_direction(AXIS_X, board);
				} 
				else 
				{
					direction = safe_direction(AXIS_X, board);
				}
			} 
			else if (go_y)
			{
				axis = 	AXIS_Y;
				if (coord[y] < (int)(BOARD_SIZE / 2))
				{ 
					direction = safe_direction(AXIS_Y, board);
				} 
				else 
				{
					direction = safe_direction(AXIS_Y, board);
				}
			}
			show_board(board);
			if(is_failure_state(axis, direction, board))
			{
				direction = safe_direction(AXIS_X, board);
				axis = AXIS_X;
				if(direction == 0) //Not a safe X direction
				{
					direction = safe_direction(AXIS_Y, board);
					axis = AXIS_Y;
				}
				if(direction == 0) //No safe direction
				{	
					direction = 1;
					axis = AXIS_X; //Just whatever.
				}

			}
			play_on = advance_frame(axis, direction, board);

			usleep(WAIT);
		}
		
	}
	end_game(&board);

	return SCORE;
}

int main()
{
	play_game();
	//printf("main");
	int scores[100];
	/*for(int i = 0; i < 1; ++i)
	{
		scores[i] = play_game();
		SCORE = 0;
		usleep(275550);
	}
	for(int i = 0; i < 1; ++i)
	{
		printf("%i\n",scores[i]);
	}*/

	
	/*queue* try = create_queue();
	enqueue(try, 5);
	display_queue(try);
	enqueue(try, 4);
	display_queue(try);
	enqueue(try, 3);
	display_queue(try);
	printf("%f\n",try->top->value); //dequeue(try));
	printf("%f\n",dequeue(try));
	printf("%f\n",dequeue(try));*/

	return 0;
}

/*
if (go_x) {
			// going to hit the bottom or top
			// go right or left
			axis = AXIS_X;
			if (coord[x] < (int)(BOARD_SIZE / 2)){ 
				direction = RIGHT;
			} else {
				direction = LEFT;
			}
		} else if (go_y) {
			axis = 	AXIS_Y;
			if (coord[y] < (int)(BOARD_SIZE / 2)){ 
				direction = DOWN;
			} else {
				direction = UP;
			}
}
*/