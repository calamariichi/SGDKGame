
#include <genesis.h>
#include <resources.h>
#include <string.h>

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

//GAME STATES
enum GAME_STATE
{
    STATE_MENU,
    STATE_PLAY,
    STATE_GAMEOVER
};
enum GAME_STATE currentState;

int score = 0;
char label_score[6] = "SCORE\0";
char str_score[4] = "0";
int sign(int x)
{
    return (x > 0) - (x < 0);
}


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

Sprite* player;
fix32 player_x = FIX32(144);
fix32 player_y = FIX32(170);
fix32 player_x_vel = FIX32(1.5);
bool move_left = FALSE;
bool move_right = FALSE;

fix32 gravity = FIX32(0.8);
fix32 player_y_vel = FIX32(0);
fix32 jump_vel = FIX32(12);
bool jumping = FALSE;
fix32 terminal_vel = FIX32(6);

//

typedef struct
{
    u8 active;
    s16 x;
    s16 y;
    s8 speedX;
    s8 speedY;
    Sprite* sprite;
    u8 anim;
} Pizza;

Pizza pizzas[30];

Pizza* pizza;

u8 ticks;

u8 TICKS2;
u8 TICKS3;
u8 TICKS4;
u8 TICKS6;
u8 TICKS10;

u8 timeTicks;

//

static void spawnPizza();
static void edge();
static void handleInput();
static void collission();
static void joyEvent(u16 joy, u16 changed, u16 state);


void updateScore();

//VARIABLES FOR STATES
void basicInit();
void processStateMenu();
void processStatePlay();
void processStateGameOver();
void joyHandlerMenu(u16 joy, u16 changed, u16 state);


//When you want to do something ONCE, you should put it here.
int main() 
{
    //PROCESSING MENU AND PLAY STATES
    basicInit();

    while(1) //Things you want to do EVERY SINGLE FRAME goes here.
    {
        switch(currentState)
        {
            case STATE_MENU:
            {
                processStateMenu();
                break;
            }
            case STATE_PLAY:
            {
                
                processStatePlay();
                break;
            }
        }
        SYS_doVBlankProcess();
    }
    return (0);
}

//Movement code, also changes animation and flips sprite
static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_LEFT)
    {
        move_left = TRUE;
        move_right = FALSE;
    }
    else if (value & BUTTON_RIGHT)
    {
        move_left = FALSE;
        move_right = TRUE;
    }
    else
    {
        move_left = FALSE;
        move_right = FALSE;
    }

    if ( !(value & BUTTON_LEFT) && !(value & BUTTON_RIGHT) && !(value & BUTTON_UP) && !(value & BUTTON_DOWN))
    {
        SPR_setAnim(player, ANIM_IDLE);
    }

    SPR_setPosition(player, fix32ToInt(player_x), fix32ToInt(player_y));
}

void basicInit()
{
    JOY_init();
    currentState = STATE_MENU;
}

void processStateMenu()
{
    JOY_setEventHandler(&joyHandlerMenu);

    VDP_drawText("PRESS START TO PLAY", 10, 13);

    while (currentState == STATE_MENU)
    {
        SYS_doVBlankProcess();
    }

    VDP_clearText(10, 13, 20);
}

void processStatePlay()
{
    JOY_setEventHandler(NULL);
    //VDP_drawText("PLAY STATE", 10, 13);

    VDP_setTextPlane(BG_A);
    VDP_drawText(label_score,1 ,1);
    updateScore();

    //Initializing player sprite
    SPR_init();
    JOY_setEventHandler(joyEvent);

    PAL_setPalette(PAL0, bg_pal0.data, DMA);
    PAL_setPalette(PAL1, sp_pizza.palette->data, DMA);
    PAL_setPalette(PAL2, sp1.palette->data, DMA);

    VDP_loadTileSet(&background_tileset, ind, DMA);
    level_map = MAP_create(&background_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
    ind += background_tileset.numTile;
    MAP_scrollTo(level_map, 0, 0);

    player = SPR_addSprite(&sp1, fix32ToInt(player_x), fix32ToInt(player_y), TILE_ATTR(PAL2, FALSE, FALSE, TRUE));
    SPR_setAnim(player, ANIM_IDLE);

//
    for(u8 i = 0 ; i < 30 ; i++)
    {
        pizza=&pizzas[i];
		pizza->active=0;
		pizza->speedX=0;
		pizza->speedY=0;
		pizza->x=0;
		pizza->y=-32;

        if(pizza->sprite != NULL ){
		SPR_setPosition(pizza->sprite, 0, -32);
		SPR_setVisibility(pizza->sprite, HIDDEN);
		}
    } 

    spawnPizza();
    pizzas[0].x=0;
    pizzas[0].y=20;

    ticks = 0;


//
    while (currentState == STATE_PLAY)
    {
		TICKS2 = ticks % 2;
		TICKS3 = (ticks % 3 == 0);
		TICKS4 = (ticks % 4 == 0);
		TICKS6 = (ticks % 6 == 0);
		TICKS10 = (ticks % 10 == 0);

        for(u8 i = 0 ; i < 30 ; i++)
        {
            pizza=&pizzas[i];

            if(pizza->active)
            {
                pizza->x+=pizza->speedX;
                pizza->y+=pizza->speedY;

                if (TICKS10)
                {
                    pizza->speedY=pizza->speedY+1;
                }
            }
        }

        	if( (score < 5 && ticks % 240 == 30)
			|| (score >= 5 && score < 10 && ticks % 220 == 30)
			|| (score >= 10 && score < 15 && ticks % 200 == 30)
			|| (score >= 15 && score < 20 && ticks % 180 == 30)
			|| (score >= 20 && ticks % 160 == 30)
            ){
                spawnPizza();
            }

            ++ticks;
            if(ticks == 240)
            {
                ticks = 0;
            }
//
        handleInput();
        edge();
        updateScore();
        collission();
        SPR_setPosition(player, fix32ToInt(player_x), fix32ToInt(player_y));
        SPR_update();
        SYS_doVBlankProcess();
    }

    VDP_clearText(10, 13, 10);

}

void joyHandlerMenu(u16 joy, u16 changed, u16 state)
{
    if (joy == JOY_1)
    {
        if (state & BUTTON_START)
        {
            currentState = STATE_PLAY;
        }
    }
}

void updateScore()
{
    sprintf(str_score, "%d", score);
    VDP_clearText(1, 2, 3);
    VDP_drawText(str_score, 1, 2);
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
        score++;
        updateScore();
     }
     
}

static void spawnPizza()
{
    u8 index;
    for (index = 0; index < 30; index++)
    {
        if(pizzas[index].active == 0)
        {
            break;
        }

    }

    if (index < 30)
    {
        pizzas[index].active=1;
        pizzas[index].x=0;
        pizzas[index].y=20+(random() % 100);
        pizzas[index].speedX=1;
        pizzas[index].speedY=-3;

        if (pizzas[index].sprite == NULL)
        {
            pizzas[index].sprite = SPR_addSprite(&sp_pizza, pizzas[index].x, pizzas[index].y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
        }

        else
        {
            SPR_setVisibility(pizzas[index].sprite, VISIBLE);
        }
    }
    
}