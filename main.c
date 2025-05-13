#include <raylib.h> 
#include <stdint.h> 

#define NO_FLAGS 0  
#define EMPTY    1 
#define BOMB		 1 << 1 
#define FLAGGED  1 << 2 
#define HIDDEN   1 << 3

const float MINES  = 10.0f/100; 
const float EASY   = 10.0f/100; // number of mines placed : 10% of the total number of cells 
const float MEDIUM = 20.0f/100; 
const float HARD   = 25.0f/100;

#define ROWS 16 
#define COLS ROWS  
#define SIZE 32 
#define MARGIN SIZE 
#define FONT_SIZE 18 
#define FONT_SIZE_LARGE 32 
const int WIDTH = ROWS * SIZE + (MARGIN * 2); //left and right 
const int HEIGHT = WIDTH ; 
const int CELLS =  ROWS * COLS  ; 

char* NUMS_TXT[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };  
char* GAMEOVER_TXT = "Game Over"; 
char* PROMPT_1_TXT = "Press Enter to continue";  

typedef struct{
	uint8_t neighbours; 
	uint8_t state; 
} Cell ;

typedef enum { 
	MAIN_LOOP,
	GAME_OVER,
	GAME_FINISHED,
}GameState; 

Cell board[ COLS ][ ROWS ]; 
void renderBoard () ; 
void revealBoard ( ) ;
void initBoard(float mode) ; 
void floodFill( int row, int col ) ;

void renderTextBox( char* text, int pos_x, int pos_y);

int main( void ) { 

	InitWindow( WIDTH, HEIGHT, "MINESWEEPER") ; 
	SetTargetFPS( 30 ); 
	
	initBoard( EASY ) ;
	Vector2 mouse_pos ; 
	int     col, row  ; //hovered cell
	
	GameState current_state = MAIN_LOOP ; 
	while( !WindowShouldClose() ) { 
		 
		switch( current_state ){ 
			case MAIN_LOOP: 
						mouse_pos = GetMousePosition() ; 
						col = (mouse_pos.y - MARGIN) / SIZE;
						row = (mouse_pos.x - MARGIN) / SIZE;
						Cell* cell = &board[ col ][ row ]; 

						if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT )) { 
							floodFill( row, col); 
							if( cell->state & BOMB ) {	
								revealBoard(); 
								current_state = GAME_OVER ; 
							}
						}

						if( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT))  
							if( cell->state & HIDDEN ) cell->state ^= FLAGGED ; 
						
						BeginDrawing() ; 
							ClearBackground( WHITE ); 
							renderBoard() ; 
						EndDrawing() ;
						break ;

			case GAME_OVER: 
						if( IsKeyPressed( KEY_ENTER )) { 
							initBoard( EASY ); 
							current_state = MAIN_LOOP; 
						}
						BeginDrawing(); 
							ClearBackground( WHITE); 			
								renderBoard();	
								renderTextBox( GAMEOVER_TXT , WIDTH / 2, HEIGHT / 4);
								renderTextBox( PROMPT_1_TXT , WIDTH / 2, HEIGHT / 2); 
						EndDrawing(); 
						break;	

			case GAME_FINISHED:	
						break;
		}
	}
	return 0; 
}
void renderTextBox( char* text, int pos_x, int pos_y){ 
	int length = MeasureText( text, FONT_SIZE_LARGE); 

	DrawRectangle( pos_x - (length / 2) - MARGIN, pos_y , length + (MARGIN * 2) , FONT_SIZE_LARGE * 2, WHITE );

	DrawText( text, pos_x - (length / 2), pos_y + (MARGIN / 2), FONT_SIZE_LARGE, BLACK); 
	
}

void renderBoard ( )  { 	
	for (int y = 0; y < COLS; y++) {
		for (int x= 0; x < ROWS; x++) {
			Cell cell = board[ y ][ x ]; 
			uint8_t state = cell.state ; 
			int pos_x = x * SIZE + MARGIN; 
			int pos_y = y * SIZE + MARGIN;

			if( state & FLAGGED) DrawCircle( pos_x + SIZE / 2, pos_y + SIZE / 2, 8, GREEN);
			if( state & HIDDEN) continue ;

			if( state & BOMB  )   DrawCircle( pos_x + ( SIZE / 2 ), pos_y + ( SIZE / 2 ), 10, RED); 
			if( state & EMPTY )   DrawRectangle( pos_x , pos_y , SIZE , SIZE , GRAY );
		  if( !( state & EMPTY )) 
				//ugly :-) 
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

void revealBoard () { 
	for (int y = 0; y < COLS; y++) 
		for (int x= 0; x < ROWS; x++)	
			if( board[y][x].state & HIDDEN)
				board[y][x].state ^= HIDDEN; 		
	return;
}

void initBoard (float mode) { 
	for (int y = 0; y < COLS; y++) {
		for (int x= 0; x < ROWS; x++) {	
			//all the cell must be empty can hidden 
			board[ y ][ x ].state = EMPTY | HIDDEN;
			board[ y ][ x ].neighbours = 0 ; 
		}
	}

	//set up the bombs 
	int bombs = CELLS * mode; 
	for(int i = 0; i <= bombs; i++){ 
		int random_col = GetRandomValue( 0, COLS - 1); 
		int random_row = GetRandomValue( 0, ROWS - 1); 
		while( !(board[ random_col ][ random_row ].state & EMPTY)){ 
			random_col = GetRandomValue( 0, COLS - 1); 
			random_row = GetRandomValue( 0, ROWS - 1); 			
		}
		board[ random_col ][ random_row ].state |= BOMB ; 
		board[ random_col ][ random_row ].state ^= EMPTY ; 

		//updates the neighbours
		for (int i = -1; i <= 1; i++) 
			for (int j = -1; j <= 1; j++) {
				if( i == 0 && j == 0) continue; 
				int x = random_row + j;
				int y = random_col + i; 

				if( x >= ROWS || y >= COLS ) continue ; 
				if( x < 0 || y < 0 ) continue ; 

				board[ y ][ x ].neighbours += 1; 
				if( board[ y ][ x ].state & EMPTY )
					board[ y ][ x ].state ^= EMPTY ; 
			}
	}		
	return ; 
}

void floodFill( int row, int col ){ 
	if( col < 0 || col >= COLS ) return  ;
	if( row < 0 || row >= ROWS ) return  ;

	if( board[col][row].state & BOMB ) return ; 
	if(! (board[col][row].state & HIDDEN)) return ; 
	if(board[ col ][ row ].state & HIDDEN) board[ col ][ row ].state ^= HIDDEN ; 

	if( board[ col ][ row ].state & EMPTY ) 
		for (int dy=-1; dy <= 1; dy ++) 
			for (int dx =-1; dx <= 1; dx++){
				if( dx == 0 && dy == 0) continue; 
				floodFill( row + dx, col + dy ); 	
			}		
}
