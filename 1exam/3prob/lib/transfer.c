// Inter-segment copy functions

#define MTXSEG 0x1000

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

// Get a byte from Umode
u8 get_byte(u16 segment, u16 offset)
{
  u8 byte;
  setds(segment);
  byte = *(u8*)offset;
  setds(MTXSEG);
  return byte;
}

// Get a word from Umode
u16 get_word(u16 segment, u16 offset)
{
  u16 byte;
  setds(segment);
  byte = *(u16*)offset;
  setds(MTXSEG);
  return byte;
}

// Put a byte into Umode
void put_byte(u8 byte, u16 segment, u16 offset)
{
  setds(segment);
  *(u8*)offset = byte;
  setds(MTXSEG);
}

// Put a word into Umode
void put_word(u16 word, u16 segment, u16 offset)
{
  setds(segment);
  *(u16*)offset = word;
  setds(MTXSEG);
}
