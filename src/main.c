#include <gb/gb.h>
#include <stdint.h>
#include "../res/map.h"
#include "../res/tiles.h"

void move_player_sprite(int8_t x,int8_t y);
void init_player_sprite();
void handle_player_sprite_gravity();

void init_gfx() {
    // Load Background tiles and then map
    set_bkg_data(0, 79u, tiles);
    set_bkg_tiles(0, 0, 32u, 32u, mapPLN0);

    SPRITES_8x8;
    OBP1_REG=OBP0_REG;
    set_sprite_data(0,4,tiles);

    init_player_sprite();

    move_player_sprite(8,16);

	// Turn the background map on to make it visible
    SHOW_BKG;  SHOW_SPRITES;
}

void init_player_sprite(){
    // PC will be a 16x16 sprite consisting of 4 smaller sprites
    set_sprite_tile(0,2);
    set_sprite_tile(1,2);
    set_sprite_tile(2,2);
    set_sprite_tile(3,2);
}

void move_player_sprite(int8_t x,int8_t y){
    move_sprite(0,x  ,y);
    move_sprite(1,x+8,y);
    move_sprite(2,x  ,y+8);
    move_sprite(3,x+8,y+8);


}

uint8_t player_x_pos=20;
uint8_t player_y_pos=20;
uint8_t player_x_vel = 0;
uint8_t player_y_vel = 0;
const uint8_t player_max_y_vel = (3<<4);
const uint8_t player_max_x_vel = (3<<4);
void handle_player_sprite_gravity(){
    player_y_vel += 2;
    if (player_y_vel>player_max_y_vel) player_y_vel=player_max_y_vel;
    if (player_x_vel>player_max_y_vel) player_x_vel=player_max_x_vel;

    player_y_pos += player_y_vel>>4;
    player_x_pos += player_x_vel>>4;
    move_player_sprite(player_x_pos,player_y_pos);

}

void main(void)
{
	init_gfx();

    // Loop forever
    while(1) {


		// Game main loop processing goes here
        handle_player_sprite_gravity();

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
