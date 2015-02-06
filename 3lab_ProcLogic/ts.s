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

        .globl _main,_running,_scheduler,_proc,_procSize  ! IMPORT
        .globl _tswitch,_resume                           ! EXPORT


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
