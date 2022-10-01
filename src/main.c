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

int8_t player_x_vel = 0;
int8_t player_y_vel = 0;

uint8_t player_sprite_x = 0;
uint8_t player_sprite_y = 0;

#define maxvel 32

void handle_player_sprite_physics(){
    if (abs(player_y_vel)>maxvel){
        if(player_y_vel>0) { player_y_vel = maxvel; }
        else { player_y_vel = -maxvel; }
    }
        if (abs(player_x_vel)>maxvel){
        if(player_x_vel>0) { player_x_vel = maxvel; }
        else { player_x_vel = -maxvel; }
    }
    player_x_pos += player_x_vel;
    player_y_pos += player_y_vel;


    player_sprite_x = player_x_pos>>8;
    player_sprite_y = player_y_pos>>8;
    move_player_sprite(player_sprite_x,player_sprite_y);

}

void main(void)
{
	init_gfx();

    // Loop forever
    while(1) {
        switch(joypad()){
            case J_RIGHT:
                player_x_vel = maxvel;
                break;
            case J_LEFT:
                player_x_vel = -maxvel;
                break;
            case J_UP:
                player_y_vel = -maxvel;
                break;
            case J_DOWN:
                player_y_vel = maxvel;
                break;
            default:
                player_x_vel=0;
                player_y_vel=0;
        }

		// Game main loop processing goes here
        handle_player_sprite_physics();
        uint8_t SX,SY,T;
        SX = sprite_x_to_tile_x(player_sprite_x);
        SY = sprite_y_to_tile_y(player_sprite_y);
        T = get_bkg_tile_xy(SX,SY);

        EMU_printf("BB %d,%d  %d",SX,SY,T);

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
