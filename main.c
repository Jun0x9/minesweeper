#include <raylib.h> 
#include <stdbool.h>
#include <stdint.h> 

#define NO_FLAGS 0  
#define EMPTY    1 
#define BOMB		 1 << 1 
#define FLAGGED  1 << 2 
#define HIDDEN   1 << 3

//Game Difficulties 
const float EASY   = 10.0f/100; // number of mines placed : 10% of the total number of cells 
const float HARD   = 15.0f/100;

#define ROWS 16 
#define COLS ROWS  
#define SIZE 32 
#define MARGIN SIZE 
#define FONT_SIZE 18 
#define FONT_SIZE_LARGE 32 
const int WIDTH = ROWS * SIZE + (MARGIN * 2); //left and right 
const int HEIGHT = WIDTH ; 
const int CELLS =  ROWS * COLS  ; 

const char* NUMS_TXT[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };  
char* GAMEOVER_TXT = "Game Over"; 
char* PROMPT_1_TXT = "Press Enter to continue";  
char* PROMPT_MODE_1 = "EASY   : 1" ; 
char* PROMPT_MODE_2 = "MEDIUM : 2"; 

typedef struct{
	uint8_t neighbours; 
	uint8_t state; 
} Cell ;

typedef enum { 
	MENU, 
	MAIN_LOOP,
	GAME_OVER,
}GameState; 

typedef struct{ 
	Cell board[ COLS ][ ROWS ]; 
	GameState state ; 
	float mode ; 
	int moves ; //moves left
}Game ; 

void initGame( Game* game , float mode); 
void initBoard (Game* game)  ; 
void renderGame( Cell board[ COLS ][ ROWS ] ) ; 
void renderGameOver( Cell board[ COLS ][ ROWS ] ) ; 
void renderMenu( ) ; 

void renderBoard ( Cell board[ COLS ][ ROWS ]) ; 
void revealBoard ( Cell board[ COLS ][ ROWS ]) ;
bool checkBoard( Cell board[ ROWS ][ COLS ]) ; 
void initGame( Game* game, float mode) ; 
void floodFill( Cell board[ COLS ][ ROWS ], int row, int col ) ; 
void renderTextBox( char* text, int pos_x, int pos_y, bool lightmode);

int main( void ) { 

	InitWindow( WIDTH, HEIGHT, "MINESWEEPER") ; 
	SetTargetFPS( 30 ); 

	Game game ; //new_game 	
	Vector2 mouse_pos ; 
	int     col, row  ; //hovered cell	
	
	game.state = MENU; 

	while( !WindowShouldClose() ) {  
		switch( game.state ){ 
			case MENU: 
				if( IsKeyPressed( KEY_ONE ) ) 
					initGame( &game, EASY ) ; 
				if( IsKeyPressed( KEY_TWO )) 
					initGame( &game, HARD ); 

				renderMenu( )  ;
				break; 

			case MAIN_LOOP: 
						mouse_pos = GetMousePosition() ; 
						col = (mouse_pos.y - MARGIN) / SIZE;
						row = (mouse_pos.x - MARGIN) / SIZE;
						Cell* cell = &game.board[ col ][ row ]; 

						if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) && !(cell->state & FLAGGED)){ 
							floodFill( game.board, row, col); 	
							if( cell->state & BOMB ) {	
								revealBoard(game.board); 
								game.state = GAME_OVER ; 
							}
						}
						if( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT)){ 
							if( cell->state & HIDDEN ){
								cell->state ^= FLAGGED ; 						
								cell->state ^= HIDDEN; 
							}
						}
						
						if( checkBoard( game.board ) ) game.state = MENU ; 

						renderGame( game.board ) ;
						break ;

			case GAME_OVER: 
						if( IsKeyPressed( KEY_ENTER )) { 
							initGame(&game, EASY); 
							game.state = MAIN_LOOP; 
						}
						renderGameOver( game.board );
						break;	
		}
	}
	return 0; 
}


void renderTextBox( char* text, int pos_x, int pos_y, bool lightmode){ 
	Color bg = lightmode ? WHITE : BLACK; 
	Color fg = lightmode ? BLACK : WHITE; 
	int length = MeasureText( text, FONT_SIZE_LARGE); 
	DrawRectangle( pos_x - (length / 2) - MARGIN, pos_y , length + (MARGIN * 2) , FONT_SIZE_LARGE * 2, bg);
	DrawText( text, pos_x - (length / 2), pos_y + (MARGIN / 2), FONT_SIZE_LARGE, fg); 	
}

void renderBoard ( Cell board[ COLS ][ ROWS ])  { 	
	for (int y = 0; y < COLS; y++) {
		for (int x= 0; x < ROWS; x++) {
			Cell cell = board[ y ][ x ]; 
			uint8_t state = cell.state ; 
			int pos_x = x * SIZE + MARGIN; 
			int pos_y = y * SIZE + MARGIN;

			if( state & FLAGGED) DrawCircle( pos_x + SIZE / 2, pos_y + SIZE / 2, 10, BLACK);
			if( state & HIDDEN)  continue ;

			if( state & BOMB  )   DrawCircle( pos_x + ( SIZE / 2 ), pos_y + ( SIZE / 2 ), 10, BLACK); 
			if( state & EMPTY )   DrawRectangle( pos_x , pos_y , SIZE , SIZE , GRAY );
		  if( !( state & EMPTY ) ) 
				DrawText( NUMS_TXT[ cell.neighbours ], 
									pos_x + ( SIZE / 2 ) - ( MeasureText( NUMS_TXT[ cell.neighbours ], FONT_SIZE ) / 2 ), 
									pos_y + ( SIZE / 2 ) - ( MeasureText( NUMS_TXT[ cell.neighbours ], FONT_SIZE ) ), FONT_SIZE, BLACK);
		}
	}

	//draws the grid 
	for ( int x = 0 ; x <= ROWS ; x++ ) {
		DrawLineEx( (Vector2) { MARGIN , x * SIZE + MARGIN }, 
							  (Vector2) { WIDTH - MARGIN , x * SIZE + MARGIN },
							  2.0 , BLACK ); 

		DrawLineEx( (Vector2) { x * SIZE + MARGIN , MARGIN }, 
								(Vector2) { x * SIZE + MARGIN ,  HEIGHT - MARGIN },
								2.0, BLACK ); 
	}
	return ; 	
}

void revealBoard (Cell board[ COLS ][ ROWS ]) { 
	for (int y = 0; y < COLS; y++) 
		for (int x= 0; x < ROWS; x++)	
			if( board[y][x].state & HIDDEN ) board[y][x].state ^= HIDDEN; 		
	return;
}

bool checkBoard( Cell board[ ROWS ][ COLS ]){ 
	for (int y = 0; y < COLS; y++) 
		for (int x= 0; x < ROWS; x++)	{ 
			if( board[y][x].state & HIDDEN ) return false; 
		}
	return true; 
}

void initGame( Game* game, float mode ) { 	
	game->state = MAIN_LOOP ; 
	game->mode  = mode ; 

	initBoard(game);
}


void initBoard (Game* game) { 

	for (int y = 0; y < COLS; y++) {
		for (int x= 0; x < ROWS; x++) {	
			//all the cell must be empty or hidden 
			game->board[ y ][ x ].state = EMPTY | HIDDEN;
			game->board[ y ][ x ].neighbours = 0 ; 
		}
	}

	//set up the bombs 
	int bombs = CELLS * game->mode; 
	for(int i = 0; i <= bombs; i++){ 

		int random_col = GetRandomValue( 0, COLS - 1); 
		int random_row = GetRandomValue( 0, ROWS - 1); 
		while( !(game->board[ random_col ][ random_row ].state & EMPTY)){ 
			random_col = GetRandomValue( 0, COLS - 1); 
			random_row = GetRandomValue( 0, ROWS - 1); 			
		}
		game->board[ random_col ][ random_row ].state |= BOMB ; 
		game->board[ random_col ][ random_row ].state ^= EMPTY ; 

		//updates the neighbours
		for (int i = -1; i <= 1; i++) 
			for (int j = -1; j <= 1; j++) {
				if( i == 0 && j == 0) continue; 
				int x = random_row + j;
				int y = random_col + i; 

				if( x >= ROWS || y >= COLS ) continue ; 
				if( x < 0 || y < 0 ) continue ; 

				game->board[ y ][ x ].neighbours += 1; 
				if( game->board[ y ][ x ].state & EMPTY )
					game->board[ y ][ x ].state ^= EMPTY ; 
			}
	}		
	
	for (int y = 0; y < COLS; y++) 
		for (int x= 0; x < ROWS; x++) {	
			if( game->board[ y ][ x ].state & EMPTY ){ 
				floodFill( game->board, x, y ); 
				return ;  
			}	
	}
	return ; 
}


void renderGame( Cell board[ COLS ][ ROWS ] ) { 
		BeginDrawing() ; 
			ClearBackground( WHITE ); 
			renderBoard( board ) ; 
		EndDrawing() ;
		return ; 
}

void renderMenu( ){ 
	BeginDrawing() ; 
		ClearBackground( BLACK ); 
			renderTextBox("GAMEMODE", WIDTH / 2, FONT_SIZE_LARGE, false) ; 
			renderTextBox(PROMPT_MODE_1, WIDTH / 2, HEIGHT / 4, true) ; 
			renderTextBox(PROMPT_MODE_2, WIDTH / 2, HEIGHT / 4 + FONT_SIZE_LARGE * 3, true) ; 
			renderTextBox(">press the number key<", WIDTH / 2, HEIGHT - FONT_SIZE_LARGE * 2, false) ;	
	EndDrawing(); 
}

void renderGameOver( Cell board[ COLS ][ ROWS ] ){ 
		BeginDrawing(); 
			ClearBackground( WHITE); 			
				renderBoard(board);	
				renderTextBox( GAMEOVER_TXT , WIDTH / 2, HEIGHT / 4, true);
				renderTextBox( PROMPT_1_TXT , WIDTH / 2, HEIGHT / 2, true); 
		EndDrawing(); 
}

void floodFill( Cell board[ COLS ][ ROWS ], int row, int col ){ 
	if( col < 0 || col >= COLS ) return  ;
	if( row < 0 || row >= ROWS ) return  ;

	uint8_t state = board[ col ][ row ].state ; 

	if( state & HIDDEN ) board[ col ][ row ].state ^= HIDDEN ; 
	if( state & BOMB ) return ; 
	if( !(state & HIDDEN) ) return ; 
	if( !( state & EMPTY ) ) return; 

	//check the neighbours
	for (int dy=-1; dy <= 1; dy ++) 
		for (int dx =-1; dx <= 1; dx++){
			if( dx == 0 && dy == 0) continue; 
			floodFill( board, row + dx, col + dy ); 	
		}		
}
