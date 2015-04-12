// Inter-segment copy functions

#define MTXSEG 0x1000

#define KB        1024
#define REG_SIZE  2
#define WORD_SIZE 2

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

int copy_image(int source_segment, int dest_segment)
{
    // A u16 can store values from 0 to 65,535 
    // because it can store 2^16 different values
    // so 0 to (2^16)-1
    u16 i;
    u16 word;

    // A segment is 64kb
    for(i = 0; i < ((64 / WORD_SIZE) * KB) - 1; i++)
    {
        // Consider assuming get/put word is not so simple
        // try using only get/put byte because it was made by KC
        word = get_word(source_segment, i * WORD_SIZE);
        put_word(word, dest_segment, i * WORD_SIZE);
    }
    return dest_segment;
}
