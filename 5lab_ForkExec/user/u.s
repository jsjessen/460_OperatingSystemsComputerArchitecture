        .globl _main0,_exit     ! IMPORT
        .globl _getcs,_syscall  ! EXPORT

        call _main0
	
! if ever return, exit(0)
	    push  #0            ! push exitValue 0
        call  _exit

_getcs:
        mov   ax, cs        ! return CS segment value
        ret

! int syscall(a,b,c,d) int.c
_syscall:
        int   80
        ret
