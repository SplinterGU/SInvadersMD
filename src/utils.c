#include <genesis.h>

#include "utils.h"
#include "input.h"
#include "common.h"

/* ********************************************* */

int8_t _ExitIfKeyPressed = 0;

/* ********************************************* */

int8_t DelayFrames( uint8_t frames ) {
    while ( frames-- ) {
        SPR_update();
        SYS_doVBlankProcess();
        readInput();
        if ( _ExitIfKeyPressed && syskey.keyPressed ) return 1;
    }
    return 0;
}

/* ********************************************* */

char buff[ 16 ];

void PrintNumAt( uint8_t x, uint8_t y, uint16_t num, uint8_t flags ) {
    char *p = &buff[ 14 ];
    int8_t d, c = 0, leftzero = flags & NUM_LEFTZERO, digits = flags & 7;

    p[ 1 ] = '\0';

    uint16_t n = ( uint16_t ) num;
    while ( digits-- && p >= buff ) {
        d = n % 10;
        n /= 10;
        if ( !( n | d ) && !leftzero && c ) {
            *p-- = ' ';
        } else {
            *p-- = '0' + d;
        }
        c = 1;
    }
    p++;

    PrintAt( x, y, p );
}


/* ********************************************* */

void PrintChar( uint8_t x, uint8_t y, uint8_t ch, uint8_t mode ) {
    char str[2];
    str[0] = ch;
    str[1] = '\0';
    VDP_drawText( str, MARGIN_X_TILE + x, y);
};

/* ********************************************* */

void PrintAt( uint8_t x, uint8_t y, char * msg ) {
    VDP_drawText( msg, MARGIN_X_TILE + x, y);
}

/* ********************************************* */

int PrintAtDelay( uint8_t x, uint8_t y, char * msg, uint8_t delay ) {
    while ( *msg ) {
        if ( DelayFrames( delay ) ) return 1;
        PrintChar( x, y, *msg++, 0 );
        x++;
    }

    return 0;
}

/* ********************************************* */
