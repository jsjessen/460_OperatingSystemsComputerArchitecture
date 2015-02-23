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

        MTXSEG  = 0x1000

auto_start:
!export auto_start
	
       .globl _main,_running,_scheduler,_proc,_procSize  ! IMPORT
       .globl _tswitch                                   ! EXPORT
	
        jmpi   start,MTXSEG

start:	
        mov     ax,cs            ! establish segments 
        mov     ds,ax            ! Let DS,SS,ES = CS=0x1000.
        mov     ss,ax            
        mov     es,ax
        
        mov     sp,#_proc        ! sp -> proc[0]
        add     sp,_procSize     ! sp -> proc[0]'s HIGH END
        
        call _main               ! call main() in C

dead:	jmp dead                 ! loop if main() ever returns


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
	mov  bx,_running
	mov  2[bx],sp

FIND:	call _scheduler

RESUME:
	mov  bx,_running
	mov  sp,2[bx]
	popf
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax

	ret

	
! added functions for KUMODE
	.globl _int80h,_goUmode,_kcinth
!These offsets are defined in struct proc
USS =   4
USP =   6

_int80h:
        push ax                 ! save all Umode registers in ustack
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        push es
        push ds

! ustack contains : flag,uCS,uPC, ax,bx,cx,dx,bp,si,di,ues,uds
        push cs
        pop  ds                 ! KDS now

	    mov bx,_running  	    ! ready to access proc
        mov USS[bx],ss          ! save uSS  in proc.USS
        mov USP[bx],sp          ! save uSP  in proc.USP

! Change ES,SS to kernel segment
        mov  ax,ds              ! stupid !!        
        mov  es,ax              ! CS=DS=SS=ES in Kmode
        mov  ss,ax

! set sp to HI end of running's kstack[]
	    mov  sp,_running        ! proc's kstack [2 KB]
        add  sp,_procSize       ! HI end of PROC

        call  _kcinth
        jmp   _goUmode
  
_goUmode:
        cli
	    mov bx,_running 	    ! bx -> proc
        mov ax,USS[bx]
        mov ss,ax               ! restore uSS
        mov sp,USP[bx]          ! restore uSP
  
	    pop ds
	    pop es
	    pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax                  ! NOTE: contains return value to Umode     
	
        iret    ! ret pop stack top and puts in PC register, iret is interupt return (pops 3 things as 1)
                ! uPC uCS flag all popped into CPU as one unit (CPU always obeys PC and CS)
                ! 0 location is where code was moved to

	
