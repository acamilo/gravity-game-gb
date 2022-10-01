#include <gb/gb.h>
#include <stdint.h>
#include <stdlib.h>
#include <gb/emu_debug.h>
#include "../res/map.h"
#include "../res/tiles.h"

void move_player_sprite(int8_t x,int8_t y);
void init_player_sprite();
void handle_player_sprite_physics();

void init_gfx() {
    // Load Background tiles and then map
    set_bkg_data(0, 85u, tiles);
    set_bkg_tiles(0, 0, 32u, 32u, mapPLN0);
    move_bkg(0,0);

    SPRITES_8x8;
    OBP1_REG=OBP0_REG;
    set_sprite_data(0,85u,tiles);

    init_player_sprite();

    move_player_sprite(0,16);

	// Turn the background map on to make it visible
    SHOW_BKG;  SHOW_SPRITES;
}

void init_player_sprite(){
    // PC will be a 16x16 sprite consisting of 4 smaller sprites
    set_sprite_tile(0,0x53);
    //set_sprite_tile(1,0x50);
    set_sprite_tile(2,0x54);
    //set_sprite_tile(3,0x51);
}

void move_player_sprite(int8_t x,int8_t y){
    move_sprite(0,x  ,y);
    //move_sprite(1,x+8,y);
    move_sprite(2,x  ,y+8);
    //move_sprite(3,x+8,y+8);


}

uint8_t background_scroll_x = 0;
uint8_t background_scroll_y = 0;

// Map sprite X,Y to tile in background
#define sprite_x_to_tile_x(SX)    ((SX- 8) + background_scroll_x ) >> 3
#define sprite_y_to_tile_y(SY)    ((SY- 16) + background_scroll_y ) >> 3

uint16_t player_x_pos=20<<8;
uint16_t player_y_pos=20<<8;

int16_t player_y_vel = 0;
int16_t player_x_vel = 0;

BOOLEAN jumpflag=FALSE;

#define maxvel 16

void handle_player_sprite_physics(){

    uint16_t Tx,Ty;
    Tx=player_x_pos;
    Ty=player_y_pos;

    uint8_t buttons = joypad();

    // gravity
    player_y_vel += 3;
    if (player_y_vel> 500) player_y_vel = 500;


    if (buttons && J_UP){
        if (jumpflag){
            player_y_vel += -500;
            jumpflag=FALSE;
        }
    }

    if (!(buttons && J_UP)){
        jumpflag=TRUE;
    }

    EMU_printf("pyv %d",player_y_vel);
    Ty += player_y_vel;
    Tx += player_x_vel;
    // Check for collisions with platforms
    uint8_t SX,SY,T;
    uint8_t x,y;
    x = Tx>>8;
    y = Ty>>8;
    SX = sprite_x_to_tile_x(x);
    SY = sprite_y_to_tile_y(y);
    SY +=2;
    T = get_bkg_tile_xy(SX,SY);

    if (T!=1){
        player_y_pos = Ty;
    } else {
        //player_y_vel=0;
    }
    player_x_pos = Tx;

    

    //EMU_printf("BB %d,%d  %d",SX,SY,T);


    move_player_sprite(player_x_pos>>8,player_y_pos>>8);


}

void main(void)
{
	init_gfx();

    // Loop forever
    while(1) {



		// Game main loop processing goes here
        handle_player_sprite_physics();


		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
