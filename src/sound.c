#include <genesis.h>

#include <sound.h>
#include <sfx.h>

struct {
    void * data;
    uint32_t len;
} sound_table[10] = { 
    { sound_ufo              , sizeof( sound_ufo                 ) },
    { sound_player_shot      , sizeof( sound_player_shot         ) },
    { sound_player_explosion , sizeof( sound_player_explosion    ) },
    { sound_alien_explosion  , sizeof( sound_alien_explosion     ) },
    { sound_alien_step1      , sizeof( sound_alien_step1         ) },
    { sound_alien_step2      , sizeof( sound_alien_step2         ) },
    { sound_alien_step3      , sizeof( sound_alien_step3         ) },
    { sound_alien_step4      , sizeof( sound_alien_step4         ) },
    { sound_ufo_explosion    , sizeof( sound_ufo_explosion       ) },
    { sound_extra_life       , sizeof( sound_extra_life          ) }
};

#define MAX_CHANNELS    4

int8_t channels[] = { -1, -1, -1, -1 };

static int8_t play_sound(const uint8_t *sample, const uint32_t len, const uint16_t channel, const uint8_t loop)
{
    vu8 *pb;
    uint8_t status;
    uint16_t ch;
    uint32_t addr;

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_4PCM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 status
    pb = (uint8_t *) Z80_DRV_STATUS;
    // get status
    status = *pb;

    // auto channel ?
    if (channel == SOUND_PCM_CH_AUTO)
    {
        // scan for first free channel
        ch = 0;

        while ((ch < MAX_CHANNELS) && (status & (Z80_DRV_STAT_PLAYING << ch))) ch++;

        // if all channel busy, no sound
        if (ch == MAX_CHANNELS) {
            Z80_releaseBus();
            return -1;
        }
    }
    // we use specified channel
    else ch = channel;

    // point to Z80 base parameters
    pb = (uint8_t *) (Z80_DRV_PARAMS + (ch * 4));

    addr = (uint32_t) sample;

#if 0
    // sample address (128 bytes aligned)
    pb[0] = addr >> 7;
    pb[1] = addr >> 15;
    // sample length (128 bytes aligned)
    pb[2] = len >> 7;
    pb[3] = len >> 15;
#endif
    pb[0] = addr >> 8;
    pb[1] = addr >> 16;
    // sample length (256 bytes aligned)
    pb[2] = len >> 8;
    pb[3] = len >> 16;


    // point to Z80 command
    pb = (uint8_t *) Z80_DRV_COMMAND;
    // play command
    *pb |= (Z80_DRV_COM_PLAY << ch);

    // point to Z80 status
    pb = (uint8_t *) Z80_DRV_STATUS;

    // loop flag in status
    if (loop) pb[1] |= (Z80_DRV_STAT_PLAYING << ch);
    else pb[1] &= ~(Z80_DRV_STAT_PLAYING << ch);

    Z80_releaseBus();

            // set volume values for driver 4PCM_ENV
    SND_setVolume_4PCM_ENV(SOUND_PCM_CH1, 15);
    SND_setVolume_4PCM_ENV(SOUND_PCM_CH2, 15);
    SND_setVolume_4PCM_ENV(SOUND_PCM_CH3, 15);
    SND_setVolume_4PCM_ENV(SOUND_PCM_CH4, 15);

    return ch;
}

int SoundPlay( int id, int loop ) {
    int n, chav = -1, ch;
    
    if ( !sound_table[ id ].data ) return -1;

    for ( n = 0; n < MAX_CHANNELS; n++ ) {
        if ( !SND_isPlaying_4PCM( Z80_DRV_STAT_PLAYING << n ) ) channels[ n ] = -1;
        if ( channels[ n ] == id ) return n;
        if ( chav == -1 && channels[ n ] == -1 ) chav = n;
    }

    if ( chav != -1 ) {
        if ( ( ch = play_sound( sound_table[ id ].data, sound_table[ id ].len, chav, loop ) ) != -1 ) channels[ ch ] = id;
    }
    return ch;
}

void SoundStop( int id ) {
    int n;
    for ( n = 0; n < MAX_CHANNELS; n++ ) {
        if ( id == -1 || channels[ n ] == id ) {
            SND_stopPlay_4PCM( n );
            channels[ n ] = -1;
        }
    }    
}

int SoundIsPlaying( int id ) {
    int n;
    for ( n = 0; n < MAX_CHANNELS; n++ ) {
        if ( channels[ n ] == id ) {
            int ret = SND_isPlaying_4PCM( Z80_DRV_STAT_PLAYING << n );
            if ( !ret ) channels[ n ] = -1;
            return ret;
        }
    }    
    return 0;
}
