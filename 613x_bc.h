/* ----------------------------------------------------------------------------
 *                            HOLT Integrated Circuits 
 * ----------------------------------------------------------------------------
 *
 *    file	613x_bc.h
 *    brief     This file contains the prototype functions and
 * 		definitions used by routines in 613x_bc.c file. 
 *
 *		IMPORTANT: Edit file 613x_initialization.h to define either:
 *
 *
 *		Most functional procedures in this project are written for
 *		both parallel bus or SPI host interface schemes, this runtime 
 *		switch determines which are procedures are compiled.
 * 
 *
 *	   	HOLT DISCLAIMER
 *      	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
 *      	KIND, EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 *      	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 *      	PURPOSE AND NONINFRINGEMENT. 
 *      	IN NO EVENT SHALL HOLT, INC BE LIABLE FOR ANY CLAIM, DAMAGES
 *      	OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 *      	OTHERWISE,ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 *      	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 *
 *              Copyright (C) 2009-2011 by  HOLT, Inc.
 *              All Rights Reserved
 */


//------------------------------------------------------------------------------
//                       Macro Definitions
//------------------------------------------------------------------------------

//      Macros for HI-613x Bus Controller Instruction List
//      BC Control Word 
//
#define TXTTMC17	1<<15	// only applies for mode code 17: transmit BC time tag count
#define MEMASK		1<<14	// if bit 0 = 0, Status Set occurs for RT Status Word Msg Error bit
#define SRQMASK		1<<13	// if bit 0 = 0, Status Set occurs for RT Status Word Svc Request bit
#define BSYMASK		1<<12	// if bit 0 = 0, Status Set occurs for RT Status Word Busy bit
#define SSFMASK		1<<11	// if bit 0 = 0, Status Set occurs for RT Status Word Subsystem Fail bit
#define TFMASK		1<<10	// if bit 0 = 0, Status Set occurs for RT Status Word Terminal Flag bit
#define RSVMASK		1<<9	// if bit 0 = 0, Status Set occurs for any RT Status Word Reserved bit 7-5
#define RTRYENA		1<<8	// if retry enabled in BC Config reg, retry occurs for Status Set
#define USEBUSA		1<<7	// if bit = 1 then use Bus A, if bit = 0 then use Bus B
#define USEBUSB		0<<7
#define SFTEST		1<<6	// if bit = 1 then use offline self-test
#define MSKBCR		1<<5	// if BCRME = 1 in BC Config, this bit INVERTED reflects expected BCR status, 
				//				mismatch when BCR = 1 causes status set
				// if BCRME = 0 in BC Config, this bit reflects expected BCR status, 
				//				mismatch = status set
#define EOMINT		1<<4	// if BCEOM interrupt is enabled, this bit causes message EOM interrupt
// bit 3 reserved 					
#define MCODE		1<<2	// select mode code message format
#define BCST		1<<1	// select broadcast message format
#define RT_RT		1<<0	// select RT-to-RT message format


//      Macros for HI-613x Bus Controller Instruction List
//      Command Word entries
//
#define RX 		0
#define TX		1<<10


//	Macros for HI-613x Bus Controller Instruction List
//      BC Op Codes 
//
#define XEQ	0x01<<10
#define XQG	0x16<<10
#define JMP	0x02<<10
#define CAL	0x03<<10
#define IRQ	0x06<<10
#define RTN	0x04<<10
#define HLT	0x07<<10
#define DLY	0x08<<10
#define WFT	0x09<<10
#define CFT	0x0A<<10
#define CMT	0x0B<<10
#define LTT	0x0D<<10
#define LTH	0x18<<10
#define LFT	0x0E<<10
#define SFT	0x0F<<10
#define PTT	0x10<<10
#define PTH	0x19<<10
#define PTB	0x1A<<10
#define PBS	0x11<<10
#define PSI	0x12<<10
#define PSM	0x13<<10
#define WTG	0x14<<10
#define XQF	0x15<<10
#define XFG	0x17<<10
#define WMP	0x1B<<10
#define WMI	0x1C<<10
#define DSZ	0x1D<<10
#define FLG	0x0C<<10


//	Macros for HI-613x Bus Controller Instruction List
//      BC Condition Codes 
//
#define LT        0
#define GP0       0
#define GTorEQ    16
#define nGP0      16
#define EQ        1
#define GP1       1
#define nEQ       17
#define nGP1      17
#define GP2       2
#define nGP2      18
#define GP3       3
#define nGP3      19
#define GP4       4
#define nGP4      20
#define GP5       5
#define nGP5      21
#define GP6       6
#define nGP6      22
#define GP7       7
#define nGP7      23
#define nRESP     8
#define RESP      24
#define FMTERR    9
#define nFMTERR   25
#define GBLOCK    10
#define nGBLOCK   26
#define MASKSTAT  11
#define nMASKSTAT 27
#define BADMSG    12
#define GOODMSG   28
#define x1RETRY   13
#define n1RETRY   29
#define x2RETRY   14
#define ALWAYS    15
#define NEVER     31


// op code word validation field with odd parity bit = 0 
#define VP0                 0x0140

// op code word validation field with odd parity bit = 1 
#define VP1                 0x8140

//  message block addresses, non-RT-to-RT msgs have 8 words 
#define MSG_BLK1_ADDR	    0x3E00
#define MSG_BLK2_ADDR	    0x3E08
#define MSG_BLK3_ADDR	    0x3E10
#define MSG_BLK4_ADDR	    0x3E18
#define MSG_BLK5_ADDR 	    0x3E20
#define MSG_BLK6_ADDR	    0x3E28
#define MSG_BLK7_ADDR	    0x3E30
#define MSG_BLK8_ADDR	    0x3E38

//  message block addresses, RT-to-RT msgs have 16 words 
#define RTRT_MSG_BLK1_ADDR  0x3E40
#define RTRT_MSG_BLK2_ADDR  0x3E50  // thru 0x3E5F

#define BC_ILIST_BASE_ADDR 0x1B70 // thru 0x1BFF allocated, 90 words total, RELOCATABLE
                                  // starting RAM address for BC instruction list. Initialization
                                  // should copy this value into the BC Instruction List Start Addr
                                  // register 0x0033. No need to copy to pointer reg 0x0034, read-only.


//----------------------------------------------------------------------





//------------------------------------------------------------------------------
//      Global Function Prototypes
//------------------------------------------------------------------------------

void BC_bus_addressing_examples(void);

// Function call initializes Bus Controller Message Blocks
//
void initialize_bc_msg_blocks(void);
//void initialize_bc_msg_blocks2(void);


// Function call initializes Bus Controller Instruction List
//
void initialize_bc_instruction_list(void);


// This function disables the Holt HI-613x BC by writing 
// the Master Configuration Register to reset the BCENA bit.
//
void bc_disable(void);


// If the BCENA pin is high, this function enables the Holt 
// HI-613x BC by writing the Master Configuration Register 
// to set the BCENA bit. Then BC operation only begins after
// the BCSTART bit or BCTRIG pin gets a rising edge.
//
void bc_enable(void);


// If the BCENA pin is high, this function enables and starts 
// the Holt HI-613x BC by writing the Master Configuration 
// Register to set the BCENA and BCSTART bits.
//
void bc_start(void);


// This function generates a BC Trigger pulse for the HI-613x.
//
void bc_trigger(void);


// part of infinite standby loop when CONSOLE_IO is false.
// this function permits user testing of BC mode only...
//
void bc_switch_tests(void);


// This function generates a BC Trigger pulse for the HI-613x
// if evauation board button SW1 is pressed
//
char SW1_BC_Trigger(void);

  
// For some BC tests, this function is called from main() standby loop 
// when user presses button SW2 to modify GP flag bits in Cond Code reg.
// primary purpose: testing the condition codes GP0-GP7 and nGP0-nGP7
//
void SW2_BCtest (void);

// This function initializes the Holt HI-613x BC by writing 
// configuration registers in the device. Only BC mode option 
// bits are affected. The program has already called function 
// initialize_613x_shared() to initialize the common parameters
// shared by BC, RT1, RT2 and/or Bus Monitor
//
void initialize_613x_BC(void);



// End of File 

