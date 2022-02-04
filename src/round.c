#include <genesis.h>

#include "common.h"
#include "utils.h"
#include "sprites.h"
#include "tables.h"
#include "screens.h"

#include "input.h"

#include "sfx.h"
#include "sound.h"

/* ********************************************* */

typedef struct {
    // off: 0
    uint8_t active;
    // off: 1
    uint8_t aniFrame;
    // off: 2-3
    int16_t explodingCnt;
    // off: 4-5
    // off: 6-7
    int16_t x, y;
} _alienShotInfo;

/* ********************************************* */
 
//#define bittest(  v, n )      ( v )[ ( n ) >> 3 ] &   ( 1u << ( ( n ) & 7 ) )
//#define bitset(   v, n )      ( v )[ ( n ) >> 3 ] |=  ( 1u << ( ( n ) & 7 ) )
//#define bitreset( v, n )      ( v )[ ( n ) >> 3 ] &= ~( 1u << ( ( n ) & 7 ) )

/* ********************************************* */
/* Global Vars                                   */
/* ********************************************* */

/* Player */

int16_t playerX;
uint8_t playerExplodingCnt = 0;
int8_t  playerFrames = 0; // Must be signed!!!

/* Player Shot */

int16_t shotX, shotY; // shotX don't need be initialized
uint8_t shotExplodingCnt = 0;
uint8_t shotFrames = 0;

/* Aliens */

uint8_t alienAnimationStatus = 0;
uint8_t alienExplodingCnt = 0;
int16_t alienExplodingX = 0, alienExplodingY = 0;
int16_t tDeltaX;
uint8_t aliensFrames = 0;

/* Alien Shot */

int8_t alienShotTimer = 0;
uint8_t alienShotFrameCnt = 0b00000001;
int8_t reloadCnt = 0;
//int8_t aShotFrame = 0;

enum {
    AS_ROL = 0,
    AS_PLU,
    AS_SQU
};

_alienShotInfo * asRol, * asPlu, * asSqu;
_alienShotInfo alienShotInfo[3];

/* Saucer */

int8_t saucerStart = 0;
int8_t saucerActive = 0;
int8_t saucerExplodingCnt = 0;
int16_t saucerX = 0;
int16_t saucerDeltaX = 0;
uint8_t saucerFrames = 0;

/* Collision */

int16_t aCol = 0, aRow = 0;

/* Demo */

int8_t demoFrame = DEMOSPEED;
uint8_t demoCurCommand = 0;

/* Others */

int8_t gameState = 0;

uint8_t alienSoundCnt = 0;

uint8_t anybodyExploding = 0;

struct {
    uint8_t killed:1;
    uint8_t shotReady:1;

    uint8_t playerIsAlive:1;    
    uint8_t waitForEventsComplete:1;
    uint8_t downFleet:2;
} state1;

/* ********************************************* */

Sprite * spr_lifes[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

Sprite * spr_aliens[55] = { NULL };
Sprite * spr_player = NULL;
Sprite * spr_ufo = NULL;

Sprite * spr_alien_explosion = NULL;
Sprite * spr_alien_shot_explosion[3] = { NULL, NULL, NULL };
Sprite * spr_player_explosion = NULL;
Sprite * spr_player_shot_explosion = NULL;
Sprite * spr_ufo_explosion = NULL;

Sprite * spr_alien_shot[3] = { NULL, NULL, NULL };

Sprite * spr_player_shot = NULL;



/* megadrive */

void loadSprites() {
    int n;
    for ( n = 0; n < 6; n++ ) {
        spr_lifes[n] = SPR_addSprite(&PlayerSprite, MARGIN_X_PX + 24 + n * 16, 216, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_lifes[n], HIDDEN);
    }

    for ( n = 0; n < 11; n++ ) {
        spr_aliens[0*11+n] = SPR_addSprite(&AlienSprA, MARGIN_X_PX + 16 * n,  0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_aliens[0*11+n], HIDDEN);
        spr_aliens[1*11+n] = SPR_addSprite(&AlienSprA, MARGIN_X_PX + 16 * n, 16, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_aliens[1*11+n], HIDDEN);
        spr_aliens[2*11+n] = SPR_addSprite(&AlienSprB, MARGIN_X_PX + 16 * n, 32, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_aliens[2*11+n], HIDDEN);
        spr_aliens[3*11+n] = SPR_addSprite(&AlienSprB, MARGIN_X_PX + 16 * n, 48, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_aliens[3*11+n], HIDDEN);
        spr_aliens[4*11+n] = SPR_addSprite(&AlienSprC, MARGIN_X_PX + 16 * n, 64, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_aliens[4*11+n], HIDDEN);
    }

    spr_player = SPR_addSprite(&PlayerSprite, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_player, HIDDEN);

    spr_ufo = SPR_addSprite(&SpriteSaucer, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_ufo, HIDDEN);

    spr_alien_explosion = SPR_addSprite(&AlienExplode, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_alien_explosion, HIDDEN);

    for ( n = 0; n < 3; n++ ) {
        spr_alien_shot_explosion[n] = SPR_addSprite(&AShotExplo, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setVisibility(spr_alien_shot_explosion[n], HIDDEN);
    }

    spr_player_explosion = SPR_addSprite(&PlrBlowupSprites, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_player_explosion, HIDDEN);

    spr_player_shot_explosion = SPR_addSprite(&ShotExploding, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_player_shot_explosion, HIDDEN);

    spr_ufo_explosion = SPR_addSprite(&SpriteSaucerExp, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_ufo_explosion, HIDDEN);

    spr_player_shot = SPR_addSprite(&PlayerShotSpr, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_player_shot, HIDDEN);

    spr_alien_shot[ AS_ROL ] = SPR_addSprite(&RollShot, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_alien_shot[ AS_ROL ], HIDDEN);

    spr_alien_shot[ AS_PLU ] = SPR_addSprite(&PlungerShot, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_alien_shot[ AS_PLU ], HIDDEN);

    spr_alien_shot[ AS_SQU ] = SPR_addSprite(&SquiglyShot, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    SPR_setVisibility(spr_alien_shot[ AS_SQU ], HIDDEN);

}


void ClearGameArea() {
    int n;

    VDP_clearTextArea(0, 2, 40, 25);

    for ( n = 0; n < 11; n++ ) {
        SPR_setVisibility(spr_aliens[0*11+n], HIDDEN);
        SPR_setVisibility(spr_aliens[1*11+n], HIDDEN);
        SPR_setVisibility(spr_aliens[2*11+n], HIDDEN);
        SPR_setVisibility(spr_aliens[3*11+n], HIDDEN);
        SPR_setVisibility(spr_aliens[4*11+n], HIDDEN);
    }

    SPR_setVisibility(spr_player, HIDDEN);
    SPR_setVisibility(spr_ufo, HIDDEN);
    SPR_setVisibility(spr_alien_explosion, HIDDEN);

    for ( n = 0; n < 3; n++ ) SPR_setVisibility(spr_alien_shot_explosion[n], HIDDEN);

    SPR_setVisibility(spr_player_explosion, HIDDEN);
    SPR_setVisibility(spr_player_shot_explosion, HIDDEN);
    SPR_setVisibility(spr_ufo_explosion, HIDDEN);
    SPR_setVisibility(spr_player_shot, HIDDEN);
    
    for ( n = 0; n < 3; n++ ) SPR_setVisibility(spr_alien_shot[n], HIDDEN);

    SPR_update();
    SYS_doVBlankProcess();
}

/* ********************************************* */

Bitmap * cloneBitmap( const Bitmap * bmp ) {
    Bitmap * dest = malloc( sizeof( Bitmap ) + bmp->w * bmp->h / 2 );
    if ( !dest ) return NULL;
    dest->image = ( ( uint8_t * ) dest ) + sizeof( Bitmap );
    return unpackBitmap( bmp, dest );
}

/* ********************************************* */

inline uint8_t getPixel(Bitmap * bmp, int16_t x, int16_t y)
{
    if ( x < 0 || y < 0 || x >= bmp->w || y >= bmp->h ) return 0;
    
    uint8_t * dst = bmp->image + ( y * ( bmp->w >> 1 ) ) + ( x >> 1 );

    if (x & 1)  return *dst & 0x0f;
    else        return *dst >> 4;
}

/* ********************************************* */

inline void unsetPixel(Bitmap * bmp, int16_t x, int16_t y)
{
    if ( x < 0 || y < 0 || x >= bmp->w || y >= bmp->h ) return;
    
    uint8_t * dst = bmp->image + ( y * ( bmp->w >> 1 ) ) + ( x >> 1 );

    if (x & 1) *dst &= 0xf0;
    else       *dst &= 0x0f;
}


/* ********************************************* */
// playerPtr->alienIdx is the current idx

void getAliensDeltaPos( int16_t * dX, int16_t * dY, uint8_t * row ) {
//    *row = getDivideBy11( playerPtr->alienIdx ); // / 11;
    *row = playerPtr->alienIdx / 11; // / 11;
    *dX = ( playerPtr->alienIdx - ( *row * 11 ) ) * 16;
    *dY = -( ( 8 + FLEETDELTAY ) * *row );
}

/* ********************************************* */
// playerPtr->alienIdx is the next idx

void getAliensDeltaPos2( int8_t idx, int16_t * dX, int16_t * dY ) {
    int16_t _dx = 0, _dy = 0;
    if ( idx >= playerPtr->alienIdx ) {
        if ( state1.downFleet ) {
            _dy = FLEETDELTAY;
        } else {
            _dx = playerPtr->aliensDeltaX < 0 ? -2 : playerPtr->numAliens > 1 ? 2 : 3;
        }
    }

    //int8_t r = getDivideBy11( idx ); // = idx / 11;
    int8_t r = idx / 11;
    *dX = ( idx - r * 11 ) * 16 - _dx;
    *dY = -( ( 8 + FLEETDELTAY ) * r + _dy );
}

/* ********************************************* */
/*
int16_t getDivideBy8( int16_t n ) {
    int8_t i = 0;
    int8_t u = 8;
    while( n - u >= 0 ) {
        u <<= 1;
        i++;
    }
    return i;
}
*/


int8_t getAlienColumn( uint8_t x ) {
    int16_t delta = x - playerPtr->aliensX;

    if ( delta < 0 || delta > 175 ) return -1;
    delta /= 16; // divide by 16
//    delta = getDivideBy16( delta );
    return ( int8_t ) delta;
}

/* ********************************************* */
// playerPtr->alienIdx is the next idx

int16_t getAlienIdx( int16_t x, int16_t y, int8_t skipAdjust ) {
    int16_t idx = -1;

    int16_t c = x - playerPtr->aliensX;
    if ( c < 0 || c > 175 ) return -1;
    c /= 16; // divide by 16
//    c = getDivideBy16( c );

    int16_t r = playerPtr->aliensY - y;
    if ( r < 0 || r > ( 4 * ( 8 + FLEETDELTAY ) + 7 ) ) return -1;
    r /= 8 + FLEETDELTAY;
//    r = getDivideBy12( r );

    idx = r * 11 + c;

    if ( !skipAdjust && idx >= playerPtr->alienIdx ) {
        int16_t _dx = 0, _dy = 0;
        if ( state1.downFleet ) {
            _dy = FLEETDELTAY;
        } else {
            _dx = playerPtr->aliensDeltaX < 0 ? -2 : playerPtr->numAliens > 1 ? 2 : 3;
        }
        return getAlienIdx( x + _dx, y + _dy, 1 );
    }

    aCol = c;
    aRow = r;

    return idx;
}

/* ********************************************* */

int8_t getLowestAlien( int8_t col ) {
    if ( col < 0 || col > 10 ) return -1;

    int8_t i;
    for ( i = 0; i < 55; i+=11 ) {
        if ( playerPtr->aliens[ col + i ] ) return col + i;
    }
    return -1;
}

/* ********************************************* */

int16_t getReloadRate() {
    int n;
    if ( !playerPtr->score ) return ShotReloadRate[ 0 ];
    for ( n = 0; n < sizeof( AReloadScoreTab ) / sizeof ( AReloadScoreTab[ 0 ] ) && playerPtr->score > AReloadScoreTab[ n ]; n++ );
    return ShotReloadRate[ n - 1 ];
}

/* ********************************************* */

void resetRoundVars() {

    /* Aliens */

    alienAnimationStatus = 0;

    alienExplodingCnt = 0;

    /* Saucer */

    saucerStart = 0;
    saucerActive = 0;

    saucerExplodingCnt = 0;

    /* Player */

    playerX = 14;
    shotY = -1;
    state1.shotReady = 1;
    shotExplodingCnt = 0;

    playerExplodingCnt = 0;

    /* Alien Shot */

    alienShotTimer = 0;

    reloadCnt = 0;
//    aShotFrame = 0;

    /* General for round */

    saucerFrames = 0;
    aliensFrames = 0;
    playerFrames = -100;
    shotFrames = 0;

    state1.playerIsAlive = 1;
    state1.waitForEventsComplete = 0;
    state1.downFleet = 0;

    anybodyExploding = 0;

    int i;
    for ( i = 0; i < 3; i++ ) {
        alienShotInfo[ i ].active = 0;
        alienShotInfo[ i ].aniFrame = 0;
        alienShotInfo[ i ].explodingCnt = 0;
    }

}

/* ********************************************* */

void resetPlayer( int p, int8_t isFirst ) {
    int n;

    playerPtr = &player[ p ];

    if ( isFirst ) {
        playerPtr->score = 0;

        /* Alien Shot */

        playerPtr->pluShotColIdx = 0;
        playerPtr->squShotColIdx = 6;

        playerPtr->numShips = numLives;
        playerPtr->round = 0;

        playerPtr->reloadRate = demo_mode ? 8 : getReloadRate();

    } else {
        playerPtr->round++;
    }

    /* Aliens */

    playerPtr->numAliens = 55;

    playerPtr->leftLimitCol = 0;
    playerPtr->rightLimitCol = 10;

    playerPtr->leftLimit = 0;
    playerPtr->rightLimit = 256 - ( ( playerPtr->rightLimitCol * 16 ) + 16 ) - 1;

    playerPtr->aliensColMask = 0b11111111111;

    for ( n = 0; n < playerPtr->numAliens; n++ ) playerPtr->aliens[ n ] = 1;

    playerPtr->fleetTopBase = ( playerPtr->round == 0 ? FLEETPOSSTART : AlienStartTable[ ( playerPtr->round - 1 ) & 7 ] );

    playerPtr->aliensX = 40;
    playerPtr->aliensY = playerPtr->fleetTopBase;
    playerPtr->aliensDeltaX = 2;
    
    playerPtr->alienIdx = 0;

    /* Saucer */

    playerPtr->tillSaucer = SAUCERTIMER; // Original Arcade 0x0600
    playerPtr->sauScore = 0;

    /* Alien Shot */

    playerPtr->alienShotMask = 0b00101101;

    /* Player */

    playerPtr->shotsCounter = 0;

    /* Shield */

    playerPtr->currentShieldTopY = SHIELDTOPY;

    for ( n = 0; n < 4; n++ ) {
        if ( playerPtr->shield[ n ] ) { free( playerPtr->shield[ n ] ); playerPtr->shield[ n ] = NULL; }
        if ( !playerPtr->shield[ n ] ) playerPtr->shield[ n ] = cloneBitmap( &ShieldImage );
    }

    asRol = &alienShotInfo[ AS_ROL ];
    asPlu = &alienShotInfo[ AS_PLU ];
    asSqu = &alienShotInfo[ AS_SQU ];

    for ( n = 0; n < 3; n++ ) {
        alienShotInfo[ n ].active = 0;
        alienShotInfo[ n ].aniFrame = 0;
        alienShotInfo[ n ].explodingCnt = 0;
    }

    if ( !demo_mode ) SoundStop( -1 );
}

/* ********************************************* */

void DrawShields() {
    int n;
    for ( n = 0; n < 4; n++ ) {
        VDP_drawBitmapEx( BG_A, playerPtr->shield[ n ], TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX + n * 6 ), MARGIN_X_TILE + 4 + 7 * n, SHIELDTOPY / 8, TRUE);
    }
}

/* ********************************************* */

void DrawShips() {
    int n;

    PrintNumAt( 1, 27, playerPtr->numShips, 1 | NUM_LEFTZERO );
    int8_t l = ( ( playerPtr->numShips > 6 ) ? 6 : playerPtr->numShips ) - 1;
    for ( n = 0; n < l; n++ ) {
        SPR_setVisibility(spr_lifes[ n ], VISIBLE);
    }
    for ( ; n < 6; n++ ) {
        SPR_setVisibility(spr_lifes[ n ], HIDDEN);
    }
    SPR_update();
    SYS_doVBlankProcess();
}

/* ********************************************* */

void scoreUpdate( int16_t points ) {
    // Extra Life
    if ( playerPtr->score < bonusLife && ( playerPtr->score + points ) >= bonusLife ) {
        SoundPlay( SOUND_EXTRA_LIFE, 0 );

        playerPtr->numShips++;
        DrawShips();
    }

    playerPtr->score += points;
    PrintNumAt( !currPlayer ? 2 : 26, 1, playerPtr->score, 4 | NUM_LEFTZERO );

    playerPtr->reloadRate = getReloadRate();

}

/* ********************************************* */

void hiScoreUpdate( int16_t score ) {
    if ( score > hiScore ) {
        hiScore = score;
        PrintNumAt( 13, 1, hiScore, 4 | NUM_LEFTZERO | NUM_UNSIGNED );
    }
}

/* ********************************************* */

void createAlienShotCol( _alienShotInfo * asi, int8_t * colIdx, int8_t colMin, int8_t colMax ) {
    if ( !asi->active && !asi->explodingCnt && !shotExplodingCnt ) {
        uint8_t col = ColFireTable[ ( int ) *colIdx ];

        int8_t aidx = getLowestAlien( col );
        if ( aidx != -1 ) {
            getAliensDeltaPos2( aidx, &asi->x, &asi->y );
            asi->x += playerPtr->aliensX + 8 - 1;
            asi->y += playerPtr->aliensY + 16;
            asi->active = 1;
            asi->aniFrame = 0;
            reloadCnt = 0;
        }
        ( *colIdx )++;
        if ( *colIdx > colMax ) *colIdx = colMin;
    }
}

/* ********************************************* */

void handleAlienShot( int aid ) {
    _alienShotInfo * asi = &alienShotInfo[ aid ];
    Sprite * _sprite = spr_alien_shot[ aid ];

    int8_t hit = 0;

    SPR_setVisibility( _sprite, HIDDEN );

    asi->y += 2;
    asi->aniFrame++;

    if ( asi->aniFrame >= 4 ) asi->aniFrame = 0;

    if ( asi->y <= PLAYERY + 8 ) {
        if ( asi->y + 8 > PLAYERY && asi->x < playerX + 14 && asi->x + 2 > playerX + 2 ) {
            asi->active = 0;
            if ( state1.playerIsAlive ) {
                playerExplodingCnt = 60; // 0x100;
                state1.waitForEventsComplete = 1;
                state1.playerIsAlive = 0;
                if ( !demo_mode ) {
                    SoundStop( SOUND_PLAYER_SHOT );
                    SoundPlay( SOUND_PLAYER_EXPLOSION, 0 );
                }
            }
        } else {
            // Shield Collision?
            if ( asi->y + 8 > playerPtr->currentShieldTopY - 4 && asi->y + 8 < SHIELDTOPY + 16 ) {
                int n;
                for ( n = 0; n < 4; n++ ) {
                    int px = asi->x - ( 32 + 56 * n );

                    if ( px >= 0 && px < 24 ) {
                        int py = asi->y - SHIELDTOPY; //playerPtr->currentShieldTopY;

                        if ( getPixel( playerPtr->shield[ n ], px    , py + 6 ) || 
                             getPixel( playerPtr->shield[ n ], px + 1, py + 6 ) || 
                             getPixel( playerPtr->shield[ n ], px + 2, py + 6 )
                           ) {

                            asi->y += 2;

                            // hay que borrar el escudo tambien
                            // need fix (max 3)
                            SPR_setVisibility( spr_alien_shot_explosion[ aid ], VISIBLE );
                            SPR_setPosition( spr_alien_shot_explosion[ aid ], asi->x - 2 + MARGIN_X_PX, asi->y );

                            asi->active = 0;
                            asi->explodingCnt = 8; // 60;
                            hit = 1;

                            // Explosion Mask
                            uint8_t points[] = {
                                                    0, 2,
                                                    1, 0,   1, 4,
                                                    2, 2,   2, 3,   2, 5,
                                                    3, 1,   3, 2,   3, 3,   3, 4,
                                                    4, 0,   4, 2,   4, 3,   4, 4,
                                                    5, 1,   5, 2,   5, 3,   5, 4,   5, 5,
                                                    6, 0,   6, 2,   6, 3,   6, 4,
                                                    7, 1,   7, 3,   7, 5
                                               };

                            int i;

                            px -= 2;
                            py += 2;

                            for ( i = 0; i < sizeof( points ) / sizeof( points[ 0 ] ); i += 2 ) {
                                unsetPixel( playerPtr->shield[ n ], px + points[ i + 1 ], py + points[ i ] );
                            }

                            VDP_drawBitmapEx( BG_A, playerPtr->shield[ n ], TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX + n * 6 ), MARGIN_X_TILE + 4 + 7 * n, SHIELDTOPY / 8, TRUE);

                            break;
                        }
                    }
                }
            }

            if ( !hit ) {
                SPR_setVisibility( _sprite, VISIBLE );
                SPR_setPosition( _sprite, asi->x + MARGIN_X_PX, asi->y );
                SPR_setAnim( _sprite, asi->aniFrame );
            }
        }
    } else {
        asi->y = PLAYERY + 8;

        SPR_setVisibility( spr_alien_shot_explosion[ aid ], VISIBLE );
        SPR_setPosition( spr_alien_shot_explosion[ aid ], asi->x - 2 + MARGIN_X_PX, asi->y );

        asi->active = 0;
        asi->explodingCnt = 8; //60;
    }
}

/* ********************************************* */

void drawShotExploding( uint8_t y ) {
    SPR_setVisibility( spr_player_shot_explosion, VISIBLE );
    SPR_setPosition( spr_player_shot_explosion, shotX - 4 + MARGIN_X_PX, y );
}

/* ********************************************* */

void handleAShotCollision( int aid, int8_t aShotYH ) {
    _alienShotInfo * asi = &alienShotInfo[ aid ];
    Sprite * _sprite = spr_alien_shot[ aid ];

    if ( ( uint16_t ) ( shotX - asi->x ) < 3 && shotY + 4 < asi->y + aShotYH && shotY + 8 > asi->y ) {
        SPR_setVisibility( spr_player_shot, HIDDEN );

        SPR_setVisibility( _sprite, HIDDEN );

        SPR_setVisibility( spr_alien_shot_explosion[ aid ], VISIBLE );
        SPR_setPosition( spr_alien_shot_explosion[ aid ], asi->x - 3 + MARGIN_X_PX, asi->y + aShotYH );

        SPR_setVisibility( spr_player_shot_explosion, VISIBLE );
        SPR_setPosition( spr_player_shot_explosion, shotX - 4 + MARGIN_X_PX, shotY + 2 );

        shotY = -shotY;
        asi->active = 0;
        asi->explodingCnt = 8; //60;
        shotExplodingCnt = 8; //60;
        state1.killed = 1;
    }
}

/* ********************************************* */

void handleDeleteAShotExplodingCnt( int aid ) {
    _alienShotInfo * asi = &alienShotInfo[ aid ];
    if ( asi->explodingCnt ) {
        asi->explodingCnt--;
        if ( !asi->explodingCnt ) {
            SPR_setVisibility( spr_alien_shot_explosion[ aid ], HIDDEN );
        }
    }
}

/* ********************************************* */

void handleSauScore() {
    playerPtr->shotsCounter++;
    playerPtr->sauScore++;
    if ( playerPtr->sauScore == sizeof( SaucerScrTab ) / sizeof( SaucerScrTab[ 0 ] ) ) {
        playerPtr->sauScore = 0;
    }
}

/* ********************************************* */

int8_t iterGame() {
    int n;

    anybodyExploding = 0;

    if ( demo_mode ) {
        readInput();

        if ( syskey.keyPressed ) return 2; // Key pressed on demo mode
        if ( playerFrames >= 0 ) {
            demoFrame++;
            if ( demoFrame >= DEMOSPEED ) {
                uint8_t key;
                demoFrame = 0;
                key = DemoCommands[ demoCurCommand ];
                input.moveLeft  = ( key & 2 ) >> 1;
                input.moveRight =   key & 1;
                input.shot = 1;
                state1.shotReady = 1;
                demoCurCommand++;
                if ( demoCurCommand > sizeof( DemoCommands ) / sizeof( DemoCommands[ 0 ] ) ) {
                    demoCurCommand = 0;
                }
            }
        }
    }

    if ( playerPtr->aliensY != playerPtr->fleetTopBase ) {
        playerPtr->tillSaucer--;
        if ( playerPtr->tillSaucer <= 0 ) {
            saucerStart = 1;
            playerPtr->tillSaucer = SAUCERTIMER; // Original Arcade 0x0600
        }
    }

    // Clear Alien Explosion
    if ( alienExplodingCnt ) {
        alienExplodingCnt--;
        if ( !alienExplodingCnt ) {
            SPR_setVisibility( spr_alien_explosion, HIDDEN );
            if ( !playerPtr->numAliens ) {
                resetPlayer( currPlayer, 0 );
                return 3; // Next round
            }                
            handleSauScore();
        } else {
            anybodyExploding = 1;            
        }
    }

    // Clear Shot Explosion
    if ( shotExplodingCnt ) {
        shotExplodingCnt--;
        if ( !shotExplodingCnt ) {
            SPR_setVisibility( spr_player_shot_explosion, HIDDEN );
            if ( saucerActive == 1 && shotY == -16 ) {
                SPR_setVisibility( spr_ufo, VISIBLE );
                SPR_setPosition( spr_ufo, saucerX + MARGIN_X_PX, SAUCERROW );
            }
            handleSauScore();
            shotY = -1;
        } else {
            anybodyExploding = 1;
        }
    }

    // Saucer Explosion
    if ( saucerActive < 0 ) {
        saucerExplodingCnt--;
        if ( saucerExplodingCnt == 24 ) {
            anybodyExploding = 1;
            handleSauScore();
            if ( !demo_mode ) scoreUpdate( SaucerScrTab[ ( int ) playerPtr->sauScore ] );
            SPR_setVisibility( spr_ufo_explosion, HIDDEN );
            PrintNumAt( saucerX / 8, 2, SaucerScrTab[ ( int ) playerPtr->sauScore ], 3 );
        } else if ( !saucerExplodingCnt ) {
            PrintAt( saucerX / 8, SAUCERROW / 8, "   " );
            saucerActive = 0;
        }
    }

    handleDeleteAShotExplodingCnt( AS_ROL );
    handleDeleteAShotExplodingCnt( AS_PLU );
    handleDeleteAShotExplodingCnt( AS_SQU );

    if ( state1.playerIsAlive && playerPtr->numAliens ) {
//        handleNextAlien();

        aliensFrames++;
        if ( aliensFrames >= ALIENSPEED ) {
            aliensFrames = 0;
            for ( ; playerPtr->alienIdx < 55 && !playerPtr->aliens[ ( int ) playerPtr->alienIdx ]; playerPtr->alienIdx++ );

            if ( playerPtr->alienIdx == 55 ) {
                for ( playerPtr->alienIdx = 0; playerPtr->alienIdx < 55 && !playerPtr->aliens[ ( int ) playerPtr->alienIdx ]; playerPtr->alienIdx++ );

                // Alien Sound
                if ( !demo_mode ) {
                    if ( !SoundIsPlaying( SOUND_ALIEN_EXPLOSION ) ) {
                        SoundStop( SOUND_ALIEN_STEP1 + ( alienSoundCnt & 0x03 ) );
                        SoundPlay( SOUND_ALIEN_STEP1 + ( ( ++alienSoundCnt ) & 0x03 ), 0 );
                    }
                }

                alienAnimationStatus ^= 1;            

                // Right to Left allways -2
                tDeltaX = playerPtr->aliensDeltaX < 0 ? -2 : playerPtr->numAliens > 1 ? 2 : 3;
                playerPtr->aliensX += tDeltaX;

                state1.downFleet = 0;

                if ( playerPtr->aliensX < playerPtr->leftLimit || playerPtr->aliensX > playerPtr->rightLimit ) {
                    state1.downFleet = 0x02;

                    playerPtr->aliensX -= tDeltaX;
                    playerPtr->aliensDeltaX = -playerPtr->aliensDeltaX;

                    playerPtr->aliensY += FLEETDELTAY;
                }
            }

            int16_t dX, dY;
            uint8_t row;
            getAliensDeltaPos( &dX, &dY, &row );
            uint8_t posX = playerPtr->aliensX + dX,
                    posY = playerPtr->aliensY + dY;
            if ( state1.downFleet ) {
                if ( ( state1.downFleet & 0x02 ) && posY + 8 > playerPtr->currentShieldTopY ) {
                    if ( posY - SHIELDTOPY < 16 ) {
                        for ( n = 0; n < 4; n++ ) {
                            int y = posY - SHIELDTOPY;
                            Bitmap * bmp = playerPtr->shield[ n ];
                            uint8_t * dst = bmp->image + ( y * ( bmp->w >> 1 ) );
                            memset( dst, '\0', bmp->w << 2 ) ; // w / 2 * 8

                            VDP_drawBitmapEx( BG_A, playerPtr->shield[ n ], TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX + n * 6 ), MARGIN_X_TILE + 4 + 7 * n, SHIELDTOPY / 8, TRUE);
                        }
                    }
                    playerPtr->currentShieldTopY = posY + 8;
                    state1.downFleet = 0x01;
                }

                // Aliens reached the Player
                if ( posY >= PLAYERY ) 
                {
                    playerExplodingCnt = 60; //0x100;
                    state1.waitForEventsComplete = 1;
                    state1.playerIsAlive = 0;

                    if ( !demo_mode ) {
                        SoundStop( SOUND_PLAYER_SHOT );
                        SoundPlay( SOUND_PLAYER_EXPLOSION, 0 );
                    }

                    if ( playerPtr->numShips ) playerPtr->numShips = 1;
                }
            }

            Sprite * sprite = spr_aliens[ row * 11 + ( playerPtr->alienIdx % 11 ) ];

            SPR_setVisibility( sprite, VISIBLE );
            SPR_setPosition( sprite, posX + MARGIN_X_PX, posY );
            SPR_setAnim( sprite, alienAnimationStatus );
  
            playerPtr->alienIdx++;
        }


        /* Player */

        playerFrames++;
        if ( playerFrames >= PLAYERSPEED ) {
            playerFrames = 0;

            if ( !demo_mode ) readInput();

            if ( input.moveLeft  && playerX > 0   ) playerX--;
            if ( input.moveRight && playerX < 240 ) playerX++;

            SPR_setVisibility( spr_player, VISIBLE );
            SPR_setPosition( spr_player, playerX + MARGIN_X_PX, PLAYERY );
        }

        if ( saucerStart && !asSqu->active && playerPtr->numAliens >= 8 ) {
            saucerActive = 1;
            if ( playerPtr->shotsCounter & 1 ) {
                saucerDeltaX = 2;
                saucerX = 0;
            } else {
                saucerDeltaX = -2;
                saucerX = 232;
            }
            saucerFrames = SAUCERSPEED;
            saucerStart = 0;
        }
    }

    if ( playerExplodingCnt ) {
        SPR_setVisibility( spr_player, HIDDEN );
        playerExplodingCnt--;
        if ( playerExplodingCnt ) {
            anybodyExploding = 1;
            if ( !( playerExplodingCnt & 0x03 ) ) {
                SPR_setVisibility( spr_player_explosion, VISIBLE );
                SPR_setPosition( spr_player_explosion, playerX + MARGIN_X_PX, PLAYERY );
                SPR_setAnim( spr_player_explosion, ( playerExplodingCnt & 0x04 ) ? 1 : 0 );
            }
        } else {
            SPR_setVisibility( spr_player_explosion, HIDDEN );
            playerPtr->numShips--;
            DrawShips();
        }
    }

    if ( state1.waitForEventsComplete                                           &&
         !anybodyExploding                                                      &&
         !( asRol->explodingCnt | asPlu->explodingCnt | asSqu->explodingCnt )   &&
         !( asRol->active | asPlu->active | asSqu->active | saucerActive )      &&
         shotY == -1 
    ) {

        if ( !demo_mode ) SoundStop( -1 );

        if ( demo_mode ) return 0; // demo complete

        if ( !playerPtr->numShips ) return 4; // game over

        return 6; // player death, but have ships availables
    }

    if ( saucerActive == 1 ) {
        if ( !demo_mode ) SoundPlay( SOUND_UFO, 0 );

        saucerFrames++;
        if ( saucerFrames >= SAUCERSPEED ) {
            saucerFrames = 0;
            saucerX += saucerDeltaX;
            if ( saucerX < 0 || saucerX > 232 ) {
                SPR_setVisibility( spr_ufo, HIDDEN );
                saucerActive = 0;
            } else {
                SPR_setVisibility( spr_ufo, VISIBLE );
                SPR_setPosition( spr_ufo, saucerX + MARGIN_X_PX, SAUCERROW );
            }
        }
    }

    shotFrames++;
    if ( playerFrames >= 0 /*shotFrames >= SHOTSPEED */ ) {
        shotFrames = 0;

        if ( !demo_mode ) {
            readInput();
            if ( !input.shot ) state1.shotReady = 1;
        }

        if ( input.shot && state1.shotReady && shotY == -1 && state1.playerIsAlive && !anybodyExploding ) {
            if ( !demo_mode ) SoundPlay( SOUND_PLAYER_SHOT, 0 );

            shotY = PLAYERY - 8;
            shotX = playerX + 8;
            state1.shotReady = 0;
        }

        if ( shotY > 0 ) {
            shotY -= 4;

            state1.killed = 0;

            // Shot collision with aliens shots

            if ( asRol->active ) handleAShotCollision( AS_ROL, 7 );
            if ( asPlu->active ) handleAShotCollision( AS_PLU, 6 );
            if ( asSqu->active ) handleAShotCollision( AS_SQU, 7 );

            // Shot collision with saucer
            if ( !state1.killed && saucerActive > 0 && 
                  shotY + 8 >= SAUCERROW && shotY + 4 < ( SAUCERROW + 8 ) &&
                  shotX >= saucerX + 4 && shotX < saucerX + 20
               ) {
                SPR_setVisibility( spr_player_shot, HIDDEN );
                SPR_setVisibility( spr_ufo, HIDDEN );

                SPR_setVisibility( spr_ufo_explosion, VISIBLE );
                SPR_setPosition( spr_ufo_explosion, saucerX + MARGIN_X_PX, SAUCERROW );

                saucerActive = -1;
                saucerExplodingCnt = 32;

                if ( !demo_mode ) {
                    SoundStop( SOUND_UFO );
                    SoundPlay( SOUND_UFO_EXPLOSION, 0 );
                }

                shotY = -1;
                state1.killed = 1;
            }

            // Shot collision with shield
            if ( !state1.killed && shotY + 8 > playerPtr->currentShieldTopY && shotY + 4 < SHIELDTOPY + 16 ) {

                for ( n = 0; n < 4; n++ ) {
                    int px = shotX - ( 32 + 56 * n );

                    if ( px >= 0 && px < 24 ) {
                        int py = shotY + 4 - SHIELDTOPY;

                        if ( getPixel( playerPtr->shield[ n ], px, py     ) || 
                             getPixel( playerPtr->shield[ n ], px, py + 1 ) || 
                             getPixel( playerPtr->shield[ n ], px, py + 2 ) || 
                             getPixel( playerPtr->shield[ n ], px, py + 3 )
                           ) {

                            SPR_setVisibility( spr_player_shot, HIDDEN );

                            shotY = -( shotY + 2 );
                            drawShotExploding( -shotY );
                            shotExplodingCnt = 6; //60;
                            state1.killed = 1;

                            // Explosion Mask
                            uint8_t points[] = {
                                                    0, 0,   0, 4,   0, 7,
                                                    1, 2,   1, 6,
                                                    2, 1,   2, 2,   2, 3,   2, 4,   2, 5,   2, 6,
                                                    3, 0,   3, 1,   3, 2,   3, 3,   3, 4,   3, 5,   3, 6,   3, 7,
                                                    4, 0,   4, 1,   4, 2,   4, 3,   4, 4,   4, 5,   4, 6,   4, 7,
                                                    5, 1,   5, 2,   5, 3,   5, 4,   5, 5,   5, 6,
                                                    6, 2,   6, 6,
                                                    7, 0,   7, 3,   7, 7
                                               };

                            int i;

                            px -= 4;
                            py -= 2;

                            for ( i = 0; i < sizeof( points ) / sizeof( points[ 0 ] ); i += 2 ) {
                                unsetPixel( playerPtr->shield[ n ], px + points[ i + 1 ], py + points[ i ] );
                            }

                            VDP_drawBitmapEx( BG_A, playerPtr->shield[ n ], TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX + n * 6 ), MARGIN_X_TILE + 4 + 7 * n, SHIELDTOPY / 8, TRUE);

                            break;
                        }
                    }
                }
            }

            // Shot collision with aliens
            if ( !state1.killed ) {
                int16_t idx = getAlienIdx( shotX, shotY /*+ 4 - 6*/, 0 ), lowest;
                if ( idx >= 0 && idx < 55 && playerPtr->aliens[ idx ] ) {

// Alien A = 12px width (2 + 12 + 2)
// Alien B = 11px width (3 left + 11 alien + 2 right)
// Alien C = 8px width  (4 + 8 + 4)
                    getAliensDeltaPos2( idx, &alienExplodingX, &alienExplodingY );
                    alienExplodingX += playerPtr->aliensX;
                    alienExplodingY += playerPtr->aliensY;

                    int hit = 0;

                    switch ( aRow ) {
                        case 0:
                        case 1:
                            if ( shotX >= alienExplodingX + 2 && shotX < alienExplodingX + 14 ) hit = 1;
                            break;

                        case 2:
                        case 3:
                            if ( shotX >= alienExplodingX + 3 && shotX < alienExplodingX + 14 ) hit = 1;
                            break;

                        case 4:
                            if ( shotX >= alienExplodingX + 4 && shotX < alienExplodingX + 12 ) hit = 1;
                            break;

                    }

                    if ( hit ) {
                        SPR_setVisibility( spr_player_shot, HIDDEN );

                        SPR_setVisibility( spr_aliens[ idx ], HIDDEN );
                        SPR_setVisibility( spr_alien_explosion, VISIBLE );
                        SPR_setPosition( spr_alien_explosion, alienExplodingX + MARGIN_X_PX, alienExplodingY );

                        playerPtr->aliens[ idx ] = 0;
                        playerPtr->numAliens--;
                        alienExplodingCnt = 8;
                        shotY = -1;
                        state1.killed = 1;

                        if ( !demo_mode ) {
                            scoreUpdate( AlienScores[ aRow ] );

                            SoundStop( SOUND_ALIEN_STEP1 );
                            SoundStop( SOUND_ALIEN_STEP2 );
                            SoundStop( SOUND_ALIEN_STEP3 );
                            SoundStop( SOUND_ALIEN_STEP4 );

                            SoundPlay( SOUND_ALIEN_EXPLOSION, 0 );
                        }

                        if ( playerPtr->numAliens < 9 ) playerPtr->alienShotMask = 0b111111;

                        if ( ( lowest = getLowestAlien( aCol ) ) == -1 ) playerPtr->aliensColMask &= ~( 0x0001u << aCol );

                        if ( lowest && ( aCol == playerPtr->leftLimitCol || aCol == playerPtr->rightLimitCol ) ) {
                            if ( getLowestAlien( aCol ) == -1 ) {
                                int16_t ll = 10, lr = 0;
                                uint16_t bitl = 0x0400u, bitr = 0x0001u;

                                playerPtr->aliensColMask &= ~( 0x0001u << aCol );

                                for ( n = 0; n < 11; n++, bitl >>= 1, bitr <<= 1 ) {
                                    if ( playerPtr->aliensColMask & bitl ) ll = ( 10 - n );
                                    if ( playerPtr->aliensColMask & bitr ) lr = n;
                                }

                                playerPtr->leftLimitCol = ll;
                                playerPtr->rightLimitCol = lr;

                                playerPtr->leftLimit  = -( ll * 16 );
                                playerPtr->rightLimit = 256 - ( ( lr * 16 ) + 16 ) - 1;
                            }
                        }
                    }
                }
            }

            if ( !state1.killed ) {
                if ( shotY <= 16 ) {
                    SPR_setVisibility( spr_player_shot, HIDDEN );
                    drawShotExploding( 16 );
                    shotY = -( 16 /*- 2*/ );
                    shotExplodingCnt = 4; //40;
                    state1.killed = 1;
                } else {
                    SPR_setVisibility( spr_player_shot, VISIBLE );
                    SPR_setPosition( spr_player_shot, shotX + MARGIN_X_PX, shotY );
                }
            }
        }
    }

    /* Aliens Shots */

//    aShotFrame++;
//    if ( aShotFrame >= ALIENSHOTSPEED ) {
//        aShotFrame = 0;
    reloadCnt++;
    if ( state1.playerIsAlive && playerFrames >= 0 ) {
        if ( reloadCnt >= playerPtr->reloadRate ) {
            reloadCnt = 0;
            switch ( alienShotTimer ) {
                case    0:  // rolling-shot
                    if ( !asRol->active && !asRol->explodingCnt && !shotExplodingCnt ) {
                        // Track the player
                        int8_t aidx = getLowestAlien( getAlienColumn( playerX + 8 ) );

                        if ( aidx != -1 ) {
                            getAliensDeltaPos2( aidx, &asRol->x, &asRol->y );
                            asRol->x += playerPtr->aliensX + 8 - 1;
                            asRol->y += playerPtr->aliensY + 16;
                            asRol->active = 1;
                            asRol->aniFrame = 0;
                        }
                    }
                    alienShotTimer++;
                    break;

                case    1:  // plunger-shot
                    if ( playerPtr->numAliens > 1 ) createAlienShotCol( asPlu, &playerPtr->pluShotColIdx, 0, 15 );
                    alienShotTimer++;
                    break;

                case    2:  // squiggly-shot or saucer
                    if ( !saucerActive ) createAlienShotCol( asSqu, &playerPtr->squShotColIdx, 6, 20 );
                    alienShotTimer = 0;
                    break;

            }
        }
    }
    
    if ( alienShotFrameCnt & playerPtr->alienShotMask ) {
        if ( asRol->active ) handleAlienShot( AS_ROL );
        if ( asPlu->active ) handleAlienShot( AS_PLU );
        if ( asSqu->active ) handleAlienShot( AS_SQU );
    }
    if ( alienShotFrameCnt == 0b00100000 ) alienShotFrameCnt = 0b00000001;
    else                                   alienShotFrameCnt <<= 1;
//    }

    return 1; // frame complete, return for continue

}

/* ********************************************* */

int8_t playGame( int8_t _demo ) {

    int8_t n;

    demo_mode = _demo;

    /* Reset Players */

    resetPlayer( 1, 1 );
    resetPlayer( 0, 1 );

    /* Round */

    currPlayer = 0;

    if ( demo_mode ) playersInGame = 1;

start_round:
    gameState = 0;

    ClearGameArea();

    DrawShips();
    
    if ( !demo_mode ) {
        DrawScoreHeaderAndCredits();
        PrintAt(  10,  9, "PLAY PLAYER< >" );
        PrintChar( 22,  9, ( !currPlayer ? '1' : '2' ), 0 );

        for ( n = 0; n < 17; n++ ) {
            PrintAt( !currPlayer ? 2 : 26, 1, "      " );
            DelayFrames(3);

            PrintNumAt( !currPlayer ? 2 : 26, 1, playerPtr->score, 4 | NUM_LEFTZERO );
            DelayFrames(3);

        }
        ClearGameArea();
    }
    
    DrawShields();

    PrintAt( 0, 26, "################################" );

    resetRoundVars();

    gameState = 1;

    // Main Loop
    while ( 1 ) {
        gameState = iterGame();

        SPR_update();
        SYS_doVBlankProcess();

        switch ( gameState ) {
            case    0: // demo complete
                DelayFrames( 100 ); // 2 seconds
                ClearGameArea();
                demo_mode = 0;
                return 0;

            case    1: // continue game
                break;

            case    2: // key pressed on demo mode
                ClearGameArea();
                demo_mode = 0;
                return 2;

            case    3: // next round
                DelayFrames( 100 ); // 2 seconds
                goto start_round;

            case    4: // game over
                goto game_over;

            case    6: // player death a life
                    DelayFrames( 100 ); // 2 seconds
                    reloadCnt = 0;
                    if ( playersInGame > 1 ) {
                        if ( player[ currPlayer ^ 1 ].numShips > 0 ) {
                            currPlayer ^= 1;
                            playerPtr = &player[ currPlayer ];
                            goto start_round;
                        }
                    }
                    state1.waitForEventsComplete = 0;
                    state1.playerIsAlive = 1;
                    playerX = 14;
                    gameState = 1; // continue
                    break;

        }
    }

game_over:
    hiScoreUpdate( playerPtr->score );

    if ( playersInGame == 1 ) {
        PrintAtDelay( 11, 3, "GAME OVER", 6 );
        DelayFrames( 100 ); // 2 seconds
        ClearGameArea();
        return 1;

    } else if ( playersInGame == 2 ) {
        PrintAtDelay( 6, ( uint8_t ) ( PLAYERY / 8 + 1 ), "GAME OVER  PLAYER< >", 6 );
        PrintChar( 24, ( uint8_t ) ( PLAYERY / 8 + 1 ), ( !currPlayer ? '1' : '2' ), 0 );
        DelayFrames( 100 ); // 2 seconds

        currPlayer ^= 1;
        playerPtr = &player[ currPlayer ];

        if ( !playerPtr->numShips ) {
            ClearGameArea();
            return 1;
        }

        goto start_round;
    }

    gameState = 0;
    ClearGameArea();

    return 1;

}

/* ********************************************* */
