#include "common.h"

/* Others */

int8_t demo_mode = 0;

int8_t numLives = 3;
int16_t bonusLife = 1500;
int8_t displayCoinage = 1;
uint8_t numCredits = 0;
uint16_t hiScore = 0;
int8_t playersInGame = 0;
int8_t currPlayer = 0;

_player player[2];

_player * playerPtr = 0;
