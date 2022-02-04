//#include <stdio.h>
//#include <stdlib.h>

#include <genesis.h>

#include "common.h"

#include "utils.h"

#include "input.h"

#include "screens.h"

#include "sprites.h"

typedef struct {
    uint8_t x, y;
    char * text;
} scrtextA;

/* ********************************************* */

scrtextA helpScreenTxt[] = {
    { 12,    4,           "* HELP *"               },
    {  8,    7,       "START  FOR CREDITS"         },
    {  5,    9,    "START P1  FOR 1 PLAYER"        },
    {  5,   11,    "START P2  FOR 2 PLAYERS"       },
    {  5,   13,    "BUTTON C  FOR CONFIGURE"       },
    {  5,   15,    "BUTTON B  FOR THIS HELP"       },
    {  7,   21,      "MEGADRIVE VERSION BY"        },
    {  4,   23,   "JUAN JOSE PONTEPRINO V1.5"      }
};

scrtextA scoreScreenTxt[] = {
    {  12,  14, "=? MYSTERY"                      },
    {  12,  16, "=30 POINTS"                      },
    {  12,  18, "=20 POINTS"                      },
    {  12,  20, "=10 POINTS"                      }
};

scrtextA insertCoinScreenTxt[] = {
    {  9, 16, "*1 PLAYER  1 COIN"                },
    {  9, 18, "*2 PLAYERS 2 COINS"               }
};

scrtextA setupScreenTxt[] = {
    {  5,   4,  "* CONFIGURE OPTIONS *"         },
    {  3,   7,  "LIVES"                         },
    {  3,   9,  "BONUS LIFE"                    },
    {  3,  11,  "DISPLAY COINAGE"               },
    { 15,  14,  "OK"                            },
    {  4,  23,  "JUAN JOSE PONTEPRINO V1.5"     }
};

/* ********************************************* */

Sprite * spr[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

/* ********************************************* */

int8_t currentMenuScreen = 0;

/* ********************************************* */

void PrintAtArr( scrtextA * st, int8_t n ) {
    while ( n-- ) {
        PrintAt( st->x, st->y, st->text );
        st++;
    }
}

/* ********************************************* */

int PrintAtDelayArr( scrtextA * st, int8_t n, uint8_t delay ) {
    int ret = 0;
    while ( n-- && !ret ) {
        ret = PrintAtDelay( st->x, st->y, st->text, delay );
        st++;
    }
    return ret;
}

/* ********************************************* */

void freeMenuSprites() {
    int i;
    for ( i = 0; i < sizeof( spr ) / sizeof( *spr ) ; i++ )
        if ( spr[ i ] ) {
            SPR_releaseSprite( spr[ i ] );
            spr[ i ] = NULL; 
        }
}

/* ********************************************* */

void ClearMenu() {
    VDP_clearTextArea(0, 2, 40, 25);
    freeMenuSprites();
}

/* ********************************************* */

void DrawCredits() {
    PrintAt(     23, 27, "CREDIT " );
    PrintNumAt(  30, 27, numCredits, 2 | NUM_LEFTZERO );
}

/* ********************************************* */

void DrawScoreHeaderAndCredits() {
    PrintAt(        0,   0, "SCORE<1>    HI-SCORE    SCORE<2>" );

    PrintNumAt(       2,   1, player[ 0 ].score, 4 | NUM_LEFTZERO | NUM_UNSIGNED );
    PrintNumAt(      13,   1, hiScore          , 4 | NUM_LEFTZERO | NUM_UNSIGNED );

    if ( playersInGame != 1 ) {
        PrintNumAt(      26,   1, player[ 1 ].score, 4 | NUM_LEFTZERO | NUM_UNSIGNED );
    } else {
        PrintAt(         26,   1, "    " );
    }

    DrawCredits();
}

/* ********************************************* */

static int8_t animateSpriteX( int y, int from_x, int to_x, int8_t deltax, Sprite * sprite ) {
    int px;
    uint8_t anim = 0;

    for ( px = from_x; ( deltax < 0 ) ? px >= to_x : px <= to_x; px += deltax ) {
        SPR_setPosition( sprite, px, y );
        SPR_setAnim( sprite, anim );
        anim ^= 1;
        if ( DelayFrames( 2 ) ) return 1;
    }

    return 0;
}

/* ********************************************* */

static int8_t animateAlienShot( int x, int from_y, int to_y, Sprite * sprite ) {
    int py;
    uint8_t anim = 0;

    for ( py = from_y; py <= to_y; py++ ) {
        anim++;
        SPR_setPosition( sprite, x, py );
        SPR_setAnim( sprite, anim % 4);
        if ( DelayFrames( 1 ) ) return 1;
    }

    return 0;
}

/* ********************************************* */

int8_t scoreTable_Screen( int8_t invertY ) {

//    ClearRows( 16, 168 );
    ClearMenu();

    currentMenuScreen = 1;

    DrawScoreHeaderAndCredits();

    int8_t ret = DelayFrames( 50 ); // 1 seconds

    if ( !ret ) ret = PrintAtDelay( 14,  5, invertY ? "PLA\"" : "PLAY"    , 5 );
    if ( !ret ) ret = PrintAtDelay(  9,  8, "SPACE  INVADERS"             , 5 );

    if ( !ret ) ret = DelayFrames( 100 ); // 2 seconds

    if ( !ret ) {
                      PrintAt(   6,  12, "*SCORE ADVANCE TABLE*" );

                      spr[0] = SPR_addSprite(&SpriteSaucer, 32 + 76, 112, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
                      spr[1] = SPR_addSprite(&AlienSprC,    32 + 80, 128, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
                      spr[2] = SPR_addSprite(&AlienSprB,    32 + 80, 144, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
                      spr[3] = SPR_addSprite(&AlienSprA,    32 + 80, 160, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

        SPR_update();
        SYS_doVBlankProcess();
    }
    
    if ( !ret ) ret = PrintAtDelayArr( scoreScreenTxt, sizeof( scoreScreenTxt ) / sizeof( scoreScreenTxt[0] ), 5 );

    if ( ret ) ClearMenu();

    return ret;

}

/* ********************************************* */

int8_t animateAlienCY() {
    int8_t ret;
    
    ret = scoreTable_Screen( 1 );
    if ( !ret ) ret = DelayFrames( 50 ); // 1 seconds
    if ( !ret ) {
        spr[4] = SPR_addSprite(&AlienSprC, 40, 254 + 32, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        ret = animateSpriteX( 40,      254, 112 + 24 + 4 + 32, -2,   spr[4] );
    }
    if ( !ret ) {
        PrintAt( ( uint8_t ) ( 14 + 3 ), 5, " " );
        SPR_releaseSprite( spr[4] );
        spr[4] = SPR_addSprite(&AlienSprCYInv, 40, 112 + 32 + 24, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    }
    if ( !ret ) ret = animateSpriteX( 40, 112 + 32 + 24,          254 + 32,  2, spr[4]    );
    if ( !ret ) {
        SPR_releaseSprite( spr[4] );
        ret = DelayFrames( 100 ); // 2 seconds
    }
    if ( !ret ) {
        spr[4] = SPR_addSprite(&AlienSprCY, 40, 254 + 32, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    }
    if ( !ret ) ret = animateSpriteX( 40,      254 + 32, 112 + 24 + 32, -2,  spr[4]     );
    if ( !ret ) ret = DelayFrames( 100 ); // 2 seconds
    if ( !ret ) PrintAt( ( uint8_t ) ( 14 + 3 ), 5, "Y" );

    if ( ret ) ClearMenu();

    return ret;
}

/* ********************************************* */

int8_t inserCoin_Screen( int8_t extraC ) {
    int8_t ret;

//    ClearRows( 16, 168 );
    ClearMenu();

    currentMenuScreen = 2;

          PrintAt(       11,  9, "INSERT  COIN" );
    if ( extraC )
          PrintChar( ( uint8_t ) ( 11 + 7 ), 9, 'C', 0 );

    ret = PrintAtDelay(  9, 13, "<1 OR 2 PLAYERS>" , 0 );

    if ( displayCoinage ) {
        if ( !ret ) ret = PrintAtDelayArr( insertCoinScreenTxt, sizeof( insertCoinScreenTxt ) / sizeof( insertCoinScreenTxt[0] ), 5 );
    }

    if ( ret ) ClearMenu();

    return ret;
}

/* ********************************************* */

int8_t inserCoin_ScreenC() {
    int8_t ret;

    ret = inserCoin_Screen( 1 );
    if ( !ret ) ret = DelayFrames( 50 ); // 1 seconds
    if ( !ret ) {
        spr[4] = SPR_addSprite(&AlienSprC, 24, 2, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    }
    if ( !ret ) ret = animateSpriteX( 24, 2 + 32, 88 + 56 - 4 + 32, 2, spr[4] );
    if ( !ret ) {
        spr[5] = SPR_addSprite(&SquiglyShot, 88 + 56 - 4 + 6, 24 + 8, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    }
    if ( !ret ) ret = animateAlienShot( 88 + 56 - 4 + 6 + 32, 24 + 8, 72, spr[5] );

    if ( !ret ) {
        SPR_releaseSprite( spr[5] );
        PrintChar( ( uint8_t ) ( 11 + 7 ), 9, ' ', 0 );
        spr[5] = SPR_addSprite(&AShotExplo, 88 + 56 + 32, 72, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        if ( DelayFrames( 5 ) ) return 1;
        SPR_releaseSprite( spr[5] ); spr[5] = NULL;
    }

    if ( ret ) ClearMenu();

    return ret;
}

/* ********************************************* */

void pushPlayerButton_Screen() {

    if ( currentMenuScreen != 100 ) ClearMenu();

    currentMenuScreen = 100;

    PrintAt( 14, 9, "PUSH" );
    PrintAt( 6, 12, ( numCredits == 1 ) ? "ONLY 1PLAYER  BUTTON" : "1 OR 2PLAYERS BUTTON" );

    syskey.keyPressed = 0;

}


/* ********************************************* */

int8_t setup_Screen() {

    int8_t cursorPos = 0, left, right, up, down, shot, flags = 0;
    uint16_t ukey = 0, key = 0;
    uint8_t x1 = 0, x2 = 0, y = 0;

//    ClearRows( 16, 168 );
    ClearMenu();

    currentMenuScreen = 3;

    PrintAtArr( setupScreenTxt, sizeof( setupScreenTxt ) / sizeof( setupScreenTxt[0] ) );

    while ( 1 ) {
        ukey = key & ( BUTTON_LEFT | BUTTON_RIGHT | BUTTON_UP | BUTTON_DOWN | BUTTON_A );

        key = JOY_readJoypad(JOY_1) | JOY_readJoypad(JOY_2);

        left    = key & BUTTON_LEFT;
        right   = key & BUTTON_RIGHT;
        up      = key & BUTTON_UP;
        down    = key & BUTTON_DOWN;
        shot    = key & BUTTON_A;

        flags = 0;

        if ( !ukey ) {
            switch ( cursorPos ) {
                case 0:
                    x1 = 27; x2 = 29; y = 7;

                    if ( left && numLives > 3 ) numLives--;
                    if ( right && numLives < 6 ) numLives++;

                    if ( numLives > 3 ) flags |= 1;
                    if ( numLives < 6 ) flags |= 2;

                    break;

                case 1:
                    x1 = 24; x2 = 29; y = 9;

                    if ( left ) bonusLife = 1000;
                    if ( right ) bonusLife = 1500;

                    if ( bonusLife == 1500 ) flags = 1; else flags = 2;

                    break;

                case 2:
                    x1 = 25; x2 = 29; y = 11;

                    if ( left ) displayCoinage = 0;
                    if ( right ) displayCoinage = 1;

                    if ( displayCoinage ) flags = 1; else flags = 2;

                    break;

                case 3:
                    x1 = 17; x2 = 14; y = 14;

                    if ( shot ) {
                        syskey.keyPressed = 0;
                        return 0;
                    }
                    flags = 3;

                    break;
            }

            if ( up || down ) {
                PrintChar( x1, y, ' ', 0 );
                PrintChar( x2, y, ' ', 0 );

                if ( up ) cursorPos--; else cursorPos++;

                if ( cursorPos < 0 ) cursorPos = 3;
                if ( cursorPos > 3 ) cursorPos = 0;

            } else {
                PrintChar( x1, y, ( ( flags & 1 ) ? '<' : ' ' ), 0 );
                PrintChar( x2, y, ( ( flags & 2 ) ? '>' : ' ' ), 0 );
            }

            PrintNumAt( 28,   7, numLives,  1 );
            PrintNumAt( 25,   9, bonusLife, 4 );
               PrintAt( 26,  11, displayCoinage ? " ON" : "OFF" );

        }

        SPR_update();
        SYS_doVBlankProcess();

    }

    return 0;
}

/* ********************************************* */

int8_t help_Screen() {

//    ClearRows( 16, 168 );
    ClearMenu();

    currentMenuScreen = 4;

    PrintAtArr( helpScreenTxt, sizeof( helpScreenTxt ) / sizeof( helpScreenTxt[0] ) );

    DelayFrames( 200 ); // 4 seconds

    syskey.keyPressed = 0;

    return 0;
}

/* ********************************************* */
