!================================== LEGEND ================================== 
!
!   All registers are 16-bit
!
!   IP = Instruction Pointer
!   PC = Program Counter --> next intstruction in memory
!   SR = Status Register = mode, interruptMask, conditionCode; mode=System/User
!   SP = Stack Pointer --> current stack top
!   AX = Return Value
!
!   FLAG = Status Register
!   
!   General Registers: AX  BX  CX  DX  BP  SI  DI

!   Segment Registers
!   -----------------
!   CS -> Code  Segment = program code or instructions
!   DS -> Data  Segment = static and global data (ONE COPY only)
!   SS -> Stack Segment = stack area for calling and local variables. 
!   ES -> Extra Segment = temp area; may be used for malloc()/mfree()


!=================================== ts.s =================================== 

        OSSEG  = 0x1000

        .globl _main,_running,_scheduler,_proc,_procSize,_color  ! IMPORT
        .globl _getc,_putc,_tswitch,_resume,_get_esp,_get_ebp    ! EXPORT

        jmpi   start,OSSEG              ! CS=OSSEG, IP=start

start:
        mov     ax,cs                   ! establish segments 
        mov     ds,ax                   ! Let DS,SS,ES = CS=0x1000.
        mov     ss,ax                   
        mov     es,ax
        
        mov     sp,#_proc               ! sp -> proc[0]
        add     sp,_procSize            ! sp -> proc[0]'s HIGH END
        
        call _main                      ! call main() in C

dead:	jmp dead                        ! loop if main() ever returns


_tswitch:
SAVE:	
        push ax
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        pushf
        mov   bx, _running
        mov   2[bx], sp

FIND:	call _scheduler

_resume:	
RESUME:	
        mov   bx, _running
        mov   sp, 2[bx]
        popf
        pop  di
        pop  si
        pop  bp
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret


        !---------------------------------------------
        !  char getc()       function: return a char
        !---------------------------------------------
_getc:
        xorb ah,ah           ! clear ah
        int  0x16            ! call BIOS to get a char in AX
        ret 

        !----------------------------------------------
        ! char putc(char c)  function: print a char
        !----------------------------------------------
_putc:           
        push  bp
        mov   bp,sp
    
        movb  al,4[bp]        ! get the char into aL
        movb  ah,#14          ! aH = 14
        mov   bx,_color       ! bL = color
        !movb  bl,#0x0D        ! bL = cyan color 
        int   0x10            ! call BIOS to display the char

        pop   bp
        ret


       !----------------------------------------------
       ! void* putc()  function: get stack pointer 
       !----------------------------------------------
_get_esp:		
        mov  sp,ax
	    ret

       !----------------------------------------------
       ! void* get_ebp()  function: get stack frame pointer 
       !----------------------------------------------
_get_ebp:		
        mov  bp,ax
	    ret
