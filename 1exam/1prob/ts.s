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

_tswitch:                      ! _tswitch:
SAVE:	                       ! SAVE:	!Note: rPC has been pushed
 	    push bp                ! 	    push ax
 	    pushf                  ! 	    push bx
 	    push ds                ! 	    push cx
 	    push ss                ! 	    push dx
 	    push es                ! 	    push bp
                               ! 	    push si
                               ! 	    push di
  	                           ! 	    pushf
  	    mov  bx,_running       ! 	    mov  bx,_running    ! bx -> proc
  	    mov  2[bx],sp          ! 	    mov  2[bx],sp       ! save sp to proc.ksp
                               ! 
FIND:	call _scheduler        ! FIND: call _scheduler      ! call scheduler() in C
         	           
!    ksp
!  ---|------------------------------
!  | es | ss | ds | flag | bp | rPC |
!  ------------------------------|---
!    -6   -5   -4    -3    -2  kstack[SSIZE-1]

! *** Exam (1) ***
_resume:                       ! _resume: 
RESUME:                        ! RESUME:
        mov  bx,_running       ! 	    mov  bx,_running    ! bx -> running
        mov  sp,2[bx]          ! 	    mov  sp,2[bx]       ! sp = proc.ksp
        pop  es                ! 	    popf
        pop  ss                ! 	    pop  di
        pop  ds                ! 	    pop  si
        popf                   ! 	    pop  bp
        pop  bp                ! 	    pop  dx
                               ! 	    pop  cx                                   
        mov ax,0               ! 	    pop  bx                                   
        mov bx,0               ! 	    pop  ax                                   
        mov cx,0               !                                                   
        mov dx,0               !        ret                 ! now sp points at rPC 
        mov si,0    
        mov di,0
        
        ret
