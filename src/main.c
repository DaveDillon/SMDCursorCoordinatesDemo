#include <genesis.h>
#include <resources.h>


// Cursor X/Y Tech Demo.
// By Mayhem50 (David Dillon)
// This project was created to help me better understand sprite movement and screen coordinates and perils of
// not converting fix32 numbers back to ints when needed.
// The code here is no way ideal, other than it works and I got the information and learning I needed.
// Use at your own risk, risk of being awesome.
//-----------------------------------------------------------------------------------------------------------------------
// Using  SGDK version 2.11 / https://github.com/Stephane-D/sgdk
// Idea from watching Pigsy's Retro Game Dev Tutorials / https://www.youtube.com/@PigsysRetroGameDevTutorials

static void joyEvent(u16 joy, u16 changed, u16 state);
void CursorFix32Update();

u16 ind = TILE_USER_INDEX;
Map* bgImage;

Sprite* Cursor;
Sprite* square;
Sprite* circle;
Sprite* bsquare;

bool cusorMode = 0; // 0 is Snap and 1 is Free mode.
int cursor_x = 0; 
int cursor_y = 0; 

int eSquare_x = 160; 
int eSquare_y = -64; 
int eSquareSpeed = 1;

int eCircle_x = 160; 
int eCircle_y = -64; 
int eCircleSpeed = 1;

int ebSquare_x = 160; 
int ebSquare_y = -64; 
int ebSquareSpeed = 1;

fix32 cursorSpeed = FIX32(1.5); // Cursor movement speed.
fix32 fixCursor_x = 0; 
fix32 fixCursor_y = 0; 

int boundary_right = 320 - 8;
int boundary_bottom = 224 - 8;
int boundary_top = 0;
int boundary_left = 0;

int textPosMode = 0; // 0 = Uppser screen 1 = lower screen
int textBlock_y = 3;
int textBlock_x = 2;

char cursorFix[] = "     Cursor mode: Snap";
char cursorFre[] = "     Cursor mode: Free";

char text_GridXY[] = "        Grid x/y: %6i/%6i";
char string_GridXY[100];

char text_32bit[] = "Cursor pixel x/y: %6i/%6i";
char string_32Bit[100];

char text_fix32[] = "Fix32 cursor x/y: %6i/%6i";
char string_fix32[100];


int main()
{
	//Initialize sprites, forgetting this is bad mmmmmm oookkkkkkyyy.
	SPR_init();

	//Load background/Map.
	VDP_loadTileSet(&matrix_ts, ind, DMA );
	bgImage = MAP_create(&matrix_map,BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	PAL_setPalette(PAL0,matrix_pal.data, DMA );
	MAP_scrollTo(bgImage, 0, 0);

	// load Player 1 / Matrix cursor sprite
	PAL_setPalette(PAL3, matrix_curser.palette->data, DMA);
	Cursor = SPR_addSprite(&matrix_curser, cursor_x,cursor_y, TILE_ATTR(PAL3,FALSE,FALSE,FALSE));
	SPR_setAnim(Cursor, 0);

	eSquare_x = ((random() % 30) + 2) * 8;
	eSquareSpeed = (random() % 4) + 1;
	PAL_setPalette(PAL2, e_square.palette->data, DMA);
	square = SPR_addSprite(&e_square, eSquare_x,eSquare_y, TILE_ATTR(PAL2,FALSE,FALSE,FALSE));
	SPR_setAnim(square, 0);

	eCircle_x = ((random() % 30) + 2) * 8;
	eCircleSpeed = (random() % 4) + 1;
	PAL_setPalette(PAL2, e_circle.palette->data, DMA);
	circle = SPR_addSprite(&e_circle, eCircle_x,eCircle_y, TILE_ATTR(PAL2,FALSE,FALSE,FALSE));
	SPR_setAnim(circle, 0);

	ebSquare_x = ((random() % 30) + 2) * 8;
	ebSquareSpeed = (random() % 4) + 1;
	PAL_setPalette(PAL2, e_blueSquare.palette->data, DMA);
	bsquare = SPR_addSprite(&e_blueSquare, ebSquare_x,ebSquare_y, TILE_ATTR(PAL2,FALSE,FALSE,FALSE));
	SPR_setAnim(bsquare, 0);


	// Seupt joystick event handler callback.
	JOY_setEventHandler(joyEvent);

	// Change palette 2, color 1 to solid red
	// VDP_setPaletteColor(33, RGB24_TO_VDPCOLOR(0xFF0000));
	// (Note: Index 33 refers to Palette 2 (2 × 16 = 32) plus the first color index (+1), totaling 33.)
	// I'm turning the text red....
	PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFF0000));
	VDP_setTextPalette(PAL1);


	while(1)
	{        

		if (eSquare_y > 232){
			eSquare_x = ((random() % 30) + 2) * 8; 
			eSquareSpeed = (random() % 4) + 1;
			eSquare_y = -64;
		}
		eSquare_y += eSquareSpeed;
		SPR_setPosition(square, eSquare_x, eSquare_y);

		if (eCircle_y > 232){
			eCircle_x = ((random() % 30) + 2) * 8; 
			eCircleSpeed = (random() % 4) + 1;
			eCircle_y = -64;
		}
		eCircle_y += eCircleSpeed;
		SPR_setPosition(circle, eCircle_x, eCircle_y);

		if (ebSquare_y > 232){
			ebSquare_x = ((random() % 30) + 2) * 8; 
			ebSquareSpeed = (random() % 4) + 1;
			ebSquare_y = -64;
		}
		ebSquare_y += ebSquareSpeed;
		SPR_setPosition(bsquare, ebSquare_x, ebSquare_y);

		// Updates screen with what cursor mode I'm current running in.
		if (cusorMode == 0){
			VDP_drawTextBG(BG_A,cursorFix,textBlock_x,textBlock_y);
		}else{
			VDP_drawTextBG(BG_A,cursorFre,textBlock_x,textBlock_y);
		}

		// cusorMode 0 = Snap Mode.
		// Note snap mode only uses int, but will still display the fix32 value for reference.
		if (cusorMode == 0){ 
			
			// Fancy text mover logic... ohhhh sooo fancy.
			if (cursor_y < 80 && textPosMode == 0){
				textPosMode = 1;
				textBlock_y = 17;
				VDP_clearTextArea(0,0,320,224);
			}
			else if ((cursor_y > 80 ) && textPosMode == 1){
				textPosMode = 0;
				textBlock_y = 3;
				VDP_clearTextArea(0,0,320,224);
			}

			// Update corsor position.
			SPR_setPosition(Cursor, cursor_x, cursor_y);

			// Update debug information
			sprintf(string_GridXY, text_GridXY, cursor_x / 8, cursor_y / 8);
			VDP_drawTextBG(BG_A,string_GridXY,textBlock_x,textBlock_y + 1);
			
			sprintf(string_32Bit, text_32bit, cursor_x,  cursor_y);
			VDP_drawTextBG(BG_A,string_32Bit,textBlock_x,textBlock_y + 2);

			sprintf(string_fix32, text_fix32, FIX32(cursor_x) , FIX32(cursor_y));
			VDP_drawTextBG(BG_A,string_fix32,textBlock_x,textBlock_y + 3);
		}
		else if (cusorMode == 1){ // // cusorMode 1 = Free range mode (note to self, need chicken sprite... tasty free range chicken....)

			//  Again with the fancy text mover logic. Note the use of F32_toInt
			if (F32_toInt(fixCursor_y) < 80 && textPosMode == 0){
				textPosMode = 1;
				textBlock_y = 17;
				VDP_clearTextArea(0,0,320,224);
			}
			else if ((F32_toInt(fixCursor_y) > 80 ) && textPosMode == 1){
				textPosMode = 0;
				textBlock_y = 3;
				VDP_clearTextArea(0,0,320,224);
			}

			// Fix32/Free roam updates are done in a function.
			CursorFix32Update();
		}

		// Some stuff...
		SPR_update();
		SYS_doVBlankProcess(); 
		
	}
	return (0);
}


// This function updates the cursor in free roam mode.
// Nothing really fancy here, but note the use of where the fix32 values are turned back into ints.
void CursorFix32Update()
{
	 u16 value = JOY_readJoypad(JOY_1);

 	if (value & BUTTON_RIGHT && ( fixCursor_x < FIX32(boundary_right))){
		fixCursor_x += cursorSpeed;
	}else if (value & BUTTON_LEFT && ( fixCursor_x > FIX32(boundary_left))){
		fixCursor_x -= cursorSpeed;
	}

	if (value & BUTTON_UP && ( fixCursor_y > FIX32(boundary_top))){
		fixCursor_y -= cursorSpeed;
	}else if (value & BUTTON_DOWN && ( fixCursor_y < FIX32(boundary_bottom))){
		fixCursor_y += cursorSpeed;
	}

	SPR_setPosition(Cursor, F32_toInt(fixCursor_x), F32_toInt(fixCursor_y));

	sprintf(string_GridXY, text_GridXY, F32_toInt(fixCursor_x) / 8, F32_toInt(fixCursor_y) / 8);
	VDP_drawTextBG(BG_A,string_GridXY,textBlock_x,textBlock_y + 1);
	
	sprintf(string_32Bit, text_32bit, F32_toInt(fixCursor_x),  F32_toInt(fixCursor_y));
	VDP_drawTextBG(BG_A,string_32Bit,textBlock_x,textBlock_y + 2);

	sprintf(string_fix32, text_fix32, fixCursor_x , fixCursor_y);
	VDP_drawTextBG(BG_A,string_fix32,textBlock_x,textBlock_y + 3);

}

// This function is hooked into joystick events.
// Nothing super fancy here,
// however I do take adavange this function to move the cursor in snap mode as the example below enabled reading single button presses, 
// holding down on a button is stil the same press. so handy...
static void joyEvent(u16 joy, u16 changed, u16 state){

	if (changed & state &  BUTTON_A){
		if (cusorMode == 0) {
			cursor_x = 0;
			cursor_y = 0;
			cusorMode = 1;
		}else{
			fixCursor_x = 0;
			fixCursor_y = 0;
			cusorMode = 0;
		}
	}


	if (cusorMode == 0){
		if (joy == JOY_1){
			if (changed & state & BUTTON_RIGHT  && ( cursor_x < boundary_right)){
				cursor_x += 8;
			}else if (changed & state & BUTTON_LEFT && ( cursor_x > boundary_left)){
				cursor_x -= 8;
			}

			if (changed & state & BUTTON_UP && ( cursor_y > boundary_top )){
				cursor_y -= 8;
			}else if (changed & state & BUTTON_DOWN  && ( cursor_y < boundary_bottom)){
				cursor_y += 8;
			}
		}
	}
}
