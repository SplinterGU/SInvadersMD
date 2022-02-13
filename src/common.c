#include "common.h"

/* Others */

int8_t demo_mode = 0;

int8_t numLives = 3;
int16_t bonusLife = 1500;
int8_t displayCoinage = 1;
int8_t shakeEffect = 0;
int8_t color_mode = 0;

uint8_t numCredits = 0;
uint16_t hiScore = 0;
int8_t playersInGame = 0;
int8_t currPlayer = 0;

_player player[2]  = {
                        {
                            0, //numAliens
                            0, // leftLimitCol
                            0, // rightLimitCol
                            0, // leftLimit
                            0, // rightLimit
                            0, // aliensColMask
                            '\0', // aliens
                            0, // fleetTopBase
                            0, // aliensX
                            0, // aliensY
                            0, // aliensDeltaX
                            0, // alienIdx
                            0, // tillSaucer
                            0, // sauScore
                            0, // alienShotMask
                            0, // shotsCounter
                            0, // score
                            0, // reloadRate
                            0, // pluShotColIdx
                            0, // squShotColIdx
                            0, // numShips
                            0, // round
                            0, // currentShieldTopY
                            { NULL, NULL, NULL, NULL }, // shield[4]
                        },
                        {
                            0, //numAliens
                            0, // leftLimitCol
                            0, // rightLimitCol
                            0, // leftLimit
                            0, // rightLimit
                            0, // aliensColMask
                            '\0', // aliens
                            0, // fleetTopBase
                            0, // aliensX
                            0, // aliensY
                            0, // aliensDeltaX
                            0, // alienIdx
                            0, // tillSaucer
                            0, // sauScore
                            0, // alienShotMask
                            0, // shotsCounter
                            0, // score
                            0, // reloadRate
                            0, // pluShotColIdx
                            0, // squShotColIdx
                            0, // numShips
                            0, // round
                            0, // currentShieldTopY
                            { NULL, NULL, NULL, NULL }, // shield[4]
                        }
                    };

_player * playerPtr = NULL;

/* ********************************************* */
