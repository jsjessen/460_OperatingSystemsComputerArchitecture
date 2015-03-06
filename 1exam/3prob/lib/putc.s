        .globl _color  ! IMPORT
        .globl _putc   ! EXPORT

        !----------------------------------------------
        ! char putc(char c)  function: print a char
        !----------------------------------------------
_putc:           
        push  bp
        mov   bp,sp
    
        movb  al,4[bp]        ! get the char into aL
        movb  ah,#14          ! aH = 14
        mov   bx,_color       ! bL = color
        int   0x10            ! call BIOS to display the char

        pop   bp
        ret
