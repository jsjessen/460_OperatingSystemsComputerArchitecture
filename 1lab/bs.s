!=================================== LOGIC =================================== 
!
!  (1). It is to be combined with a .c source file to form a booter code 
!       which occupies the boot BLOCK (block#0) of a floppy disk.
!
!  (2). During booting, BIOS loads 512 bytes of this boot BLOCK to
!       (0x0000,0x7C00) and jumps to it. Although only half in, it can start
!       execution because it does not need any portion that's not yet loaded.
!
!  (3). It sets ES=0x9000, BX=0 and calls BIOS to load the entire boot
!       BLOCK to 0x9000.
!
!  (4). Then it jumps to the symbol start relative to 0x9000, and 
!       continues to execute from there. This jump sets CS to 0x9000. 
!
!  (5). It sets DS,SS to CS, and SP to 8KB above SS. Note that ES is already 
!       set to 0x9000 in (3). Thus, CS,DS,SS and ES all point at the same 
!       segment address 0x9000.
!
!  (6). It then calls YOUR  main() in C, which is specified in 3 below.
!
!  (7). Upon return from C code, bs.s jumps to (0x1000, 0) to start up MTX.
!       In your C code, call error() if any error.


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


!=================================== bs.s =================================== 
    
       BOOTSEG =  0x9000        ! Boot Block is loaded again to here.
       SSP      =   8192        ! Stack pointer at SS+8KB

       .globl _main,_prints                            ! IMPORT symbols
       .globl _getc,_putc,_readfd,_setes,_inces,_error ! EXPORT symbols
                                                
       !-----------------------------------------------------------
       ! Only one SECTOR loaded at (0000,7C00). Get entire BLOCK in
       !-----------------------------------------------------------
       mov  ax,#BOOTSEG    ! set ES to 0x9000
       mov  es,ax
       xor  bx,bx          ! clear BX = 0
       !---------------------------------------------------
       ! call BIOS-INT13 to read 1KB BLOCK to [0x9000,0]     
       !---------------------------------------------------
       xor  dx,dx          ! drive 0, head 0
       xor  cx,cx
       incb cl             ! cyl 0, sector 1
       mov  ax, #0x0202    ! READ 1 block
       int  0x13

       jmpi start,BOOTSEG           ! CS=BOOTSEG, IP=start

start:                    
       mov  ax,cs                   ! establish segments again
       mov  ds,ax                   ! we know ES,CS=0x8F00. Let DS=CS  
       mov  ss,ax                   ! SS = CS ===> all point at 0x8F00
       mov  es,ax
       mov  sp,#SSP                 ! SP = 8KB above 0x90000

       !--------------- OPTIONAL --------------------------------------
       mov  ax,#0x0012     ! Call BIOS for 640x480 color mode     
       int  0x10           ! 
       !--------------------------------------------------------------	
    
       call _main                      ! call main() in C

       jmpi 0,0x1000
 

!=============================== I/O functions ============================== 

       !---------------------------------------------
       !  char getc()   function: returns a char
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
       movb  bl,#0x0D        ! bL = cyan color 
       int   0x10            ! call BIOS to display the char

       pop   bp
       ret

       !---------------------------------------
       ! readfd(cyl, head, sector, buf)
       !        4     6     8      10
       !---------------------------------------
_readfd:                             
       push  bp
       mov   bp,sp            ! bp = stack frame pointer

       movb  dl, #0x00        ! drive 0=FD0
       movb  dh, 6[bp]        ! head
       movb  cl, 8[bp]        ! sector
       incb  cl
       movb  ch, 4[bp]        ! cyl
       mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
       mov   ax, #0x0202      ! READ 2 sectors to (EX, BX)

       int  0x13              ! call BIOS to read the block 
       jb   _error            ! to error if CarryBit is on [read failed]

       pop  bp                
       ret
    
! void set_es(unsigned short segment)  set ES register to segment
_setes:  
        push  bp
        mov   bp,sp
        mov   ax,4[bp]        
        mov   es,ax
        pop   bp
    ret
       
! void inces() inc ES by 0x40, or 1KB
_inces:                         
        mov   ax,es
        add   ax,#0x40
        mov   es,ax
        ret

        !------------------------------
        !       error & reboot
        !------------------------------
_error:
        mov  bx, #bad
        push bx
        call _prints
        
        int  0x19                       ! reboot

bad:    .asciz  "Error!"

! end of bs.s file
