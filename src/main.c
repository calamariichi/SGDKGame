
#include <genesis.h>
#include <resources.h>

const u16 LENGTH_COL = 20;
#define SOLID_TILE 1

const u8 LEVEL_COL[280] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 
};



s16 current_camera_x;
s16 current_camera_y;

#define H_RES 320
#define V_RES 224

#define MAP_WIDTH 320
#define MAP_HEIGHT 224

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32

#define PLAYER_COL_LEFT 3
#define PLAYER_COL_RIGHT 28
#define PLAYER_COL_TOP 8
#define PLAYER_COL_BOTTOM 30

Map* level_map;
u16 ind = TILE_USER_INDEX;

#define ANIM_IDLE 0
#define ANIM_WALK 1

// int attack_timer = 0;
// int attack_duration = 56;

// int hscroll_offset = 0;
// int hscroll_offset_fore = 0;

Sprite* player;
fix32 player_x = FIX32(100);
fix32 player_y = FIX32(150);
fix32 player_x_vel = FIX32(1.5);
bool move_left = FALSE;
bool move_right = FALSE;
// bool move_up = FALSE;
// bool move_down = FALSE;

fix32 gravity = FIX32(0.8);
fix32 player_y_vel = FIX32(0);
fix32 jump_vel = FIX32(12);
bool jumping = FALSE;
fix32 terminal_vel = FIX32(6);

static void edge();
static void handleInput();
static void collission();
static void joyEvent(u16 joy, u16 changed, u16 state);


//When you want to do something ONCE, you should put it here.
int main() 
{

    //Initializing player sprite
    SPR_init();
    JOY_setEventHandler(joyEvent);

    PAL_setPalette(PAL0, bg_pal0.data, DMA);
    PAL_setPalette(PAL2, sp1.palette->data, DMA);

    VDP_loadTileSet(&background_tileset, ind, DMA);
    level_map = MAP_create(&background_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
    ind += background_tileset.numTile;
    MAP_scrollTo(level_map, 0, 0);

    player = SPR_addSprite(&sp1, fix32ToInt(player_x), fix32ToInt(player_y), TILE_ATTR(PAL2, FALSE, FALSE, TRUE));
    SPR_setAnim(player, ANIM_IDLE); 

    //Draw background and foreground
    // PAL_setPalette(PAL0, bg1.palette->data, DMA);
    // VDP_drawImageEx(BG_B, &bg1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    // ind += bg1.tileset->numTile;
    // PAL_setPalette(PAL1, fg1.palette->data, DMA);
    // VDP_drawImageEx(BG_A, &fg1, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    // ind += fg1.tileset->numTile;

    while(1) //Things you want to do EVERY SINGLE FRAME goes here.
    {
        handleInput();
        edge();
        collission();
        SPR_setPosition(player, fix32ToInt(player_x), fix32ToInt(player_y));
        SPR_update();
        SYS_doVBlankProcess();
        //VDP_setHorizontalScroll(BG_B, hscroll_offset);
        //hscroll_offset -= 1;
        
        // VDP_setHorizontalScroll(BG_A, hscroll_offset_fore);
        // hscroll_offset_fore -= 2;

        //For versions prior to SGDK 1.60 use VDP_waitVSync instead.
    }
    return (0);
}

//Movement code, also changes animation and flips sprite
static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_LEFT)
    {
        // player_x -= player_x_vel;
        // SPR_setAnim(player, ANIM_WALK);
        // SPR_setHFlip(player, FALSE);
        move_left = TRUE;
        move_right = FALSE;
    }
    else if (value & BUTTON_RIGHT)
    {
        // player_x += player_x_vel;
        // SPR_setAnim(player, ANIM_WALK);
        // SPR_setHFlip(player, TRUE);
        move_left = FALSE;
        move_right = TRUE;
    }
    else
    {
        move_left = FALSE;
        move_right = FALSE;
    }

    // if (value & BUTTON_UP)
    // {
    //     move_up = TRUE;
    //     move_down = FALSE;
    //     // player_y -= player_x_vel;
    //     // SPR_setAnim(player, ANIM_WALK);
    // }
    // else if (value & BUTTON_DOWN)
    // {
    //     move_up = FALSE;
    //     move_down = TRUE;
    //     // player_y += player_x_vel;
    //     // SPR_setAnim(player, ANIM_WALK);
    // }
    // else
    // {
    //     move_up = FALSE;
    //     move_down = FALSE;
    // }

    if ( !(value & BUTTON_LEFT) && !(value & BUTTON_RIGHT) && !(value & BUTTON_UP) && !(value & BUTTON_DOWN))
    {
        SPR_setAnim(player, ANIM_IDLE);
    }

    SPR_setPosition(player, fix32ToInt(player_x), fix32ToInt(player_y));
}

static void edge()
{
    if (player_x < FIX32(0)) player_x = FIX32(0);
    else if (player_x > FIX32(MAP_WIDTH - PLAYER_WIDTH)) player_x = FIX32(MAP_WIDTH - PLAYER_WIDTH);
    if (player_y < FIX32(0)) player_y = FIX32(0);
    else if (player_y > FIX32(MAP_HEIGHT - PLAYER_HEIGHT)) player_y = FIX32(MAP_HEIGHT - PLAYER_HEIGHT);
    
}

static void collission()
{
    s16 left_col_coord;
    s16 right_col_coord;
    s16 top_col_coord = fix32ToInt(player_y) + PLAYER_COL_TOP;
    s16 bottom_col_coord = fix32ToInt(player_y) + PLAYER_COL_BOTTOM;

    s16 xtilecoord_left;
    s16 xtilecoord_right;
    s16 ytilecoord_top =  top_col_coord >> 4;
    s16 ytilecoord_bottom =  bottom_col_coord >> 4;

    u16 array_tleft_colbox;
    u16 array_tright_colbox;
    u16 array_bleft_colbox;
    u16 array_bright_colbox;

    u8 tilecol_tleft;
    u8 tilecol_tright;
    u8 tilecol_bleft;
    u8 tilecol_bright;

    s16 block_coord;


    if (move_left)
    {
        player_x -= player_x_vel;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, FALSE);

        left_col_coord = fix32ToInt(player_x) + PLAYER_COL_LEFT;
        xtilecoord_left =  left_col_coord >> 4;
        array_tleft_colbox = xtilecoord_left + (ytilecoord_top * LENGTH_COL);
        array_bleft_colbox = xtilecoord_left + (ytilecoord_bottom * LENGTH_COL);        
        tilecol_tleft = LEVEL_COL[array_tleft_colbox];
        tilecol_bleft = LEVEL_COL[array_bleft_colbox];

        if (tilecol_tleft == SOLID_TILE || tilecol_bleft == SOLID_TILE)
        {
        block_coord = (xtilecoord_left << 4) + 16 - PLAYER_COL_LEFT;
        player_x = intToFix32(block_coord);
        }
    }
    else if (move_right)
    {
        player_x += player_x_vel;
        SPR_setAnim(player, ANIM_WALK);
        SPR_setHFlip(player, TRUE);

        right_col_coord = fix32ToInt(player_x) + PLAYER_COL_RIGHT;
        xtilecoord_right =  right_col_coord >> 4;
        array_tright_colbox = xtilecoord_right + (ytilecoord_top * LENGTH_COL);
        array_bright_colbox = xtilecoord_right + (ytilecoord_bottom * LENGTH_COL);
        tilecol_tright = LEVEL_COL[array_tright_colbox];
        tilecol_bright = LEVEL_COL[array_bright_colbox];

        if (tilecol_tright == SOLID_TILE || tilecol_bright == SOLID_TILE)
        {
            block_coord = (xtilecoord_right << 4) - PLAYER_COL_RIGHT;
            player_x = intToFix32(block_coord);
            player_x -= FIX32(0.1);
        }
        
    }

    // if (move_up)
    // {
    //     player_y -= player_x_vel;
    //     SPR_setAnim(player, ANIM_WALK);

    //     top_col_coord = fix32ToInt(player_y) + PLAYER_COL_TOP;
    //     ytilecoord_top =  top_col_coord >> 4;

    //     left_col_coord = fix32ToInt(player_x) + PLAYER_COL_LEFT;
    //     xtilecoord_left =  left_col_coord >> 4;
    //     array_tleft_colbox = xtilecoord_left + (ytilecoord_top * LENGTH_COL);
    //     array_bleft_colbox = xtilecoord_left + (ytilecoord_bottom * LENGTH_COL);        
    //     tilecol_tleft = LEVEL_COL[array_tleft_colbox];

    //     right_col_coord = fix32ToInt(player_x) + PLAYER_COL_RIGHT;
    //     xtilecoord_right =  right_col_coord >> 4;
    //     array_tright_colbox = xtilecoord_right + (ytilecoord_top * LENGTH_COL);
    //     array_bright_colbox = xtilecoord_right + (ytilecoord_bottom * LENGTH_COL);
    //     tilecol_tright = LEVEL_COL[array_tright_colbox];

    //     if (tilecol_tleft == SOLID_TILE || tilecol_tright == SOLID_TILE)
    //     {
    //         block_coord = (ytilecoord_top << 4) + 16 - PLAYER_COL_TOP;
    //         player_y = intToFix32(block_coord);
    //     }
        
    // }

    // else if (move_down)
    // {
        player_y_vel += gravity;
        if (player_y_vel > terminal_vel) player_y_vel = terminal_vel;
        player_y += player_y_vel;
        SPR_setAnim(player, ANIM_WALK);

        top_col_coord = fix32ToInt(player_y) + PLAYER_COL_TOP;
        ytilecoord_top =  top_col_coord >> 4;

        bottom_col_coord = fix32ToInt(player_y) + PLAYER_COL_BOTTOM;
        ytilecoord_bottom =  bottom_col_coord >> 4;

        left_col_coord = fix32ToInt(player_x) + PLAYER_COL_LEFT;
        xtilecoord_left =  left_col_coord >> 4;

        right_col_coord = fix32ToInt(player_x) + PLAYER_COL_RIGHT;
        xtilecoord_right =  right_col_coord >> 4;

        array_tleft_colbox = xtilecoord_left + (ytilecoord_top * LENGTH_COL);        
        tilecol_tleft = LEVEL_COL[array_tleft_colbox];

        array_bleft_colbox = xtilecoord_left + (ytilecoord_bottom * LENGTH_COL);
        tilecol_bleft = LEVEL_COL[array_bleft_colbox];

        array_bright_colbox = xtilecoord_right + (ytilecoord_bottom * LENGTH_COL);
        tilecol_bright = LEVEL_COL[array_bright_colbox];

        array_tright_colbox = xtilecoord_right + (ytilecoord_top * LENGTH_COL);
        tilecol_tright = LEVEL_COL[array_tright_colbox];


        if (tilecol_bleft == SOLID_TILE || tilecol_bright == SOLID_TILE)
        {
            block_coord = (ytilecoord_bottom << 4) - PLAYER_COL_BOTTOM;
            player_y = intToFix32(block_coord);
            player_y -= FIX32(0.1);
            player_y_vel = FIX32(0);
            jumping = FALSE;
        }
        else if (tilecol_tleft == SOLID_TILE || tilecol_tright == SOLID_TILE)
        {
            block_coord = (ytilecoord_top << 4) + 16 - PLAYER_COL_TOP;
            player_y = intToFix32(block_coord);
            player_y_vel = FIX32(0);
        }

    // }
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
     if ((changed & state & BUTTON_B) && !jumping)
     {
        player_y_vel -= jump_vel;
        jumping = TRUE;
     }
     
}