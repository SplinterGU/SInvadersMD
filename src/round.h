#ifndef __ROUND_H
#define __ROUND_H

extern int8_t gameState;

extern void resetPlayer( int p, int8_t isFirst );
extern int8_t playGame( int8_t demo );
extern int8_t iterGame();
extern void loadSprites();

/* ********************************************* */

enum {
    AS_ROL = 0,
    AS_PLU,
    AS_SQU
};

/* ********************************************* */

extern Sprite * spr_ufo;
extern Sprite * spr_alien_shot[3];
extern Sprite * spr_alien_shot_explosion[3];

#endif
