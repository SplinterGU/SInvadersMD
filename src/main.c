#include <genesis.h>

#include "font.h"
#include "image.h"
#include "sprites.h"
#include "dma.h"

#include "common.h"

#include "input.h"

#include "utils.h"

#include "screens.h"

#include "round.h"

#include <smp_null_pcm.h>
#include <z80_ctrl.h>

//#include "z80_3adpcm_drv.h"

#define PLAN_A  BG_A
#define PLAN_B  BG_B

u16 custom_palette1[] = { 
    0, VDPPALETTE_REDMASK|VDPPALETTE_GREENMASK|VDPPALETTE_BLUEMASK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
} ;

u16 custom_palette2[] = { 
    0, VDPPALETTE_GREENMASK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
} ;


int main()
{
    memset( player, '\0', sizeof( player ) );

    //disable interrup
    SYS_disableInts();

//	load_Z80Driver();

    //320x224px
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

	SPR_initEx(184);

   	/* Configuraci�n de dise�o de fuente */
    VDP_loadFontData(custom_font.tiles, 59, DMA);
    /* Lectura de datos de la paleta */
    VDP_setPalette(PAL0, custom_palette1 );
    /* Configuraci�n de la paleta de fuentes */
    VDP_setTextPalette(PAL0);

    /* Lectura de datos de la paleta */
    VDP_setPalette(PAL1, custom_palette2 );

    // VDP process done, we can re enable interrupts
    SYS_enableInts();

	VDP_drawImageEx(PLAN_B, &background, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_USERINDEX + 6 * 8), 0, 0, TRUE, DMA);

	loadSprites();

    help_Screen();

    numCredits = 0;

    int8_t menu = 0;

main_menu:

    resetPlayer( 1, 1 );
    resetPlayer( 0, 1 );

    int8_t ret = 0;

    if ( !numCredits ) {
        do {
		    playersInGame = 0;

            _ExitIfKeyPressed = 1;

            readInput();

            switch ( menu ) {
                case 0:
                    if ( !( ret = scoreTable_Screen( 0 ) ) ) menu++;
                    break;

                case 1:
				    ClearMenu();
                    if ( !( ret = playGame( 1 ) ) ) menu++;
                    break;

                case 2:
                    if ( !( ret = inserCoin_Screen( 0 ) ) ) menu++;
                    break;

                case 3:
                    if ( !( ret = animateAlienCY() ) ) menu++;
                    break;

                case 4:
            	    ClearMenu();
                    if ( !( ret = playGame( 1 ) ) )
                		menu++;
                    break;

                case 5:
                    if ( !( ret = inserCoin_ScreenC() ) ) menu = 0;
                    break;

            }
            
            if ( !ret ) ret = DelayFrames( 100 ); // 2 seconds

            _ExitIfKeyPressed = 0;
            if ( syskey.showSetup ) { ret = setup_Screen(); syskey.keyPressed = 0; }
            if ( syskey.showHelp  ) { ret = help_Screen();  syskey.keyPressed = 0; }

        SPR_update();
        SYS_doVBlankProcess();

        } while ( !ret && !numCredits );
    }

    playersInGame = 0;
    syskey.keyPressed = 0;
    pushPlayerButton_Screen();
    do {
        readInput();
        if ( syskey.showSetup ) { ret = setup_Screen(); syskey.keyPressed = 0; syskey.keyCredit = 1; }
        if ( syskey.showHelp  ) { ret = help_Screen();  syskey.keyPressed = 0; syskey.keyCredit = 1; }
        if ( syskey.keyCredit ) pushPlayerButton_Screen();

        SPR_update();
        SYS_doVBlankProcess();

    } while ( !syskey.keyStart1UP && !syskey.keyStart2UP );

    playersInGame = syskey.keyStart2UP ? 2 : 1;

    numCredits -= playersInGame;

    currentMenuScreen = 0;

    ret = playGame( 0 );

    playersInGame = 0;
    syskey.keyPressed = 0;

    if ( menu < 2 ) menu = 2;
    else menu = 5;

    goto main_menu;

    return 0;
}