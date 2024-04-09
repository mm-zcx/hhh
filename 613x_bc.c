/* ----------------------------------------------------------------------------
 *                            HOLT Integrated Circuits 
 * ----------------------------------------------------------------------------
 *
 *    file	613x_bc.c
 *    brief     This file contains initialization functions and arrays for 
 *		initializing BC register and RAM tables inside the Holt  
 *		HI-6130 or HI-6131 device. 
 *
 *		IMPORTANT: Edit file 613x_initialization.h to define either:
 *
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
//         Headers
//------------------------------------------------------------------------------

// standard Atmel/IAR headers
#include <pio/pio.h>
#include <intrinsics.h>
#include <spi/spi.h>

// Holt project headers
#include "613x_initialization.h"
#include "613x_regs.h"
#include "613x_bc.h"
#include "board_613x.h"
#include "board_6131.h"
#include "device_6131.h"

// HI-6131 SPI slave select pin
static const Pin pinNss[]  = {BOARD_6131_NPCS_PIN};



//------------------------------------------------------------------------------
//         Functions
//------------------------------------------------------------------------------
// the initialization functions below do not use the addressing methods shown above.
// instead, they demonstrate alternative addressing methods...


void initialize_bc_instruction_list(void) {
  
	unsigned short i, j, len;
        AT91S_SPI *spi = BOARD_6131_SPI_BASE;
        unsigned short dummy, k;
        volatile unsigned char bufferTX[4];

	unsigned short inst_list[46] = {
	// test op codes WTG,XEQ,JMP, verify various msg block setups
	WTG|ALWAYS, 0x0000,			// wait for ext trigger, addr = BC_ILIST_BASE_ADDR = 0x1B70 
	XEQ|ALWAYS, MSG_BLK1_ADDR,		// 1
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK2_ADDR,		// 2
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK3_ADDR,		// 3
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK4_ADDR,		// 4
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK5_ADDR,		// 5
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK6_ADDR,		// 6
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK7_ADDR,		// 7
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK8_ADDR,		// 8 
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, RTRT_MSG_BLK1_ADDR,		// RT-RT 1
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, RTRT_MSG_BLK2_ADDR,		// RT-RT 2
	WTG|ALWAYS, 0x0000,			// wait for ext trigger
	XEQ|ALWAYS, MSG_BLK2_ADDR,		// 2
	JMP|ALWAYS, BC_ILIST_BASE_ADDR };	// loop to top 

        
        // copy BC Instruction List (above) to RAM...
 
            // fast-access write BC_ILIST_BASE_ADDR (defined in file 613x_bc.h) 
            // into the BC Instruction List Base Address register 
            Write_6131LowReg(BC_INST_LIST_BASE_ADDR_REG, BC_ILIST_BASE_ADDR, 1);
            
            // copy the BC Instruction List (declared above) into the HI-6130 RAM,
            // starting at the address just written into the base address register
            
            // enable Memory Address Pointer 1
            enaMAP(1);
               
            //  initialize the list start address into MAP1
            Write_6131LowReg(MAP_1, BC_ILIST_BASE_ADDR, 1);
    
            // instruction list array size, 16-bit words
            len = sizeof(inst_list) / sizeof(short int);
    
            __disable_interrupt();
            // Assert SPI chip select
            PIO_Clear(pinNss); 
            // Wait for TDR and shifter = empty
            while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
            // Send SPI write op code 0xC0 
            spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
            // Wait for TDRE flag (Tx Data Register Empty)
            while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
            // Wait for RDRF flag (Rx Data Register Full)
            while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
            // Read and discard received data char in Rx buffer
            dummy = spi->SPI_RDR & 0xFFFF;
            // prevent warning: variable dummy was set but never used
            dummy = dummy;
            
            // write instruction list
            for ( i = 0; i < len; i += 2) {
    
                // add the validation field and odd parity bit,
                // and write the adjusted op code word. 
                j = inst_list[i];
                // the next 4 lines determine odd parity 			
                j ^= j>>8;
                j ^= j>>4;
                j &= 0xF;
                if (!((0x6996 >> j) & 1))  { 
                    // add validation field and parity bit 15 = 1
                    j = inst_list[i] + VP1;
                }
                else {
                    // add validation field and parity bit 15 = 0
                    j = inst_list[i] + VP0;
                }
                // upper and lower op code word bytes
                bufferTX[0] = (char)(j >> 8);          
                bufferTX[1] = (char)j;               
                // upper and lower parameter word bytes
                bufferTX[2] = (char)(inst_list[i+1] >> 8);          
                bufferTX[3] = (char)(inst_list[i+1]); 
    
                // transmit the 4 bytes
                for (k = 0; k < 4; k++) {
                    // Wait for TDR and shifter = empty
                    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
                    // transmit next byte
                    spi->SPI_TDR = bufferTX[k] | SPI_PCS(BOARD_6131_NPCS);
                    // Wait for TDRE flag (Tx Data Register Empty)
                    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
                    // Wait for RDRF flag (Rx Data Register Full)
                    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
                    // Read and discard received data char in Rx buffer
                    dummy = spi->SPI_RDR & 0xFFFF;
                }  
            }
            // negate slave chip select
            PIO_Set(pinNss);
            __enable_interrupt();	        
        
}	// end initialize_bc_instruction_list()



// 	Next function initializes ten HI-613x Bus Controller Control/Status Blocks 
//      for test purposes, and initializes BC transmit data buffers used with RT receive commands. 
//
//	Msg Block 1  = Subaddress Tx  Command 03-1-30-00 (loopback subaddress) Bus A
//	Msg Block 2  = Subaddress Tx  Command 03-1-30-00 (loopback subaddress) Bus B
//	Msg Block 3  = Subaddress Rx  Command 03-0-30-00 (loopback subaddress) Bus A
//	Msg Block 4  = Subaddress BRx Command 31-0-05-11                       Bus B
//	Msg Block 5  = Subaddress BRx Command 31-0-30-00 (loopback subaddress) Bus A
//	Msg Block 6  = Mode Code  Tx  Command 03-1-31-02 (tx mode code 2)      Bus B
//	Msg Block 7  = Mode Code  Tx  Command 03-1-31-18 (tx mode code 18)     Bus A
//	Msg Block 8  = Mode Code  Rx  Command 03-0-31-21 (rx mode code 21)     Bus B
//      RT-RT Msg Block 1 = RT-RT msg Commands 04-0-30-02 03-1-05-02           Bus A
//      RT-RT Msg Block 2 = BRT-RT msg Commands 31-0-30-11 04-1-05-11          Bus B
// 
 
void initialize_bc_msg_blocks(void) {
  
	unsigned short i;
	unsigned short data[32] = {0x0101,0x0202,0x0303,0x0404,0x0505,0x0606,0x0707,0x0808,
			      0x0909,0x1010,0x1111,0x1212,0x1313,0x1414,0x1515,0x1616,
			      0x1717,0x1818,0x1919,0x2020,0x2121,0x2222,0x2323,0x2424,
			      0x2525,0x2626,0x2727,0x2828,0x2929,0x3030,0x3131,0x3232};
	//unsigned short data[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


        // ********************************************************************************
        // Msg Block 1  = Subaddress Tx  Command 03-1-30-00 (loopback subaddress) Bus A
        
	unsigned short msg_block1[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word          
	    RTRYENA|MEMASK|USEBUSA,

	//  Command Word (Transmit Command, RT --> BC )
	//  RT Addr Tx/Rx  SubAdd  WordCt 
	    3<<11  | TX  | 30<<5 | 0,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5308,  0x0200,  0xABCD,  0xABCD,  0xABCD,  0xABCD };    

	// ********************************************************************************
        // Msg Block 2  = Subaddress Tx  Command 03-1-30-00 (loopback subaddress) Bus B

	unsigned short msg_block2[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word          
	    RTRYENA|MEMASK|USEBUSB,

	//  Command Word (Transmit Command, RT --> BC )
	//  RT Addr Tx/Rx  SubAdd  WordCt 
	    3<<11  | TX  | 30<<5 | 0,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5308,  0x0200,  0xABCD,  0xABCD,  0xABCD,  0xABCD };    

	// ********************************************************************************
        // Msg Block 3  = Subaddress Rx  Command 03-0-30-00 (loopback subaddress) Bus A

	unsigned short msg_block3[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//                         |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word          
	    MEMASK|MSKBCR|USEBUSA,

	//  Command Word (Receive Command, BC --> RT )
	//  RT Addr Tx/Rx  SubAdd  WordCt 
	    3<<11  | RX  | 30<<5 | 0,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5328,  700,      0xABCD,  0xABCD,  0xABCD,  0xABCD };   

	// ********************************************************************************
        // Msg Block 4  = Subaddress BRx Command 31-0-05-11 Bus B

	unsigned short msg_block4[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//                         |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|BCST|USEBUSB,

	//  Command Word (Broadcast Receive Command, BC --> RTs )
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    31<<11 |  RX   | 1<<5 | 11,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5348,  0x333,    0xABCD,  0xABCD,  0xABCD,  0xABCD };   

	// ********************************************************************************
        // Msg Block 5  = Subaddress BRx Command 31-0-30-00 (loopback subaddress) Bus A

	unsigned short msg_block5[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|BCST|USEBUSA,

	//  Command Word (Broadcast Receive Command, BC --> RTs )
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    31<<11 |  RX   |30<<5 | 0,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5368,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD };    

	// ********************************************************************************
        // Msg Block 6  = Mode Code  Tx  Command 03-1-31-02 (tx mode code 2) Bus B


	unsigned short msg_block6[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|MCODE|USEBUSB,

	//  Command Word (Transmit Mode Command, MC0-MC15 ONLY )
	//  RT Addr  Tx/Rx  SubAdd  MCode 
	    3<<11 |  TX   | 31<<5 | 2,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0xDEAD,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD };   

	// ********************************************************************************
        // Msg Block 7  = Mode Code  Tx  Command 03-1-31-18 (tx mode code 18)     Bus A

	unsigned short msg_block7[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|MCODE|USEBUSA,
 
	//  Command Word (Transmit Mode Command, MC16-MC31 ONLY )
	//  RT Addr  Tx/Rx  SubAdd  MCode 
	    3<<11  |  TX  | 31<<5 | 18,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x1B62,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD };    

	// ********************************************************************************
        // Msg Block 8  = Mode Code  Rx  Command 03-0-31-21 (rx mode code 21)     Bus B

	unsigned short msg_block8[8] = {
	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|MCODE|TXTTMC17|USEBUSB,

	//  Command Word (Receive Mode Command, MC16-MC31 ONLY )
	//  RT Addr  Tx/Rx  SubAdd  MCode 
	    3<<11  |  RX   | 31<<5 | 21,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x1B55,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD };   

	// ********************************************************************************
        // RT-RT Msg Block 1   to RxRT4 from TxRT3   04-0-30-02 03-1-05-02  Bus A

	unsigned short rtrt_msg_block1[16] = {
	// This format is only used for RT-to-RT messages

	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|RT_RT|USEBUSA,

	//  Rx Command Word 
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    4<<11  |  RX  |  30<<5 | 2,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x5388,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD,  

	//  Tx Command Word 
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    3<<11  |  TX   | 1<<5 | 2,

	//  Rx RT
	//  Status   6 unused addresses for RT-to-RT only       
	    0x0000,  0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD };

	// ********************************************************************************
        // RT-RT Msg Block 2   broadcast to RxRT31 from TxRT3    31-0-30-11 04-1-05-11   Bus B

	unsigned short rtrt_msg_block2[16] = {
	// This format is only used for RT-to-RT messages 

	//  Contro1 Word Options = TXTTMC17|MEMASK|SRQMASK|BSYMASK|SSFMASK|TFMASK|RSVMASK
	//			   |RTRYENA|USEBUSA|USEBUSB|SFTEST|MSKBCR|EOMINT|MCODE|BCST|RT_RT 
	//  Contro1 Word
	    MEMASK|MSKBCR|BCST|RT_RT|USEBUSB,

	//  Rx Command Word 
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    31<<11 |  RX   |30<<5 | 11,

	//  Data     Time to   TimeTag  Block    LoopBack RT
	//  Addr     NextMsg   Word     Status   Word     Status 
	    0x53A8,  0,        0xABCD,  0xABCD,  0xABCD,  0xABCD,

	//  Tx Command Word 
	//  RT Addr  Tx/Rx  SubAdd  WordCt 
	    4<<11  |  TX   | 1<<5 | 11,

	//  Rx RT
	//  Status   6 unused addresses for RT-to-RT only       
	    0x0000,  0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD };

	// ********************************************************************************

		// copy the BC Message Control / Status Blocks (declared above) into HI-6131 RAM
                
                // enable Memory Address Pointer 1
                enaMAP(1);
           
        	//  initialize the list start address into MAP1
	        Write_6131LowReg(MAP_1, MSG_BLK1_ADDR, 1);	

		// these message blocks are contiguous, stored back-to-back.
		// take advantage of Mem Addr Pointer auto-increment after each write 


		// these non-RT-RT blocks need 8 words, must start with low nibble = 0x0 or 0x8 
		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block1[i],0); // starts at 0x3E00
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block2[i],0); // starts at 0x3E08
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block3[i],0); // starts at 0x3E10
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block4[i],0); // starts at 0x3E18
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block5[i],0); // starts at 0x3E20
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block6[i],0); // starts at 0x3E28
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block7[i],0); // starts at 0x3E30
		}

		for ( i = 0; i < 8; i++) {
			Write_6131_1word(msg_block8[i],0); // starts at 0x3E38
		}

		// these RT-RT blocks need 16 words, must start with low nibble = 0x0 
		for ( i = 0; i < 16; i++) {
			Write_6131_1word(rtrt_msg_block1[i],0); // starts at 0x3E40
		}

		for ( i = 0; i < 16; i++) {
			Write_6131_1word(rtrt_msg_block2[i],0); // starts at 0x3E50
		}

		// write dummy data into the transmit data buffers for the 3 receive subaddress commands 
                // REMEMBER: For Receive commands (that is RT receives), the BC IS TRANSMITTING...

		// a 32-word buffer for rx msg block 3
		Write_6131LowReg(MAP_1, msg_block3[2], 0);	
		for ( i = 0; i < 32; i++) {	
			Write_6131_1word(data[i], 1);
		}

		// a 32-word buffer for rx msg block 4 
		Write_6131LowReg(MAP_1, msg_block4[2], 0);	
		for ( i = 0; i < 32; i++) {	
			Write_6131_1word(data[i], 1);
		}

		// a 32-word buffer for rx msg block 5
		Write_6131LowReg(MAP_1, msg_block5[2], 0);	
		for ( i = 0; i < 32; i++) {	
			Write_6131_1word(data[31-i], 1);
		}

		// 16 mode data words for Rx MC16-MC31 starting at offset = 0x1B50 
		Write_6131LowReg(MAP_1, 0x1B50, 1);	
		for ( i = 1; i < 17; i++) {	
			// writes 0x1616 for Rx MC16 thru 0x3131 for Rx MC31
			Write_6131_1word(data[i+15],1);
		}
                
	///#endif	// end SPI 

}	// end initialize_bc_msg_blocks(void)




//	This function disables the Holt HI-613x BC by writing 
//	the Master Configuration Register to reset the BCENA bit.
//
void bc_disable(void) {
	
        // read-modify-write Master Config register

        unsigned short j = Read_6131LowReg(MASTER_CONFIG_REG, 1) & ~BCENA;
        Write_6131LowReg(MASTER_CONFIG_REG, j, 1);

}



// 	If the BCENA pin is high, this function enables the Holt 
//	HI-613x BC by writing the Master Configuration Register 
//	to set the BCENA bit. Then BC operation only begins after
//	the BCSTART bit or BCTRIG pin gets a rising edge.
//
void bc_enable(void) {
		        
        const Pin pinBCENA = PIN_BCENA;

	if(PIO_Get(&pinBCENA)) {
          
            // read-modify-write Master Config register
            unsigned short j = Read_6131LowReg(MASTER_CONFIG_REG, 1) | BCENA;
            Write_6131LowReg(MASTER_CONFIG_REG, j, 1);
	}
}	// return enabled but not started 



// 	If the BCENA pin is high, this function enables and starts 
//	the Holt HI-613x BC by writing the Master Configuration 
//	Register to set the BCENA and BCSTART bits.
//
void bc_start(void) {
	
        const Pin pinBCENA = PIN_BCENA;

	if(PIO_Get(&pinBCENA)) {
          
            // read-modify-write Master Config register

            unsigned short j = Read_6131LowReg(MASTER_CONFIG_REG, 1) | BCENA | BCSTRT ;
            Write_6131LowReg(MASTER_CONFIG_REG, j, 1);
	}
}	// return enabled and started 


// This function generates a BC Trigger pulse for the HI-613x.
//
void bc_trigger(void) {
	
    const Pin pinBCTRIG = PIN_BCTRIG;
    char i = 2;
    
    PIO_Set(&pinBCTRIG);        
    while (i--);       
    PIO_Clear(&pinBCTRIG);
}


// part of infinite standby loop when CONSOLE_IO is disabled.
// this function is used for demonstration for BC mode only...
//
void bc_switch_tests(void) {

    const Pin pinNSW1 = PIN_NSW1;
    //const Pin pinNSW2 = PIN_NSW2;

    // eval board button SW1 generates BC Trigger pulse
    if(!PIO_Get(&pinNSW1))  SW1_BC_Trigger();
    
    // eval board button SW2 modifies BC Condition Code and GP Flag Register
    // NEXT STATEMENT IS DISABLED TO AVOID CONFLICT WITH USER BC PROGRAM
    
}   // end bc_switch_tests()



// This function generates a BC Trigger pulse for the HI-613x
// if button SW1 is pressed. Demo program uses this call in main()
// if Console I/O is disabled. This triggers BC when a "wait for
// trigger" op code has executed. Demo program interleaves 1553 
// WTG op codes strictly for demonstration purposes).
//
char SW1_BC_Trigger(void){
  
    const Pin pinNSW1 = PIN_NSW1;
    unsigned short i=2000;

    if(!PIO_Get(&pinNSW1)) { 
         // button press
         while(i){
            // wait for release
            while(!PIO_Get(&pinNSW1));

            // if switch bounce, reload
            if(!PIO_Get(&pinNSW1)) i=2000;
            // otherwise dec debounce delay
            else i--;         
         }
         // trigger BC after debounce, minimize switch bounce retrigger
         bc_trigger();
         
         // read STATUS_AND_RESET_REG to verfy "BC msg in progress" bit, 0x0200. 
         // To see BCMIP state, set a breakpoint at "return 1" line below.

         i = Read_6131LowReg(STATUS_AND_RESET_REG, 1);

        i &= BCMIP;
               
         return 1;
    }
    else return 0;
}
        

//
// brief  For some BC tests, this function is called from main() standby loop 
//	  when user presses button SW2.	First press after reset, the BC Cond
//        Code and GP Flag Register is written so GP Flag bits 7-0 = 0x01.
//        On subsequent presses, the set bit rotates left, i.e.,0x02, 0x04,
//	  0x08, 0x10, 0x20, 0x40, 0x80 then next press starts over at 0x01.
// 		  
//	  primary purpose: testing the condition codes GP0-GP7 and nGP0-nGP7
 
void SW2_BCtest(void) {

        const Pin pinNSW2 = PIN_NSW2;
	static unsigned short press = 128;

	// SW2 was pressed before this function call 
	// turn on green LED 
        AT91C_BASE_PIOA->PIO_CODR = nLEDG;

	// wait for button release
        while(PIO_Get(&pinNSW2)) ;

	press = press << 1;
	if(press == 256) press = 1;
	    // no fast access reads for this register (must use MAP), 
            // but the fast access register writes below are okay...
            // reset all GP Flag bits by writing the 8 clear bits
            Write_6131LowReg(BC_CCODE_AND_GPF_REG, 0xFF00, 1);
            // set the GP Flag bits to match variable "press" by writing set bits
	    Write_6131LowReg(BC_CCODE_AND_GPF_REG, press, 1);

	// debounce delay then turn off green LED
	Delay_x100ms(4);
        AT91C_BASE_PIOA->PIO_SODR = nLEDG;
        
}   // end SW2_BCtest()



// This function initializes the Holt HI-613x BC by writing configuration registers in the device. 
// Only BC mode option bits are affected. The program has already initialized the common parameters
// shared by the BC, RT1, RT2 and/or Bus Monitor.
 
void initialize_613x_BC(void) {
		// HI-6131 uses host SPI interface to initialize registers and RAM.
		// The SPI can directly read registers 0-15 decimal, without using the mem address pointer.
		// The SPI can directly write registers 0-63 decimal, without using the mem address pointer.
		// For addresses > 63, all read/write accesses must use a memory address pointer. 

		unsigned short j = BCTO_138U|BCGTE|ETTSYN|TTSYNEN|BCRME|BCRE;//|BSYNDV|MENDV|RESBS|CHKCWFMT|BCGCE|BC2RE|BCR1A|BCR2A;
                Write_6131LowReg(BC_CONFIG_REG, j, 0);
		//Write_6131LowReg(BC_INST_LIST_BASE_ADDR_REG,OP_START_ADDR,0); 
		//Write_6131LowReg(BC_GP_QUEUE_POINTER, 0x00C0, 0);  // default (64 locations)
                j = BCWDT|SELMSG|BCGPQ|BCRETRY|CSTKERR|BCTRAP|STATSET|BCIRQMASK|BCMERR|BCEOM;
		Write_6131LowReg(BC_INT_ENABLE_REG, j, 0);
                j = BCWDT|SELMSG|BCGPQ|BCRETRY|CSTKERR|STATSET|BCIRQMASK|BCMERR|BCEOM; //|BCTRAP
		Write_6131LowReg(BC_INT_OUTPUT_ENABLE_REG, j, 0);

}	// end initialize_613x_BC()




// end of file 

