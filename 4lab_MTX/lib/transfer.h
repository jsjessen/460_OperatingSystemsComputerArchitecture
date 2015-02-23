// James Jessen
// 10918967

#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include "../type.h"

// Get a from Umode
u8 get_byte(u16 segment, u16 offset);
u16 get_word(u16 segment, u16 offset);

// Put a into Umode
void put_byte(u8 byte, u16 segment, u16 offset);
void put_word(u16 word, u16 segment, u16 offset);


#endif
