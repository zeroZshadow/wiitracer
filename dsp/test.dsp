/********************************/
/**      REGISTER NAMES        **/
/********************************/

AR0:	equ	0x00	; address registers
AR1:	equ	0x01
AR2:	equ	0x02
AR3:	equ	0x03	// used as jump function selector

IX0:	equ	0x04	// LEFT_VOLUME accel
IX1:	equ	0x05	// RIGHT_VOLUME accel
IX2:	equ	0x06	// ADDRH_SMP accel
IX3:	equ	0x07	// ADDRL_SMP accel

R08:	equ	0x08	// fixed to 48000 value
R09:	equ	0x09	// problems using this
R0A:	equ	0x0a	// ADDREH_SMP accel
R0B:	equ	0x0b	// ADDREL_SMP accel

ST0:	equ	0x0c
ST1:	equ	0x0d
ST2:	equ	0x0e
ST3:	equ	0x0f

CONFIG:	equ	0x12
SR:	equ	0x13

PRODL: equ	0x14
PRODM: equ	0x15
PRODH: equ	0x16
PRODM2: equ	0x17 

AXL0:  equ	0x18
AXL1:  equ	0x19
AXH0:  equ	0x1A	// SMP_R accel
AXH1:  equ	0x1b	// SMP_L accel

ACC0:  equ	0x1c	// accumulator (global)
ACC1:  equ	0x1d

ACL0:  equ	0x1c	// Low accumulator
ACL1:  equ	0x1d
ACM0:  equ	0x1e	// Mid accumulator
ACM1:  equ	0x1f
ACH0:  equ	0x10	// Sign extended 8 bit register 0
ACH1:  equ	0x11	// Sign extended 8 bit register 1

/********************************/
/**  HARDWARE REGISTER ADDRESS **/
/********************************/

DSCR:	equ	0xffc9	; DSP DMA Control Reg
DSBL:	equ	0xffcb	; DSP DMA Block Length
DSPA:	equ	0xffcd	; DSP DMA DMEM Address
DSMAH:	equ	0xffce	; DSP DMA Mem Address H
DSMAL:	equ	0xffcf	; DSP DMA Mem Address L

DIRQ:	equ	0xfffb	; DSP Irq Request
DMBH:	equ	0xfffc	; DSP Mailbox H
DMBL:	equ	0xfffd	; DSP Mailbox L
CMBH:	equ	0xfffe	; CPU Mailbox H
CMBL:	equ	0xffff	; CPU Mailbox L

DMA_TO_DSP:	equ	0
DMA_TO_CPU:	equ	1

/***  START CODE ***/

/**************************************************************/
/*			EXCEPTION TABLE			      */
/**************************************************************/

	jmp	exception0
	jmp	exception1
	jmp	exception2
	jmp	exception3
	jmp	exception4
	jmp	exception5
	jmp	exception6
	jmp	exception7

	lri $CONFIG, #0xff
	lri	$SR,#0
	s40         
	clr15       
	m0         

/**************************************************************/
/*                            main                            */
/**************************************************************/

main:
	
	ret

// exception table

exception0:	// RESET
	rti

exception1:	// STACK OVERFLOW
	rti

exception2:
	rti
	
exception3:
	rti

exception4:
	rti

exception5:	// ACCELERATOR ADDRESS OVERFLOW
	rti
	
exception6:
	rti

exception7:
	rti



