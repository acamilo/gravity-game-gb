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
    set_bkg_data(0, 126u, tiles);
    set_bkg_tiles(0, 0, 32u, 32u, mapPLN0);
    move_bkg(0,0);

    SPRITES_8x8;
    OBP1_REG=OBP0_REG;
    set_sprite_data(0,32u,tiles);

    init_player_sprite();

    move_player_sprite(0,16);

	// Turn the background map on to make it visible
    SHOW_BKG;  SHOW_SPRITES;
}

void init_player_sprite(){
    // PC will be a 16x16 sprite consisting of 4 smaller sprites
    set_sprite_tile(0,0x00);
    set_sprite_tile(1,0x02);
    set_sprite_tile(2,0x01);
    set_sprite_tile(3,0x03);
}

void move_player_sprite(int8_t x,int8_t y){
    move_sprite(0,x  ,y);
    move_sprite(1,x+8,y);
    move_sprite(2,x  ,y+8);
    move_sprite(3,x+8,y+8);


}

uint8_t background_scroll_x = 0;
uint8_t background_scroll_y = 0;

// Map sprite X,Y to tile in background
#define sprite_x_to_tile_x(SX)    ((SX- 8) + background_scroll_x ) >> 3
#define sprite_y_to_tile_y(SY)    ((SY- 16) + background_scroll_y ) >> 3

uint8_t player_x_pos=30;
uint8_t player_y_pos=30;


BOOLEAN jumpflag=FALSE;



#define jump_height 6
#define gravity 1   
#define max_fall_Speed 10 

int8_t falling_speed=0;
BOOLEAN falling = TRUE;
uint8_t jump_counter=2;
BOOLEAN jumping=FALSE;
BOOLEAN debounce=TRUE;
BOOLEAN gravity_direction=TRUE;
void handle_player_sprite_physics(){
    uint8_t SX,SY,Ta,Tb;
    uint8_t buttons = joypad();
    int8_t Dx=0;
    int8_t Dy=0;

    if ( (buttons & J_A) ) {
        gravity_direction=TRUE;
        falling=TRUE;
        }
    if ( (buttons & J_B) ) {
        gravity_direction=FALSE;
        falling=TRUE;
        }   

    if ( (buttons & J_RIGHT) ) Dx += 2;
    if ( (buttons & J_LEFT)  ) Dx -= 2;   

    if ( (buttons & J_UP ) && debounce && jump_counter !=0) {
        jump_counter--;
        jumping=TRUE;
        falling=TRUE;
        debounce=FALSE;
        falling_speed=jump_height;
    }



    if ( !(buttons & J_UP) ) debounce=TRUE;



if (falling){
    if (falling_speed<-max_fall_Speed) falling_speed=-max_fall_Speed;
    if (falling_speed>=max_fall_Speed) falling_speed=max_fall_Speed;

    if (gravity_direction){
        falling_speed--;
        Dy -= falling_speed;
    } else {
        falling_speed--;
        Dy += falling_speed;

    }
} else {
    // Should we be falling?
        SX = sprite_x_to_tile_x(player_x_pos);
        SY = sprite_y_to_tile_y(player_y_pos);
        if (gravity_direction){
            Ta = get_bkg_tile_xy(SX,SY+2);
            Tb = get_bkg_tile_xy(SX+1,SY+2);
        } else {
            Ta = get_bkg_tile_xy(SX,SY);
            Tb = get_bkg_tile_xy(SX+1,SY);
        }
       
        if (Ta==0x1A | Tb==0x1A) {
            
        } else {
            falling=TRUE;
        }
}









    // Clamp speed so we don't fall through a block



    // Are we going to fall into a block?
    if (falling){

        if (Dy>7) Dy = 7;
        if (Dx>7) Dx = 7;
        SX = sprite_x_to_tile_x(player_x_pos+Dx);
        SY = sprite_y_to_tile_y(player_y_pos+Dy);
        Ta = get_bkg_tile_xy(SX,SY+2);
        Tb = get_bkg_tile_xy(SX+1,SY+2);
        if (Ta==0x1A | Tb==0x1A) {
            // Bring us right up to the block below.
            Dy=8-(player_y_pos) & 0x07;
            falling_speed=0;
            falling=FALSE;
            
        }
    }

    // Are we goint to jump into a block
    if (falling){
    uint8_t SX,SY,Ta,Tb;
        if (Dy<-7) Dy = -7;
        if (Dx<-7) Dx = -7;
        SX = sprite_x_to_tile_x(player_x_pos+Dx);
        SY = sprite_y_to_tile_y(player_y_pos+Dy);
        Ta = get_bkg_tile_xy(SX,SY);
        Tb = get_bkg_tile_xy(SX+1,SY);
        if (Ta==0x1A | Tb==0x1A) {
            // Bring us right up to the block below.
            Dy=(player_y_pos) & 0x07;
            falling_speed=0;
            falling=FALSE;
            
        }
    }

    if (!falling) {
        jumping = FALSE;
        jump_counter=2;
        
    }
    
    //EMU_printf("Dx: %d\tDy: %d\tj: %d\td: %d\trem: %d",Dx,Dy,jumping,debounce,(player_y_pos) & 0x07);
    EMU_printf("Dx: %d\tDy: %d\tPx: %d\tPy: %d\tfs: %d\tGD: %d\tFAL: %d",Dx,Dy,player_x_pos,player_y_pos,falling_speed,gravity_direction,falling);

    //EMU_printf("but: %x",buttons);
    player_x_pos += Dx;
    player_y_pos += Dy;
    move_player_sprite(player_x_pos,player_y_pos);
}

void main(void)
{
    EMU_printf("TEST");
	init_gfx();
    uint8_t frameCounter=0;
    // Loop forever
    while(1) {
        frameCounter++;


		// Game main loop processing goes here
        if((frameCounter & 0x01)==0)handle_player_sprite_physics();


		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
