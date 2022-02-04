#ifndef _UTILS_H
#define _UTILS_H

//#include <stdint.h>

extern int8_t _ExitIfKeyPressed;

// PrintNumAt Flags
// bits 0-3 digits
// bit 6 fill leftzero
// bit 7 unsigned

#define NUM_UNSIGNED 0x80
#define NUM_LEFTZERO 0x60

extern void PrintNumAt( uint8_t x, uint8_t y, uint16_t n, uint8_t flags );
extern void PrintAt( uint8_t x, uint8_t y, char * msg );
extern int PrintAtDelay( uint8_t x, uint8_t y, char * msg, uint8_t delay );

extern int8_t DelayFrames( uint8_t frames );

extern void PrintChar( uint8_t x, uint8_t y, uint8_t ch, uint8_t mode );

#endif
