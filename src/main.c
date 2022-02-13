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

#define COLOR_WHITE     VDPPALETTE_REDMASK|VDPPALETTE_GREENMASK|VDPPALETTE_BLUEMASK
#define COLOR_RED       VDPPALETTE_REDMASK
#define COLOR_GREEN     VDPPALETTE_GREENMASK
#define COLOR_BLUE      VDPPALETTE_BLUEMASK
#define COLOR_YELLOW    VDPPALETTE_REDMASK|VDPPALETTE_GREENMASK
#define COLOR_CYAN      VDPPALETTE_GREENMASK|VDPPALETTE_BLUEMASK

u16 custom_palette1[16] = {                            
    0, COLOR_WHITE, COLOR_GREEN, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_GREEN,    COLOR_RED, COLOR_YELLOW,      0x282, 0xe0a, 0x80e, COLOR_BLUE,      0xee0, 0x600, 0xe60
} ;

u16 custom_palette2[16] = { 
    0,   COLOR_RED, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_GREEN, COLOR_GREEN, COLOR_YELLOW,   COLOR_BLUE,      0xee0, 0x00c,     0,          0,          0,     0,     0
} ;

u16 custom_palette3[16] = { 
    0, COLOR_GREEN,           0, COLOR_WHITE, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN,            0,            0,          0,     0, 0x444,      0x666,      0x999, 0xccc,     0
} ;

int main()
{
//    memset( player, '\0', sizeof( player ) );

    //disable interrup
    SYS_disableInts();

//	load_Z80Driver();

    //320x224px
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

	SPR_initEx(184);

   	/* Configuración de diseño de fuente */
    VDP_loadFontData(custom_font.tiles, 59, DMA);

    /* Lectura de datos de la paleta */
    VDP_setPalette( PAL0, custom_palette1 );
    VDP_setPalette( PAL1, custom_palette2 );
    VDP_setPalette( PAL2, custom_palette3 );

    /* Configuración de la paleta de fuentes */
    VDP_setTextPalette(PAL0);

    // VDP process done, we can re enable interrupts
    SYS_enableInts();

    // reserved tiles
    // --------------
    // 6 * 8 = shields
    // 2 = player (lifes)
    // 32 = floor
	VDP_drawImageEx(PLAN_B, &background, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_USERINDEX + 6 * 8 + 2 + 32 ), 0, 0, TRUE, DMA);

	loadSprites();

    enableInput = 0;
    help_Screen();
    enableInput = 1;

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
