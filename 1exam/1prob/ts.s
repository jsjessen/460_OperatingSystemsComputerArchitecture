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


!==============================================================

KSP = 2

_tswitch:                            ! _tswitch:
SAVE:	                             ! SAVE:	
 	    push bp                      ! 	    push ax
 	    pushf                        ! 	    push bx
 	    push ds                      ! 	    push cx
 	    push ss                      ! 	    push dx
 	    push es                      ! 	    push bp
                                     ! 	    push si
                                     ! 	    push di
  	                                 ! 	    pushf
  	    mov  bx,_running             ! 	    mov  bx,_running
  	    mov  2[bx],sp                ! 	    mov  2[bx],sp
                                     ! 
FIND:	call _scheduler              ! FIND: call _scheduler
                        
         	           
! *** Exam (1) ***
_resume:                             ! _resume: 
RESUME:                              ! RESUME:
        mov  bx,_running             ! 	    mov  bx,_running
        mov  sp,KSP[bx]              ! 	    mov  sp,2[bx]
        pop  es                      ! 	    popf
        pop  ss                      ! 	    pop  di
        pop  ds                      ! 	    pop  si
        popf                         ! 	    pop  bp
        pop  bp                      ! 	    pop  dx
                                     ! 	    pop  cx
        mov ax,2[bp]                 ! 	    pop  bx
        mov bx,4[bp]                 ! 	    pop  ax
        mov cx,6[bp]                 ! 
        mov dx,8[bp]                 !      ret
       !mov bp,10[bp]
        mov si,12[bp]
        mov di,14[bp]
