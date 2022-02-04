#include <genesis.h>

#include "common.h"
#include "input.h"
#include "screens.h"

/* ********************************************* */

union _syskey syskey = { 0 };

/* Input */

union _input input = { 0 };

static uint8_t keyCreditEnable = 1;

/* ********************************************* */

void readInput() {
    uint16_t value1 = JOY_readJoypad(JOY_1);
    uint16_t value2 = JOY_readJoypad(JOY_2);

    if ( !demo_mode && playerPtr ) {
        uint16_t value = !currPlayer ? value1 : value2;
        input.moveLeft = ( value & BUTTON_LEFT ) ? 1 : 0;
        input.moveRight = ( value & BUTTON_RIGHT ) ? 1 : 0;
        input.shot = ( value & BUTTON_A ) ? 1 : 0;
    }

    if ( value1 & BUTTON_START || value2 & BUTTON_START ) { // Key Credits (+/k)
        int coins = 0;

        if ( demo_mode || !playersInGame ) {
            if ( value1 & BUTTON_START ) if ( numCredits < 1 ) coins = 1;
            if ( value2 & BUTTON_START ) if ( numCredits < 2 ) coins = 2 - numCredits;
        } else {
            if ( value1 & BUTTON_START ) coins++;
            if ( value2 & BUTTON_START ) coins++;
        }

        if ( keyCreditEnable ) {
            keyCreditEnable = 0;
            syskey.keyCredit = 1;
            if ( numCredits < 99 ) {
                numCredits += coins;
                DrawCredits();
            }
        }
    } else {
        syskey.keyCredit = 0;
        keyCreditEnable = 1;
    }

    if ( numCredits > 0 ) {
        // A - G
        if ( value1 & BUTTON_START ) { // One Player (1)
            syskey.keyStart1UP = 1;
        }

        if ( numCredits > 1 ) {
            if ( value2 & BUTTON_START ) { // Two Player (2)
                syskey.keyStart2UP = 1;
            }
        }
    }

    if ( value1 & BUTTON_C || value2 & BUTTON_C ) syskey.showSetup = 1;
    if ( value1 & BUTTON_B || value2 & BUTTON_B ) syskey.showHelp = 1;

}
