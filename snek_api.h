/**
AUTHOR: SAIMA ALI
LATEST WORKING VERSION 
FEBRUARY 2ND, 2020
ESC190H1S PROJECT
SNAKE API
 **/

#define WAIT 175550
#define WAIT_FAKE 0//275550

#include <stdlib.h>
#include <stdio.h>

#define CYCLE_ALLOWANCE 1.5
#define BOARD_SIZE 10

#define LIFE_SCORE 1 //score awarded for simply staying alive one frame

#define AXIS_X -1
#define AXIS_Y 1

#define UP -1
#define DOWN 1
#define LEFT -1
#define RIGHT 1

#define AXIS_INIT AXIS_Y
#define DIR_INIT DOWN

#define x 0
#define y 1

#define MOOGLE_POINT 20
#define HARRY_MULTIPLIER 3

int CURR_FRAME;
int SCORE;
int MOOGLE_FLAG;

int DEAD;
char throw_away;

//Create a Queue
typedef struct queue_entry
{
	int value;
	struct queue_entry* next;
	struct queue_entry* before;

} queue_entry;

typedef struct queue
{
	int size;
	struct queue_entry* top; 
	struct queue_entry* bottom; 
} queue;


typedef struct SnekBlock
{
	int coord[2];
	struct SnekBlock* next;
} SnekBlock;

typedef struct Snek
{
	struct SnekBlock* head;
	struct SnekBlock* tail;
	int length;
} Snek;

typedef struct GameBoard
{
	int cell_value[BOARD_SIZE][BOARD_SIZE];
	int occupancy[BOARD_SIZE][BOARD_SIZE];
	struct Snek* snek;
} GameBoard;


GameBoard *init_board();
Snek *init_snek(int a, int b);
int hits_edge(int axis, int direction,  GameBoard *gameBoard);
int hits_self(int axis, int direction,  GameBoard *gameBoard);
int is_failure_state(int axis, int direction,  GameBoard *gameBoard);
int advance_frame(int axis, int direction,  GameBoard *gameBoard);
void end_game(GameBoard **board);
void show_board(GameBoard* gameBoard);
int get_score();
int get_food_x(GameBoard* gameBoard); //Get the x coordinate of the food
int get_food_y(GameBoard* gameBoard);
queue* create_queue(void);
void delete_queue(queue* q);
int dequeue(queue* q);
int enqueue(queue* q, double e);
void display_queue(queue* q);
int is_food(GameBoard* gameBoard);
queue** create_moves_combined(GameBoard* gameBoard); //Called only if is_food positive.
queue* create_moves(GameBoard* gameBoard, int axis, int wanted_x, int wanted_y); //Called only if is_food positive.
int safe_direction(int axis, GameBoard* board);
queue** combine_queues(queue* x_moves, queue* y_moves, GameBoard* board);
int advance_frame_fake(int axis, int direction, GameBoard *gameBoard);
GameBoard* copy_board(GameBoard* real_board);
int hits_self_2(int axis, int direction, GameBoard *gameBoard); //Looks two in advance.

int pop(queue* q);
int push(queue* q, double e);
int next_move(queue* x_moves, queue* y_moves, queue* moves_queue, queue* axis_queue, GameBoard* board, int last);
int undo(queue* q, double e); //Returns the removed element back to the top

void clean_board(GameBoard **board);


