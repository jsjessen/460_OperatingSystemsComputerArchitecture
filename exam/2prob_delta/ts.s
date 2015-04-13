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

! *** EXAM (1) ***
! Only difference is pushes in int80h and pops in goUmode
! specifically, cx dx si di are missing
 
!These offsets are defined in struct proc                           ! USS = 4
USS =   4                                                           ! USP = 6
USP =   6                                                           ! 
_int80h:                                                            ! _int80h:
        push ax                 ! save SOME Umode registers in u    !         push ax                 ! save all Umode registers in ustack
        push bx                                                     !         push bx
        push bp                                                     !         push cx
        push es                                                     !         push dx
        push ds                                                     !         push bp
                                                                    !         push si
 ! ustack contains:                                                 !         push di
 !                                                                  !         push es
 !     | by INT 80 => | by _int80h: ====>|                          !         push ds   
 !  ---|----------------------------------                          ! !                      |by INT 80 =>|   by _int80h: ============>|
 !  ???| flag,uCS,uPC | ax,bx,bp,ues,uds |                          ! !                   ---|-----------------------------------------------
 !  ----------------------------------|---                          ! ! ustack contains : ???|flag,uCS,uPC|ax,bx,cx,dx,bp,si,di,ues,uds|
 !                                    |                             ! !                   -------------------------------------------|---- 
 !                                   uSP                            ! !                                                              |
                                                                    !
 ! ustack contains : flag,uCS,uPC, ax,bp,ues,uds                    ! !                                                             uSP
         push cs                                                    !         push cs                 ! push kCS (0x1000) 
         pop  ds                 ! KDS now                          !         pop  ds                 ! let DS=CS = 0x1000 (KDS now)
                                                                    ! 
                                                                    ! ! All variables are now relative to DS=0x1000 of Kernel space
                                                                    ! ! save running proc's Umode uSS and uSP into its PROC 
                                                                    ! 
         mov bx,_running         ! ready to access proc             ! 	      mov bx,_running  	    ! bx->PROC: ready to access running proc
                                                                    ! 
         mov USS[bx],ss          ! save uSS  in proc.USS            !         mov USS[bx],ss          ! save uSS in proc.USS
         mov USP[bx],sp          ! save uSP  in proc.USP            !         mov USP[bx],sp          ! save uSP in proc.USP
                                                                    ! 
 ! Change ES,SS to kernel segment                                   ! ! change ES SS to kernel segment 0x1000 
         mov  ax,ds              ! stupid !!                        !         mov  ax,ds              ! must mov segments this way!
         mov  es,ax              ! CS=DS=SS=ES in Kmode             !         mov  es,ax              ! CS=DS=SS=ES in Kmode
         mov  ss,ax                                                 !         mov  ss,ax              ! SS is now KSS = 0x1000
                                                                    ! 
 ! set sp to HI end of running's kstack[]                           ! ! switch (running proc's) stack from U space to K space.
         mov  sp,_running        ! proc's kstack [2 KB]             ! 	      mov  sp,_running        ! sp points at running proc (proc's kstack [2 KB])
         add  sp,_procSize       ! HI end of PROC                   !         add  sp,_procSize       ! sp -> HIGH END of running's kstack[]
                                                                    ! 
                                                                    ! ! We are now completely in K space, and stack is running proc's (empty) kstack
                                                                    ! 
                                                                    ! ! *************   CALL handler in C ******************************
                                                                    ! 
          call  _kcinth                                             !         call  _kcinth           ! call kcinth() in int.c
                                                                    !                                 ! kc = Kmode code
                                                                    ! 
                                                                    ! ! *************   RETURN TO U Mode ********************************
                                                                    ! 
          jmp   _goUmode                                            !         jmp   _goUmode
                                                                    !   
                                                                    ! !=============================================================================
                                                                    ! ! The assembly function goUmode() restores Umode stack, then restores Umode
                                                                    ! ! registers, followed by an IRET, causing the process to return to Umode.
                                                                    ! !
                                                                    ! ! These assembly functions are keys to understanding Umode/Kmode transitions. 
                                                                    ! !*****************************************************************************
_goUmode:                                                           ! _goUmode:
        cli                                                         !         cli                     ! mask out interrupts
        mov bx,_running         ! bx -> proc                        ! 	      mov bx,_running 	       ! bx -> proc
        mov ax,USS[bx]                                              !         mov ax,USS[bx]
        mov ss,ax               ! restore uSS                       !         mov ss,ax               ! restore uSS
        mov sp,USP[bx]          ! restore uSP                       !         mov sp,USP[bx]          ! restore uSP
                                                                    !   
        pop ds                                                      ! 	      pop ds
        pop es                                                      ! 	      pop es
        pop bp                                                      ! 	      pop di
        pop bx                                                      !         pop si
        pop ax                                                      !         pop bp
                                                                    !         pop dx
        iret                                                        !         pop cx
                                                                    !         pop bx
                                                                    !         pop ax                  ! NOTE: contains return value to Umode     
                                                                    ! 	
                                                                    !         iret    ! ret pop stack top and puts in PC register, iret is interupt return (pops 3 things as 1)
                                                                    !                 ! uPC uCS flag all popped into CPU as one unit (CPU always obeys PC and CS)
                                                                    !                 ! 0 location is where code was moved to

	
