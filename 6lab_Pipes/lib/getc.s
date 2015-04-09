        .globl _getc  ! EXPORT

        !---------------------------------------------
        !  char getc()       function: return a char
        !---------------------------------------------
_getc:
        xorb ah,ah           ! clear ah
        int  0x16            ! call BIOS to get a char in AX
        ret 
