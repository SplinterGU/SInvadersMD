#ifndef __COMMON_H
#define __COMMON_H

#include <genesis.h>

#include "player.h"

/* ********************************************* */

#define MARGIN_X_TILE   4
#define MARGIN_X_PX     ( MARGIN_X_TILE * 8 )


#define SHIELDTOPY      22 * 8 // 144
#define PLAYERY         192 + 8

#define SAUCERSPEED     3
#define SHOTSPEED       5
#define PLAYERSPEED     0 

#define SAUCERTIMER     0x500 
#define SAUCERROW       16

#define ALIENSPEED      1 
#define ALIENSHOTSPEED  2

#define FLEETDELTAY     8

#define FLEETPOSSTART   96 // 88

#define DEMOSPEED       50


#define NUM_UNSIGNED 0x80
#define NUM_LEFTZERO 0x60

/* ********************************************* */

extern int8_t demo_mode;
extern int8_t numLives;
extern int16_t bonusLife;
extern int8_t displayCoinage;
extern uint8_t numCredits;
extern uint16_t hiScore;
extern int8_t playersInGame;
extern int8_t currPlayer;

extern _player * playerPtr;

extern _player player[2];

/* ********************************************* */

#endif
