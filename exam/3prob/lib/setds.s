        .globl _setds   ! EXPORT

        !---------------------------------------------
        !  void setds(u16 segment)   function: Set data segment 
        !---------------------------------------------
_setds:
	push bp			
	mov  bp,sp		
	mov  ds,4[bp]		! load ds with segment value
	pop  bp
	ret
