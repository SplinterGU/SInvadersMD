#ifndef __SOUND_H
#define __SOUND_H

#define SOUND_UFO               0
#define SOUND_PLAYER_SHOT       1
#define SOUND_PLAYER_EXPLOSION  2
#define SOUND_ALIEN_EXPLOSION   3
#define SOUND_ALIEN_STEP1       4
#define SOUND_ALIEN_STEP2       5
#define SOUND_ALIEN_STEP3       6
#define SOUND_ALIEN_STEP4       7
#define SOUND_UFO_EXPLOSION     8
#define SOUND_EXTRA_LIFE        9


extern int SoundPlay( int id, int loop );
extern void SoundStop( int id );
extern int SoundIsPlaying( int id );

#endif
