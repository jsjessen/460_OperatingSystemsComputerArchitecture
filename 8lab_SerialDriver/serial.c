#include "kernel.h"

// serial.c file for SERIAL LAB ASSIGNEMNT
/**************** CONSTANTS ***********************/
#define INT_CTL     0x20
#define ENABLE      0x20

#define NULLCHAR      0
#define BEEP          7
#define BACKSPACE     8
#define ESC          27
#define SPACE        32

#define BUFLEN      64
#define NULLCHAR     0

#define NR_STTY      2    /* number of serial ports */

/* offset from serial ports base */
#define DATA         0   /* Data reg for Rx, Tx   */
#define DIVL         0   /* When used as divisor  */
#define DIVH         1   /* to generate baud rate */
#define IER          1   /* Interrupt Enable reg  */
#define IIR          2   /* Interrupt ID rer      */
#define LCR          3   /* Line Control reg      */
#define MCR          4   /* Modem Control reg     */
#define LSR          5   /* Line Status reg       */
#define MSR          6   /* Modem Status reg      */

/**** The serial terminal data structure ****/

struct stty 
{
    /* input buffer */
    char inbuf[BUFLEN];
    int inhead, intail;
    struct semaphore inchars;

    /* output buffer */
    char outbuf[BUFLEN];
    int outhead, outtail;
    struct semaphore outroom;
    int tx_on;

    /* Control section */
    char echo;   /* echo inputs */
    char ison;   /* on or off */
    char erase, kill, intr, quit, x_on, x_off, eof;

    /* I/O port base address */
    int port;
} stty[NR_STTY];


/********  bgetc()/bputc() by polling *********/
int bputc(int port, int c)
{
    // Stupid polling, wastes CPU time: should be interrupt driven instead
    while ((in_byte(port+LSR) & 0x20) == 0);
    out_byte(port+DATA, c);
}

int bgetc(int port)
{
    while ((in_byte(port+LSR) & 0x01) == 0);
    return (in_byte(port+DATA) & 0x7F);
}

int enable_irq(u8 irq_nr)
{
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));
}


/************ serial ports initialization ***************/
char *p = "\n\rSerial Port Ready\n\r\007";

int sinit()
{
    int i;  
    struct stty *t;
    char *q; 

    /* initialize stty[] and serial ports */
    for (i = 0; i < NR_STTY; i++)
    {
        q = p;

        printf("sinit : port #%d\n",i);

        t = &stty[i];

        /* initialize data structures and pointers */
        if (i==0)
            t->port = 0x3F8;    /* COM1 base address */
        else
            t->port = 0x2F8;    /* COM2 base address */  

        // JJ:
        // 0 => COM1 => 4 = 9+4 = 13
        // 1 => COM2 => 3 = 9+3 = 12

        // From instructions:
        //      t->inhead = t->intail = 0;
        //      t->inchars.value = 0;  t->inchars.queue = 0;

        //      t->outhead = t->outtail = t->tx_on = 0;
        //      t->outspace.value = BUFLEN; t->outspace.queue = 0;

        t->inchars.value  = 0;  t->inchars.queue = 0;
        t->inmutex.value  = 1;  t->inmutex.queue = 0;
        t->outmutex.value = 1;  t->outmutex.queue = 0;
        t->outroom.value = OUTBUFLEN; t->outroom.queue = 0;

        t->inhead = t->intail = 0;
        t->outhead =t->outtail = 0;

        t->tx_on = 0;

        // initialize control chars; NOT used in MTX but show how anyway
        t->ison = t->echo = 1;   /* is on and echoing */
        t->erase = '\b';
        t->kill  = '@';
        t->intr  = (char)0177;  /* del */
        t->quit  = (char)034;   /* control-C */
        t->x_on  = (char)021;   /* control-Q */
        t->x_off = (char)023;   /* control-S */
        t->eof   = (char)004;   /* control-D */

        lock();  // CLI; no interrupts

        //out_byte(t->port+MCR,  0x09);  /* IRQ4 on, DTR on */
        out_byte(t->port+IER,  0x00);  /* disable serial port interrupts */

        out_byte(t->port+LCR,  0x80);  /* ready to use 3f9,3f8 as divisor */
        out_byte(t->port+DIVH, 0x00);
        out_byte(t->port+DIVL, 12);    /* divisor = 12 ===> 9600 bauds */

        /******** term 9600 /dev/ttyS0: 8 bits/char, no parity *************/ 
        out_byte(t->port+LCR, 0x03); 

        /*******************************************************************
          Writing to 3fc ModemControl tells modem : DTR, then RTS ==>
          let modem respond as a DCE.  Here we must let the (crossed)
          cable tell the TVI terminal that the "DCE" has DSR and CTS.  
          So we turn the port's DTR and RTS on.
         ********************************************************************/

        out_byte(t->port+MCR, 0x0B);  /* 1011 ==> IRQ4, RTS, DTR on   */
        out_byte(t->port+IER, 0x01);  /* Enable Rx interrupt, Tx off */

        unlock();

        enable_irq(4-i);  // COM1: IRQ4; COM2: IRQ3

        /* show greeting message */
        // USE bputc() to PRINT MESSAGE ON THE SERIAL PORT: serial port # ready
        while(*q)
        {
            bputc(t->port, *q);
            q++;
        }
    }
}  

//======================== LOWER HALF ROUTINES ===============================
int s0handler()
{
    shandler(0);
}
int s1handler()
{
    shandler(1);
}

int shandler(int port)
{  
    struct stty *t;
    int IntID, LineStatus, ModemStatus, intType, c;

    t = &stty[port];            /* IRQ 4 interrupt : COM1 = stty[0] */

    IntID     = in_byte(t->port+IIR);       /* read InterruptID Reg */
    LineStatus= in_byte(t->port+LSR);       /* read LineStatus  Reg */    
    ModemStatus=in_byte(t->port+MSR);       /* read ModemStatus Reg */

                             // JJ: 7 = 0000 0111 
    intType = IntID & 7;     /* mask out all except the lowest 3 bits */
    switch(intType)
    {
        case 6 : do_errors(t);  break;   /* 110 = errors */
        case 4 : do_rx(t);      break;   /* 100 = rx interrupt */
        case 2 : do_tx(t);      break;   /* 010 = tx interrupt */
        case 0 : do_modem(t);   break;   /* 000 = modem interrupt */
    }
    out_byte(INT_CTL, ENABLE);   /* reenable the 8259 controller */ 
}

int do_errors()
{ printf("assume no error\n"); }
// Instructions: { printf("ignore error\n"); }

int do_modem()
{  printf("don't have a modem\n"); }


/* The following show how to enable and disable Tx interrupts */

enable_tx(struct stty *t)
{
    lock();
    out_byte(t->port+IER, 0x03);   /* 0011 ==> both tx and rx on */
    t->tx_on = 1;
    unlock();
}

disable_tx(struct stty *t)
{ 
    lock();
    out_byte(t->port+IER, 0x01);   /* 0001 ==> tx off, rx on */
    t->tx_on = 0;
    unlock();
}

// ============= Input Driver ==========================
int do_rx(struct stty *t)  /* interrupts already disabled */
{ 
    int c;
    c = in_byte(tty->port) & 0x7F;  /* read the char from port */

    printf("\nrx interrupt c="); putc(c);
    //printf("port %x interrupt:c=%c ", tty->port, c,c);

    // Write code to put c into inbuf[ ]; notify process of char available;  
    // use bputc() to ECHO the input char to serial port // JJ: can ignore this 

        // COMPLETE with YOUR C code
        // JJ: read KC's keyboard driver to understand how  
        // if buffer full: do nothing?
        // else put char in buffer: wake up anyone waiting for char
        // set V?
}      

int sgetc(struct stty *tty)
{ 
    int c;
    // write Code to get a char from inbuf[ ]
    //get a char c from inbuf[ ]
    return(c);
    //return char;
}

int sgetline(struct stty *tty, char *line)
{  
    // write code to input a line from tty's inbuf[ ] 
    // WRITE C code to get a line from a serial port tty

    return strlen(line);
}


//****************** Output driver *****************************
int do_tx(struct stty *tty)
{
   // Instructions:
   //       printf("tx interrupt\n");

   //       if outbuf empty { 
   //          turn off tx interrupt; 
   //          return;
   //       }

   //       // output a char from outbuf[ ]; 
   //       // V outspace semaphore

    int c;
    printf("tx interrupt ");
    if (tty->outroom.value == BUFLEN)
    { // nothing to do 
        disable_tx(tty);                 // turn off tx interrupt
        return;
    }

    // write code to output a char from tty's outbuf[ ]
    //       out_byte(tty->port, c); will output c to port
}

//--------------- UPPER half functions -------------------
int sputc(struct stty *tty, int c)
{
    // Instructions:
    //      WAIT FOR space in tty's outbuf[];
    //      enter c into tty's outbuf[ ];
    //      enable tx interrupt if it is off;    

    // write code to put c into tty's outbuf[ ]
    // enable tx_interrupt
}

int sputline(struct stty *tty, char *line)
{
    // write code to output a line to tty
  // WRITE C code to output a line to a serial port
}

// Does this belong in this file?
//**************** Syscalls from Umode ************************
int usgets(int port, char *y)
{  
  // get a line from serial port and write it to y in U space
}

int uputs(int port, char *y)
{
  // output line y in U space to serail port
}
