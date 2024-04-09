
//
//  file	board_6131.c
//  brief	This file contains routines for the Atmel AT91SAM3U4 processor's
// 		SPI (specifically SPI0) peripheral, operating as SPI Master. The 
//		Holt HI-6131 acts as SPI slave device. File contains initialization
//		functions, as well as example functions that utilize HI-6131 SPI 
//		op codes to read/write HI-6131 registers and RAM. If using a 
//		different microprocessor, translate the hardware control sequences 
//		to match the hardware used, but most microprocessor SPI peripherals 
//		operate similiarly. 
//
	 
//		HOLT DISCLAIMER
//
//		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//		EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//		OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//		NONINFRINGEMENT. 
//		IN NO EVENT SHALL HOLT, INC BE LIABLE FOR ANY CLAIM, DAMAGES
//		OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//		OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//		SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//		Copyright (C) 2009-2011 by  HOLT, Inc.
//		All Rights Reserved.
//
//
//	LIST OF FUNCTIONS
//	Direct-Addressed Read/Write Operations
// 	======================================
//	Write_6131LowReg( ) writes word to single register, registers 0 - 63 only
//	Read_6131LowReg( ) reads word from single register, registers 0 - 15 only
//
//	Read/write Operations Using the Memory Address Pointer
//	======================================================
//	Write_6131_1word( ) writes word to a single register or RAM location
//	Read_6131_1word( ) reads word from a single register or RAM location
//
//	Write_6131( ) writes N words to sequential register or RAM locations
//	Read_6131( ) reads N words from sequential register or RAM locations
//
//	these modify mem addr pointer relative to its current value, then perform read or write...
//	Write_6131_Buffer( ) writes N words to sequential register or RAM locations
//	Read_6131_Buffer( ) reads N words from sequential register or RAM locations
//
//	Read_Current_Control_Word( ) returns descriptor Control Word for the current/last command
//	Read_This_Control_Word() returns a specified descriptor Control Word
//	ReadWord_Adv4( ) returns data addressed by Memory Address Pointer, then adds 4 to ptr
//	Read_Last_IIW( ) returns the last Interrupt Information Word written to log buffer
//	Increase_Mem_Ptr( ) adds 1,2, or 4 to current Memory Address Pointer value in reg 15
//
//	Special Complex Functions
//	=========================
//	Fill_6131RAM_Offset( ) writes each RAM address with its address/offset value
//	Fill_6131RAM( ) writes a specified range of addresses with a fixed value
//	mem_dump( ) copies a 256-word block from HI-6131 reg/RAM to processor internal RAM
//	spi_demo( ) demonstrates various SPI function calls
//


//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <pio/pio.h>
#include <spi/spi.h>
#include <intrinsics.h>
#include "613x_initialization.h"
#include "board_6131.h"
#include "device_6131.h"
#include <stdio.h>

//------------------------------------------------------------------------------
//         Defines
//------------------------------------------------------------------------------

// none  

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

// Pins used by SPI
static const Pin pinsSPI[]  = {BOARD_6131_SPI_PINS, BOARD_6131_NPCS_PIN};

// HI-6131 /CS pin
//static const Pin pinNss[]  = {BOARD_6131_NPCS_PIN};

//------------------------------------------------------------------------------
//         Global Variables
//------------------------------------------------------------------------------


unsigned char spi_busy, spi_irq;

//	Array for storing 16-bit words read from HI-6131 RAM or registers. Several functions 
//	below read data from one or more sequential addresses. The reserved array size in 
//	this declaration can be adjusted to match project requirements. 
unsigned short read_data[256]; 

//------------------------------------------------------------------------------
//         Functions
//------------------------------------------------------------------------------


//	This function transmits the parameter 8-bit op code of the type that 
//      does not read or write following data word(s):
//
//      Op Code     Action
//       0xD8       Enable Memory Address Pointer 1 (MAP1) reg address 0x000B
//       0xD9       Enable Memory Address Pointer 2 (MAP2) reg address 0x000C
//       0xDA       Enable Memory Address Pointer 3 (MAP3) reg address 0x000D
//       0xDB       Enable Memory Address Pointer 4 (MAP4) reg address 0x000E
//       0xD0       Add 1 to currently-enabled Memory Address Pointer value
//       0xD2       Add 2 to currently-enabled Memory Address Pointer value
//       0xD4       Add 4 to currently-enabled Memory Address Pointer value
//
//      Any other op code value will give unpredictable results. The op code
//      is transmited a byte at a time. The data characters received during 
//      op code transmission are discarded.
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
void SPIopcode( unsigned char opcode)  {
        
    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short dummy;
        
    __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss);
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 
    spi->SPI_TDR = opcode | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // without this next delay, the ARM SPI reads wrong value in RDR!
    // I see good data shifting out from HI-6131 but RxData register value is wrong
    for (dummy=0; dummy<2; dummy++);
    // Read received data char in Rx buffer, discard it 
    dummy = spi->SPI_RDR & 0xFFFF;
    // prevent warning: variable dummy was set but never used
    dummy = dummy;
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
	__enable_interrupt();
}




// This function writes a single 16-bit value to a specified HI-6131 register 0-63.
// The function transmits an 8-bit op code, then transmits the data a byte at a time. 
// All data characters received during op code and data transmission are discarded.
//
// To avoid disruption during SPI transfers, interrupts that use SPI must be 
// temporarily disabled, delaying IRQ recognition until __enable_interrupt() occurs.
//      
// Either the calling routine issues __disable_interrupt() before calling this function, 
// or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
// choice determines where IRQ recognition	occurs. When series of spi function calls are 
// chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
// higher level (param irq_mgmt = 0)
//
// 	param 	data is the 16-bit word to be written to the target register
// 	param 	reg_number is the target register address, 0 through 63 ONLY.
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//			  if non-zero, this function locally calls __disable_interrupt() 
//                        and __enable_interrupt().
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
unsigned char Write_6131LowReg(unsigned char reg_number, unsigned short data, unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
	unsigned short i;
	//unsigned short dummy;
    volatile unsigned char bufferTX[2];
        
	if(reg_number > 63) return('F');	// illegal parameter 		

    bufferTX[0] = (char)(data >> 8);        // upper data byte  
    bufferTX[1] = (char)data;               // lower data byte 
        
    // disable interrupts, if IRQs managed at this level
	if(irq_mgmt)  __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss);                        
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI op code = 0x80 + reg_number
    spi->SPI_TDR = (0x80 + reg_number) | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    // dummy = spi->SPI_RDR & 0xFFFF;

    // transmit 2 data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[i] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        // dummy = spi->SPI_RDR & 0xFFFF;
        // prevent warning: variable dummy was set but never used
        // dummy = dummy;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
	if(irq_mgmt)  __enable_interrupt();	
        
	return('P');
}
        

// 	This function reads a single 16-bit value from a specified HI-6131 register 0-15.
//	The function transmits an 8-bit op code, then receives the data word as two 
//	bytes. The data is returned as a word. The data character received during 
//	op code transmission is discarded.
//
//	Either the calling routine issues __disable_interrupt() before calling this function, 
//  or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
//  choice determines where IRQ recognition	occurs. When series of spi function calls are 
//  chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
//  higher level (param irq_mgmt = 0)
//
// 	param 	reg_number is the target register address, 0 through 15 ONLY.
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//			  if non-zero, this function locally calls __disable_interrupt() 
//                        and __enable_interrupt().
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
unsigned short Read_6131LowReg(unsigned char reg_number, unsigned char irq_mgmt) {
       
    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short data, i;
    volatile unsigned char bufferRX[2];
        
    if(reg_number > 15) return('F');  // illegal parameter 		

    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI op code = reg_number << 2
    spi->SPI_TDR = (reg_number << 2) | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read received data char in Rx buffer, discard/overwrite below
    bufferRX[0] = spi->SPI_RDR;// & 0xFF;

    // receive 2 bytes register data, transmit 2 dummy data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive next byte
        spi->SPI_TDR = 0x00 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (data=0; data<2; data++);
        // Read received data char in Rx buffer
        bufferRX[i] = spi->SPI_RDR;// & 0xFF;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    // concantenate (merge) 2 data bytes into 16-bit value
    data =  (unsigned int)bufferRX[0] << 8;
    data |= (unsigned int)bufferRX[1];
        
    return data;
}
  



//  This function writes a single 16-bit word to the address indicated by the 
//  current value in the already-enabled HI-6131 Memory Address Pointer register. 
// The function transmits an 8-bit op code then the data word a byte at a time. 
// Data characters received during op code and data transmission are discarded.
// The Memory Address Pointer auto-increments after writing the data
//
// Either the calling routine issues __disable_interrupt() before calling this function, 
//  or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
//  choice determines where IRQ recognition	occurs. When series of spi function calls are 
//  chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
//  higher level (param irq_mgmt = 0)
//
// param 	data is the 16-bit word to be written
// param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//			  if non-zero, this function locally calls __disable_interrupt() 
//                        and __enable_interrupt().
//
void Write_6131_1word(unsigned short data, unsigned char irq_mgmt)  {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i;
    unsigned short dummy;
    volatile unsigned char bufferTX[2];
        
    // upper data byte
    bufferTX[0] = (char)(data >> 8);          
    // lower data byte
    bufferTX[1] = (char)data;               
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss);                        
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI op code 0xC0: write using enabled MAP current value
    spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;

    // transmit 2 data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[i] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        dummy = spi->SPI_RDR;
    }    
    // prevent warning: variable dummy was set but never used
    dummy = dummy;        
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
        
    return;
}
        



// This function reads one 16-bit register or RAM location, indicated by
// the address value in the HI-6131 Memory Address Pointer register. 
// The function transmits an 8-bit op code, then reads and returns the 
// data using SPI, a byte at a time. Bytes are merged into returned word.
//
// Either the calling routine issues __disable_interrupt() before calling this function, 
//  or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
//  choice determines where IRQ recognition	occurs. When series of spi function calls are 
//  chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
//  higher level (param irq_mgmt = 0)
//
//  param  irq_mgmt. if zero, the calling routine manages irq enable/disable.
//                   if non-zero, this function locally calls __disable_interrupt() 
//                   and __enable_interrupt().
//
// The MCU's SPI mode register is written with option WDRBT = 0 so Master
// can initialize a new transfer whether/not SPI RxData register has unread
// data. To speed up SPI operations by taking advantnage of the WDRBT
// option, several statements are commented-out in the function below.
//
unsigned short Read_6131_1word(unsigned char irq_mgmt) {
       
    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short data, i;
    volatile unsigned char bufferRX[2];
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
       
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI read op code 0x40
    spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read received data char in Rx buffer, overwrite/discard it below
    bufferRX[0] = spi->SPI_RDR & 0xFFFF;

    // receive 2 bytes register data, transmit 2 dummy data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data byte 0x00 to start receive
        spi->SPI_TDR = 0x00 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (data=0; data<5; data++);
       // Read received data char in Rx buffer
        bufferRX[i] = spi->SPI_RDR;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    // merge 2 data bytes into 16-bit value
    data =  (unsigned int)bufferRX[0] << 8;
    data |= (unsigned int)bufferRX[1];
        
	return data;
}
  
 

// This function writes one or more 16-bit words to the starting address indicated
// by the current value in the HI-6131 Memory Address Pointer register. The function
// transmits an 8-bit op code, then transmits data words a byte at a time. 
// All data characters received during op code and data transmission are discarded.
//
// Either the calling routine issues __disable_interrupt() before calling this function, 
//  or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
//  choice determines where IRQ recognition	occurs. When series of spi function calls are 
//  chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
//  higher level (param irq_mgmt = 0)
//
// Note: Because this function may perform long N-word transfers, it is typically 
// used with parameter irq_mgmt = 1. In this mode, IRQs are disabled during spi intervals,
// but IRQs are periodically enabled between whole word transfers. The function will 
// recover from vectored spi-using interrupts (recognized only between written data words) 
// when these IRQs use the demonstrated "spi_busy spi_irq" handshake. Such IRQs are 
// detected and a new SPI op code is issued to permit this function to continue to completion.
//
// 	param 	write_data[] array containing 16-bit words to be written, write_data[0] is written first
// 	param 	inc_pointer_first = 0 begins reading at current Memory Address Pointer addr, 
//                              1 (non-zero) increments Memory Address Pointer before reading
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function locally calls __disable_interrupt() 
//                        and __enable_interrupt().
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
void Write_6131(unsigned short write_data[], unsigned char inc_pointer_first, unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i, len;
    //unsigned short dummy;
    unsigned char opcode;
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
        
    if(inc_pointer_first) opcode = 0xC8;
    else opcode = 0xC0;
             
    if(irq_mgmt)  __disable_interrupt();
    // variable tested by vectored interrupt routine 
    spi_busy = 1;				
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 
    spi->SPI_TDR = opcode | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    // dummy = spi->SPI_RDR & 0xFFFF;
        
    len = sizeof(write_data);
                                                     
	for (i = 0; i < len; i++) 	{

		if(irq_mgmt) __enable_interrupt();
		// Before writing the next word, momentarily enable IRQs...
		// A pending IRQ that occurred since last __disable_interrupt() will be recognized here.
		// Its ISR (int.svc routine) will execute, probably using SPI, disrupting 
		// our multi-word transfer. The ISR sets the spi_irq flag, then execution 
		// resumes here, at this __disable_interrupt() call...   
		if(irq_mgmt) __disable_interrupt();			

		if(spi_irq) {
		    // the multi-word transfer was disturbed, but the interrupt's
	        // ISR should have used a different Memory Address Pointer and
            // reenabled the Memory Address Pointer we were using, so our
			// MAP points to the next word to be written. Issue a new SPI 
			// op code to resume a multi-word write process, starting at the 
			// RAM location addressed by the Memory Address Pointer value. 

            // Assert SPI chip select since there was an interrupt
            AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
            // Wait for TDR and shifter = empty
            while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
            // Issue SPI op code 0xC0: write using existing MAP pointer value.
            spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
            // Wait for TDRE flag (Tx Data Register Empty)
            while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
            // Wait for RDRF flag (Rx Data Register Full)
            // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
            // Read and discard received data char in Rx buffer
            // dummy = spi->SPI_RDR & 0xFFFF;
			spi_irq = 0;
		}
                
        // transmit next data word a byte at a time
                
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit upper byte
        spi->SPI_TDR = ((char)(write_data[i] >> 8)) | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        // dummy = spi->SPI_RDR & 0xFFFF;

        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit lower byte
        spi->SPI_TDR = ((char)(write_data[i])) | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        // dummy = spi->SPI_RDR & 0xFFFF;                            
	}
    // prevent warning: variable dummy was set but never used
    // dummy = dummy;
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
	spi_busy = 0;
    // re-enable interrupts, if IRQs managed at this level
	if(irq_mgmt) __enable_interrupt();				 

	return;
}




// 	This function reads one to 256 sequential 16-bit words beginning at the starting 
//	address indicated by the address value in the HI-6131 Memory Address Pointer register. 
//	The function transmits an 8-bit op code, then reads data words a byte at a time.
//	Words read are stored in global read_data[], starting at read_data[0].
//
//	Either the calling routine issues __disable_interrupt() before calling this function, 
//  or the __disable_interrupt() and __enable_interrupt() calls are performed here. This 
//  choice determines where IRQ recognition	occurs. When series of spi function calls are 
//  chained to perform larger tasks, it may be desirable to disable/enable IRQs at the 
//  higher level (param irq_mgmt = 0)
//
//	Note: Because this function may perform long N-word transfers, it is typically 
//	used with parameter irq_mgmt = 1. In this mode, IRQs are disabled during spi intervals,
//	but IRQs are periodically enabled between whole word transfers. The function will 
//	recover from vectored spi-using interrupts (recognized only between read data words) 
//	when these IRQs use the demonstrated "spi_busy spi_irq" handshake. Such IRQs are 
//	detected and a new SPI op code is issued to permit this function to continue to completion.
// 
// 	param 	number_of_words is the array size 
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function locally calls __disable_interrupt() 
//                    and __enable_interrupt().
//
//	Note: This function is designed to recover from a vectored interrupt (recognized only
//	between read data words) that disturbs the SPI operation. A new SPI op code is issued
//	to permit the operation to continue to completion.
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
#if 0
void Read_6131(unsigned short number_of_words, unsigned char irq_mgmt) {
// ************* THIS DOES NOT WORK *******************
    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    volatile unsigned char bufferRX[2];
    unsigned short dummy;
    unsigned short i,j;
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // variable tested by vectored interrupt routine
    spi_busy = 1;				         
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 0x40 to read using existing pointer value
    spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;
        
    for (i = 0; i < number_of_words; i++) 
      {

	if(irq_mgmt) __enable_interrupt();
	// Before reading the next word, momentarily enable IRQs...
	// A pending IRQ that occurred since last __disable_interrupt() will be recognized here.
	// Its ISR (int.svc routine) will execute, probably using SPI, disrupting 
	// our multi-word transfer. The ISR sets the spi_irq flag, then execution 
	// resumes here, at this next __disable_interrupt() call...   
	if(irq_mgmt) __disable_interrupt();

	if(spi_irq) 
          {
	    // the multi-word transfer was disturbed, but the interrupt's
	    // ISR should have used a different Memory Address Pointer and
            // reenabled the Memory Address Pointer we were using, so our
	    // MAP points to the next word to be read. Issue a new SPI 
	    // op code to resume a multi-word read process, starting at the 
	    // RAM location addressed by the Memory Address Pointer value. 
	    // Assert SPI chip select
	    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
	    // Wait for TDR and shifter = empty
	    // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
	    // Send SPI op code to resume reading using existing pointer value
	    spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
	    // Wait for TDRE flag (Tx Data Register Empty)
	    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
	    // Wait for RDRF flag (Rx Data Register Full)
	    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
	    // Read and discard received data char in Rx buffer
	    dummy = spi->SPI_RDR & 0xFFFF;
            // prevent warning: variable dummy was set but never used
            // dummy = dummy;
	    spi_irq = 0;
          }
      // receive 2 bytes RAM data, transmit 2 dummy data bytes
      for (j = 0; j < 2; j++) 
        {
          // Wait for TDR and shifter = empty
          while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);  // **** added this b a c k
          // transmit dummy data byte 0x00 to start receive
          spi->SPI_TDR = 0x00 | SPI_PCS(BOARD_6131_NPCS);
          // Wait for TDRE flag (Tx Data Register Empty)
          while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
          // Wait for RDRF flag (Rx Data Register Full)
          while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
          // without this next delay, the ARM SPI reads wrong value in RDR!
          // I see good data shifting out from HI-6131 but RxData register value is wrong
          for (dummy=0; dummy<5; dummy++);
          // Read received data char in Rx buffer
          bufferRX[j] = spi->SPI_RDR & 0xFFFF;
                  
          // merge 2 data bytes into 16-bit value and store
          read_data[i] =  (unsigned int)bufferRX[0] << 8;
          read_data[i] |= (unsigned int)bufferRX[1];
          }
      spi_busy = 0;
      // negate slave chip select
      AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
      // re-enable interrupts, if IRQs managed at this level 
      if(irq_mgmt)  __enable_interrupt();	    
      }
}
#endif

/*
Revised version 6/4/14

*/
void Read_6131(unsigned short address, unsigned short number_of_words) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short k,addr;
    unsigned char savemap, data, mod4=0;
    unsigned short dummy;
   
    
     __disable_interrupt();
    // we will restore the active MAP when finished
    savemap = (char)(getMAPaddr() - 0x000A);
    // use MAP3 
    enaMAP(3);
    addr = address & 0xFFF0;
    Write_6131LowReg(MAP_3,addr,0); 
    
    // variable tested by vectored interrupt routine 
    spi_busy = 1;				
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 
    spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR;// & 0xFFFF;
                            

    for (k = 0; k < number_of_words; k++) {
        read_data[k]=0;
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive upper byte
        spi->SPI_TDR = (0x00 | SPI_PCS(BOARD_6131_NPCS));
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (dummy=0; dummy<5; dummy++);
        // Read upper byte
        data = spi->SPI_RDR;// & 0xFF;
        printf("%.2X", (char)data);
        read_data[k] =  (unsigned int)data << 8;          
        
   
       // Now get the second byte 
        
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive lower byte
        spi->SPI_TDR = (0x00 | SPI_PCS(BOARD_6131_NPCS));
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (dummy=0; dummy<5; dummy++);
        // Read lower byte
        data = spi->SPI_RDR;// & 0xFF;
        printf("%.2X ", (char)data);
        read_data[k] |=  (unsigned int)data;                          
                
        // this next part provides normal operation when reading RT Descriptor tables.
        // MAP does not auto-increment when the next word is a descriptor Control Word.
        // every 4th word we reload MAP to force increment, d-table words 4,8,12, etc.
        // Behavior applies even when not accessing d-tables, AND ASSUMES RT DESCRIPTOR
        // TABLE(S) START AT BASE ADDRESS WITH 4-LSB'S = 0, any 0xNNN0: 0x0400, 0x04F0.
        // DESCRIPTOR TABLE START ADDRESS CANNOT EXCEED 0x7E00...
        addr++;
        mod4++;
        if (mod4 == 4) {
            mod4 = 0;
            // negate slave chip select. Below we reload MAP to force increment, 
            // then issue a new read op code 0x40 to resume read at next address
            AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
        }
        // Before reading the next word, momentarily enable IRQs...
        __enable_interrupt();
        // A pending IRQ that occurred since last disable_interrupt() will be recognized here.
        // Its int.service routine probably used SPI, disrupting our multi-word transfer. As long
        // as the interrupt routine sets the spi_irq flag, we can recover. When the interrupt is
        // finished, execution resumes at this next disable_interrupt() call, ...
                
        __disable_interrupt();
       
        if(spi_irq) enaMAP(4);
          
        if(spi_irq || (mod4==0)) {
            spi_irq = 0;
            // Reload MAP4 with the next read address, then issue a new SPI op code 0x40
            // to resume our multi-word read process where it left off. This occurs after
            // interrupt, or when new mod4 = 0, i.e., addr = 0xXXX0, 0xXXX4,0xXXX8 or 0xXXXC.
            Write_6131LowReg(MAP_3,addr,0); 
            
            // Wait for TDR and shifter = empty in case spi not idle
            while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
            // Negate then assert SPI chip select since there was an interrupt
            AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
            AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
            // Issue SPI op code 0x40: write using existing MAP pointer value.
            spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
            // Wait for TDRE flag (Tx Data Register Empty)
            while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
            // Wait for RDRF flag (Rx Data Register Full)
            while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
            // Read and discard received data char in Rx buffer
            dummy = spi->SPI_RDR & 0xFFFF;
        }             
    }   // end for(k = 0; k < 16; k++)
    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS;    
    spi_busy = 0;
    // prevent warning
    dummy = dummy;    
    // restore original MAP
    enaMAP(savemap);   
    __enable_interrupt();       
}    // end 


// 	After changing the Memory Address Pointer register in the HI-6131, this function writes one 
//	or more 16-bit words into sequential RAM. Before writing data, the pre-existing pointer value 
//	can be first increased by 0, 1 or 2, based on a passed parameter. Once adjusted, the value
//	in the newly addressed RAM location (assumed to be a data buffer address) is itself copied 
//	to the Memory Address Pointer. Then a multi-word write sequence begins and continues until the 
//	specified number of words is written.
//
//	Design Intent: This function is used imediately after reading an RT Descriptor Table Control Word. 
//	Because of auto-increment, the Memory Address Pointer points to the word following the Control 
//	Word. Based on Control Word value tests performed before this function call, the program needs 
//	to write data to one of the receive data buffers addressed by the three locations following the 
//	Control Word. The choice of parameter "inc_pointer_first" selects 1 of 3 pointers to be copied
//	to the Memory Address Pointer (reg 15) then "N" data words are written to the selected buffer:
//
//
//                      Descriptor Block     value of parameter   copied to Memory Address
//                      (4 words)            inc_pointer_first    Pointer before writing begins
//                      ==================   =================    =============================
//	THIS WORD READ -->  Control Word                    
//	JUST BEFORE         Data Pointer A              0              Data Buffer A addr
//      FUNCTION CALL       Data Pointer B              1              Data Buffer B addr
//                          Broadcast Data Ptr          2              Broadcast Data Buffer addr
//
//
//
//	Note: Because this function may perform long N-word transfers, it is typically 
//	used with parameter irq_mgmt = 1. In this mode, IRQs are disabled during spi intervals,
//	but IRQs are periodically enabled between whole word transfers. The function will 
//	recover from vectored spi-using interrupts (recognized only between written data words) 
//	when these IRQs use the demonstrated "spi_busy spi_irq" handshake. Such IRQs are 
//	detected and a new SPI op code is issued to permit this function to continue to completion.
// 
// 	param 	inc_pointer_first specifies pointer adjust value (0,1 or 2 only) before writing
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//	The MCU's SPI mode register is written with option WDRBT = 0 so Master
//	can initialize a new transfer whether/not SPI RxData register has unread
//	data. To speed up SPI operations by taking advantnage of the WDRBT
//	option, several statements are commented-out in the function below.
//
unsigned char Write_6131_Buffer(unsigned short write_data[], unsigned char inc_pointer_first, unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i, len;
    //unsigned short dummy;

    // inc_pointer_first parameter determines SPI op code value, must be 0,1 or 2 only
    if(inc_pointer_first > 2) return ('F');
    // op code will be 0xE8, 0xF0 or 0xF8
    i = 0xE8 + (inc_pointer_first << 3);

    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // variable tested by vectored interrupt routine
    spi_busy = 1;				         
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code i to write using adjusted pointer value
    spi->SPI_TDR = i | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    // dummy = spi->SPI_RDR & 0xFFFF;        
    len = sizeof(write_data);
    
    for (i = 0; i < len; i++) {

	if(irq_mgmt) __enable_interrupt();
	// Before writing the next word, momentarily enable IRQs...
	// A pending IRQ that occurred since last __disable_interrupt() will be 
        // recognized here. Its ISR (int.svc routine) will execute, probably 
        // using SPI, disrupting our multi-word transfer. The ISR sets the 
        // spi_irq flag, then execution resumes at this __disable_interrupt() call...   
	if(irq_mgmt) __disable_interrupt();

	if(spi_irq) {
		// the multi-word transfer was disturbed, but the interrupt's
		// ISR should have used a different Memory Address Pointer and
                // reenabled the Memory Address Pointer we were using, so our
		// MAP points to the next word to be write. Issue a new SPI 
		// op code to resume a multi-word write process, starting at the 
		// RAM location addressed by the Memory Address Pointer value. 
		// Assert SPI chip select
		AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
		// Wait for TDR and shifter = empty
		// while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
		// Send SPI op code to resume writing using existing pointer value
		spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
		// Wait for TDRE flag (Tx Data Register Empty)
		while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
		// Wait for RDRF flag (Rx Data Register Full)
		// while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
		// Read and discard received data char in Rx buffer
		// dummy = spi->SPI_RDR & 0xFFFF;
		spi_irq = 0;
	}
        // transmit next data word a byte at a time
                
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit upper byte
        spi->SPI_TDR = ((char)(write_data[i] >> 8)) | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        // dummy = spi->SPI_RDR & 0xFFFF;
    
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit lower byte
        spi->SPI_TDR = ((char)(write_data[i])) | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        // while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        // dummy = spi->SPI_RDR & 0xFFFF;                            
    }
    // prevent warning: variable dummy was set but never used
    // dummy = dummy;
    spi_busy = 0;
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    return ('P');
}



// 	After changing the Memory Address Pointer register in the HI-6131, this function reads one 
//	or more 16-bit words from sequential RAM. Before reading data, the pre-existing pointer value 
//	can be first increased by 0, 1 or 2, based on a passed parameter. Once adjusted, the value
//	in the newly addressed RAM location (assumed to be a data buffer address) is itself copied 
//	to the Memory Address Pointer. Then a multi-word read sequence begins and continues until the 
//	specified number of words is received.
//
//	Design Intent: This function is used imediately after reading a Descriptor Table Control Word. 
//	Because of auto-increment, the Memory Address Pointer points to the word following the Control 
//	Word. Based on Control Word value tests performed before this function call, the program needs 
//	to read data from one of the receive data buffers addressed by the three locations following the 
//	Control Word. The choice of parameter "inc_pointer_first" selects 1 of 3 pointers to be copied
//	to the Memory Address Pointer (reg 15) then "N" data words are read from the selected buffer:
//
//
//                      Descriptor Block     value of parameter   copied to Memory Address
//                      (4 words)            inc_pointer_first    Pointer before reading begins
//                      ==================   =================    =============================
//	THIS WORD READ -->  Control Word                    
//	JUST BEFORE         Data Pointer A              0              Data Buffer A addr
//      FUNCTION CALL       Data Pointer B              1              Data Buffer B addr
//                          Broadcast Data Ptr          2              Broadcast Data Buffer addr
//
//
//
//	Note: Because this function may perform long N-word transfers, it is typically 
//	used with parameter irq_mgmt = 1. In this mode, IRQs are disabled during spi intervals,
//	but IRQs are periodically enabled between whole word transfers. The function will 
//	recover from vectored spi-using interrupts (recognized only between read data words) 
//	when these IRQs use the demonstrated "spi_busy spi_irq" handshake. Such IRQs are 
//	detected and a new SPI op code is issued to permit this function to continue to completion.
// 
// 	param 	number_of_words is the array size 
// 	param 	inc_pointer_first specifies pointer adjust value (0,1 or 2 only) before reading
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//	The MCU's SPI mode register is written with option WDRBT = 0 so Master
//	can initialize a new transfer whether/not SPI RxData register has unread
//	data. To speed up SPI operations by taking advantnage of the WDRBT
//	option, several statements are commented-out in the function below.
//
unsigned char Read_6131_Buffer(unsigned short number_of_words, unsigned char inc_pointer_first, unsigned char irq_mgmt) {
// revised on 6/4/14
  
    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    volatile unsigned char bufferRX[2];
    unsigned short i;
    unsigned short dummy;
    unsigned char data;
    

    // inc_pointer_first parameter determines SPI op code value, must be 0,1 or 2 only
    if(inc_pointer_first > 2) return ('F');
    // op code will be 0x68, 0x70 or 0x78
    i = 0x68 + (inc_pointer_first << 3);

    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // variable tested by vectored interrupt routine
    spi_busy = 1;				         
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code i to read using adjusted pointer value
    spi->SPI_TDR = i | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;        

    for ( i = 0; i < number_of_words; i++ )	{
	__enable_interrupt();
	// Before writing the next word, momentarily enable IRQs...
	// A pending IRQ that occurred since last __disable_interrupt() will be 
        // recognized here. Its ISR (int.svc routine) will execute, probably 
        // using SPI, disrupting our multi-word transfer. The ISR sets the 
        // spi_irq flag, then execution resumes at this __disable_interrupt() call...   
	if(irq_mgmt) __disable_interrupt();

	if(spi_irq) {
          // the multi-word transfer was disturbed, but the interrupt's
          // ISR should have used a different Memory Address Pointer and
          // reenabled the Memory Address Pointer we were using, so our
          // MAP points to the next word to be write. Issue a new SPI 
          // op code to resume a multi-word write process, starting at the 
          // RAM location addressed by the Memory Address Pointer value. 
          // Assert SPI chip select
          AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
          // Wait for TDR and shifter = empty
          while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
          // Send SPI op code 0x40 to resume reading using existing pointer value
          spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
          // Wait for TDRE flag (Tx Data Register Empty)
          while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
          // Wait for RDRF flag (Rx Data Register Full)
          while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
          // Read and discard received data char in Rx buffer
          dummy = spi->SPI_RDR & 0xFFFF;
          spi_irq = 0;
        }
	// Read first byte 
        read_data[i] = 0;         // clear buffer first
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data byte 0x00 to start receive
        spi->SPI_TDR = 0x00 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (dummy=0; dummy<5; dummy++);
        // Read received data char in Rx buffer
        data = spi->SPI_RDR;
        printf("%.2X", (char)data);
        read_data[i] =  (unsigned int)data << 8;                

       // Get the second byte 
        
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive lower byte
        spi->SPI_TDR = (0x00 | SPI_PCS(BOARD_6131_NPCS));
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (dummy=0; dummy<5; dummy++);
        // Read lower byte
        data = spi->SPI_RDR;// & 0xFF;
        printf("%.2X ", (char)data);
        read_data[i] |=  (unsigned int)data; 
        printf("%.2X ", (char)data);        
    }
    // prevent warning: variable dummy was set but never used
    dummy = dummy;
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    spi_busy = 0;
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	

    return ('P');
}


//  This function reads a single 16-bit value from the RAM location pointed to by the 
//  specified RT1 or RT2 Current Control Word Address register. The function transmits 
//  an 8-bit op code, then receives the data word as two bytes. The data is returned as 
//  a word. The function copies the specified RT1 or RT2 Current Control Word Address 
//  register to the enabled Memory Address Pointer register before reading the word.
//	The data character received during op code transmission is discarded.
//
//  If parameter irq_mgmt is zero, this function disables interrupts while SPI 
//  transfers are underway. The two vectored interrupts from the HI-6131 (hardware and 
//  message IRQs) use SPI and would cause disruption. Since this is a 3-byte SPI sequence,
//  IRQ recognition is just delayed briefly. If irq_mgmt is non-zero, the calling
//  function should disable IRQs before calling this function...
//
//    param     rt_num.   '1' specifies RT1 or '2' (any value except '1') specifies RT2
//    param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//			  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//	The MCU's SPI mode register is written with option WDRBT = 0 so Master
//	can initialize a new transfer whether/not SPI RxData register has unread
//	data. To speed up SPI operations by taking advantnage of the WDRBT
//	option, several statements are commented-out in the function below.
//
unsigned short Read_Current_Control_Word(unsigned char rt_num, unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short dummy;
    unsigned char opcode;
    unsigned short data, i;
    volatile unsigned char bufferRX[2];

    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
        
    if(rt_num == 2) opcode = 0x50;
    else opcode = 0x48;
             
    if(irq_mgmt)  __disable_interrupt();
    // variable tested by vectored interrupt routine 
    spi_busy = 1;				
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 
    spi->SPI_TDR = opcode | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;
                                                     
    // receive 2 bytes register data, transmit 2 dummy data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data byte 0x00 to start receive
        spi->SPI_TDR = 0x00 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (dummy=0; dummy<2; dummy++);
        // Read received data char in Rx buffer
        bufferRX[i] = spi->SPI_RDR & 0xFFFF;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    // concantenate (merge) 2 data bytes into 16-bit value
    data =  (unsigned int)bufferRX[0] << 8;
    data |= (unsigned int)bufferRX[1];
        
    return data;
}




// 	These functions return a single 16-bit Control Word from the RT1 or RT2 Descriptor Table, 
//	specified by the passed parameters. Separate functions for RT1 and RT2.  Example usage:
//			
//	Read_RT1_Control_Word('R','S',21,0) returns Control Word for receive subaddress 21.
//	Read_RT2_Control_Word('T','M',18,0) returns Control Word for transmit mode command 18.
//
//	The function determines the address offset from the provided paramters, then adds
//	the offset to the Descriptor Table base address in register 5. The resultant address
//	is written to the Memory Address Pointer (reg 15) and the addressed RAM location
//	is read and the value is returned.
//
//	A returned value of 0xFFFF (not a valid Control Word value) signifies an error 
//	condition, such as undefined parameter, out-of-range subaddress or mode code value,etc.
//
//	If parameter irq_mgmt is zero, this function disables interrupts while SPI 
//	transfers are underway. The two vectored interrupts from the HI-6131 (hardware and 
//	message IRQs) use SPI and would cause disruption. If irq_mgmt is non-zero, the calling
//	function should disable IRQs before calling this function...
//
//      param	txrx specifies transmit or receive, valid characters are 'T','t','R'or'r'
//      param	samc specifies subaddress or mode command, valid characters are 'S','s','M'or'm'
//      param	number is subaddress or mode code number. (1-30 for subaddress, or 0-31 for mode cmds)
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
unsigned short Read_RT1_Control_Word(unsigned char txrx, unsigned char samc, unsigned char number, unsigned char irq_mgmt) {

	unsigned short address = 0, data;

	if((samc == 's') || (samc == 'S')) {
		if ((number > 0) && (number < 31)) {
			address = number << 2;
		}
		else return (0xFFFF);	// error - subaddress must be in range 1-30 inclusive 
	}

	else if ((samc == 'm') || (samc == 'M')) {

		if (number < 32) {
			address = number << 2;
			address += 0x100;
		}
		else return (0xFFFF);	// error - mode command with invalid mode number 
	}

	else  return  (0xFFFF);		// error - not coded s,S,m or M for subaddress or mode cmd 

	if((txrx == 't') || (txrx == 'T'))

		address += 0x080;

//	else if ((txrx |= 'r') && (txrx |= 'R'))
	else if ((txrx != 'r') && (txrx != 'R')) // this needs verifying

		return (0xFFFF);	// error - not coded t,T,r or R for transmit or receive 

	if(irq_mgmt) __disable_interrupt();		// disable interrupts, if IRQs managed at this level 

	data = Read_6131LowReg(RT1_DESC_TBL_BASE_ADDR_REG,0);

	address += data;		// add table base addr to offset, then load mem addr ptr 

	enaMAP(1);
	
	Write_6131LowReg(MAP_1,address,0);	

	data = Read_6131_1word(0);    // read desired Control Word from the Descriptor Table 

	if(irq_mgmt) __enable_interrupt();		// re-enable interrupts, if IRQs managed at this level 

	return data;
}




// the RT2 version
unsigned short Read_RT2_Control_Word(unsigned char txrx, unsigned char samc, unsigned char number, unsigned char irq_mgmt) {

	unsigned short address = 0, data;

	if((samc == 's') || (samc == 'S')) {
		if ((number > 0) && (number < 31)) {
			address = number << 2;
		}
		else return (0xFFFF);	// error - subaddress must be in range 1-30 inclusive 
	}

	else if ((samc == 'm') || (samc == 'M')) {

		if (number < 32) {
			address = number << 2;
			address += 0x100;
		}
		else return (0xFFFF);	// error - mode command with invalid mode number 
	}

	else  return  (0xFFFF);		// error - not coded s,S,m or M for subaddress or mode cmd 

	if((txrx == 't') || (txrx == 'T'))

		address += 0x080;

	else if ((txrx |= 'r') && (txrx |= 'R'))

		return (0xFFFF);	// error - not coded t,T,r or R for transmit or receive 

	if(irq_mgmt) __disable_interrupt();		// disable interrupts, if IRQs managed at this level 

	data = Read_6131LowReg(RT2_DESC_TBL_BASE_ADDR_REG,0);

	address += data;		// add table base addr to offset, then load mem addr ptr 

	enaMAP(1);
	
	Write_6131LowReg(MAP_1,address,0);	

	data = Read_6131_1word(0);    // read desired Control Word from the Descriptor Table 

	if(irq_mgmt) __enable_interrupt();		// re-enable interrupts, if IRQs managed at this level 

	return data;
}



//  This function reads a single 16-bit value from the RAM location pointed	to by
//  the enabled Memory Address Pointer, then advances that Memory Address Pointer
//  value by 4 RAM addresses. The function transmits an 8-bit op code, then 
//  receives the addressed data word as two bytes. The data is returned as a word.
//  The data character received during op code transmission is discarded.
//
//  Design Intent: The host can easily read successive descriptor table Control
//  Words, spaced at 4-word intervals. Example use: message activity can be detected 
//  for subaddresses not programmed to generate interrupts. Host can poll the DBAC 
//  (Data Block Accessed) bit in each Control Word, which is set when a message 
//  is completed. The DBAC bit automatically resets when the Control Word is read.
//
//  If parameter irq_mgmt is zero, this function disables interrupts while SPI 
//  transfers are underway. The two vectored interrupts from the HI-6131 (hardware and 
//  message IRQs) use SPI and would cause disruption. Since this is a 3-byte SPI sequence,
//  IRQ recognition is just delayed briefly. If irq_mgmt is non-zero, the calling
//  function should disable IRQs before calling this function...
//
//  param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//  The MCU's SPI mode register is written with option WDRBT = 0 so Master
//  can initialize a new transfer whether/not SPI RxData register has unread
//  data. To speed up SPI operations by taking advantnage of the WDRBT
//  option, several statements are commented-out in the function below.
//
unsigned short ReadWord_Adv4(unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short data, i;
    volatile unsigned char bufferRX[2];
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI op code 0x60
    spi->SPI_TDR = 0x60 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read received data char in Rx buffer, discard/overwrite below
    bufferRX[0] = spi->SPI_RDR & 0xFFFF;

    // receive 2 bytes register data, transmit 2 dummy data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive next byte
        spi->SPI_TDR = 0x0 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (data=0; data<2; data++);
        // Read received data char in Rx buffer
        bufferRX[i] = spi->SPI_RDR & 0xFFFF;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    // merge 2 data bytes into 16-bit value
    data =  (unsigned int)bufferRX[0] << 8;
    data |= (unsigned int)bufferRX[1];
        
    return data;
}
  
// This function performs the following steps: (1) write enabled memory address pointer
// with the current bit 8:0 value in Interrupt Log Address register, minus 1. Upper bits
// 15:9 (indicating number of interrupts since Log Address register was read) are not 
// used. Since the Interrupt Log Address register points to the storage address for the
// next-occurring interrupt, the decremented MAP value points to the second word stored 
// for the last interrupt, the IIW Interrupt Address Word. If the Log Address register 
// points to the first log buffer address 0x0180, then the last buffer address 0x01BF is 
// written to the enabled memory pointer register because buffer wrap-around occurred. 
// (2) The last-written Interrupt Address Word indicated by the enabled memory address
// pointer is read. (3) The memory address pointer DECREMENTS, addressing the corresponding
// Interrupt Information Word. The function returns the last IAW but not the IIW. (4) Upon
// return, the calling routine can read the corresponding IIW now addressed by the enabled
// memory address pointer. 
//
// The 0x58 op code can be used to read a single IAW location, or a revision to this function
// can start a multi-word read sequence in which the memory address pointer automatically 
// DECREMENTS after each word read, as long as SPI clock continues in 16-pulse multiples. 
// This is the only SPI op code that decrements the memory pointer for multi-word operations. 
// Repeated memory pointer decrements would wrap around the 0x0180 to 0x01BF log buffer boundary,
// returning IAW then IIW word pairs, in reverse order of interrupt occurence (last in, first out).
//
// This function reads the last Interrupt Address Word (IAW) written to the HI-6131 Interrupt 
// Log buffer. The function transmits the 8-bit 0x58 op code, then receives the IAW word as two 
// bytes, returned as a word. Upon return, the memory address pointer points to the IIW Interrupt
// Information Word corresponding to the returned IAW. The data character received during op code 
// transmission is discarded.
//
// If parameter irq_mgmt is zero, this function disables interrupts while SPI transfers are 
// underway. If irq_mgmt is non-zero, the calling function should disable IRQs before calling this function...
//
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
//		The MCU's SPI mode register is written with option WDRBT = 0 so Master
//		can initialize a new transfer whether/not SPI RxData register has unread
//		data. To speed up SPI operations by taking advantnage of the WDRBT
//		option, several statements are commented-out in the function below.
//
unsigned short Read_Last_Interrupt(unsigned char irq_mgmt) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short data, i;
    volatile unsigned char bufferRX[2];
        
    // disable interrupts, if IRQs managed at this level
    if(irq_mgmt)  __disable_interrupt();	 
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send 8-bit SPI op code 
    spi->SPI_TDR = 0x58 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read received data char in Rx buffer, discard/overwrite below
    bufferRX[0] = spi->SPI_RDR & 0xFFFF;

    // receive 2 bytes register data, transmit 2 dummy data bytes
    for (i = 0; i < 2; i++) {
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit dummy data to receive next byte
        spi->SPI_TDR = 0x0 | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
        while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // without this next delay, the ARM SPI reads wrong value in RDR!
        // I see good data shifting out from HI-6131 but RxData register value is wrong
        for (data=0; data<2; data++);
        // Read received data char in Rx buffer
        bufferRX[i] = spi->SPI_RDR & 0xFFFF;
    }    
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
		
    // re-enable interrupts, if IRQs managed at this level 
    if(irq_mgmt)  __enable_interrupt();	
    // merge 2 data bytes into 16-bit value
    data =  (unsigned int)bufferRX[0] << 8;
    data |= (unsigned int)bufferRX[1];
    // return last interrupt's IAW, the MAP points to matching IIW    
    return data;
}
  

	
// 
//  This function fills the entire HI-6131 RAM space 0x0040-0x7FFF. Each RAM
//  location is written with its memory address. Example: RAM addresses 
//  0x01BA and 0x07EF are written with values 0x01BA and 0x07EF respectively.
//
//  If exercised before RAM initialization, this RAM fill can verify locations
//  of reserved data buffers for valid transmit subaddress commands because the
//  transmitted data values indicate where the transmitted data was stored.
//
//  This function should not be used while terminal execution is enabled. 
//  Interrupts are disabled while SPI is in use. 
//
//	NOTE: Upper byte of RT Descriptor Table Control Words will not be overwritten
//      param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
//
void Fill_6131RAM_Offset(void) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i;
    unsigned short dummy;
    volatile unsigned char bufferTX[2];
        	
    __disable_interrupt();
    enaMAP(1);
    // read-modify-write Test Control reg 0x004D to set FRAMA
    // bit allowing RT descriptor tables to write normally
    Write_6131LowReg(MAP_1,0x004D,0);	
    i = Read_6131_1word(0);	 
    Write_6131LowReg(MAP_1,0x004D,0); 
    Write_6131_1word(i|0x1000,0);

    // Assert SPI chip select since there was an interrupt
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Issue SPI op code 0xC0: write using existing MAP pointer value.
    spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;

    // write mem addr pointer with first RAM address
    Write_6131LowReg(MAP_1,0x0050,0); 
    // 32K minus 80 words
    for (i = 0x0050; i < 0x8000; i++) {	 
	// upper data byte
	bufferTX[0] = (char)(i >> 8);          
	// lower data byte
	bufferTX[1] = (char)i; 
	// -------------------------------------------		
        // Wait for TDR and shifter = empty
        // while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[0] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
	while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
         dummy = spi->SPI_RDR & 0xFFFF;
	// -------------------------------------------
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[1] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
	while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        dummy = spi->SPI_RDR & 0xFFFF;
    }
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
	
    // read-modify-write Test Control reg 0x004D to reset FRAMA
    Write_6131LowReg(MAP_1,0x004D,0);	
    i = Read_6131_1word(0);	 
    Write_6131LowReg(MAP_1,0x004D,0); 
    Write_6131_1word((i & 0x7FFF),0);

    __enable_interrupt(); 
    // prevent warning: variable dummy was set but never used
    dummy = dummy;
}


// 
// This function fills a range in HI-6131 RAM space 0x0040-0x7FFF with a specified value.
// This may be used to clear a range of RAM memory addresses
//
// This function should not be used while terminal execution is enabled, 
// Interrupts are disabled while SPI is in use. 
//
//  param 	addr is first storage address. VALUE MUST EXCEED 0x1F to avoid register space
//  param 	num_words is the number of 16-bit words to be written, MAXIMUM (0x8000-0x50) = 32688 decimal 
//  param 	fill_value, 16 bits. 0x0000 clears RAM 
///
void Fill_6131RAM(unsigned short addr, unsigned short num_words, unsigned short fill_value) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i;
    unsigned short dummy;
    volatile unsigned char bufferTX[2];
        	
    __disable_interrupt();
    enaMAP(1);
    // write mem addr pointer with first RAM address
    Write_6131LowReg(MAP_1,addr,0); 
	
    // upper data byte
    bufferTX[0] = (char)(fill_value >> 8);          
    // lower data byte
    bufferTX[1] = (char)fill_value; 

    // Assert SPI chip select since there was an interrupt
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Issue SPI op code 0xC0: write using existing MAP pointer value.
    spi->SPI_TDR = 0xC0 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR & 0xFFFF;

    for (i = num_words; i > 0; i--)	{

	// -------------------------------------------		
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[0] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
	while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        dummy = spi->SPI_RDR & 0xFFFF;
	// -------------------------------------------
        // Wait for TDR and shifter = empty
        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
        // transmit next byte
        spi->SPI_TDR = bufferTX[1] | SPI_PCS(BOARD_6131_NPCS);
        // Wait for TDRE flag (Tx Data Register Empty)
	while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
        // Wait for RDRF flag (Rx Data Register Full)
        while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
        // Read and discard received data char in Rx buffer
        dummy = spi->SPI_RDR & 0xFFFF;
    }
    // negate slave chip select
    AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);

    __enable_interrupt(); 
    // prevent warning: variable dummy was set but never used
    dummy = dummy;
}


// 
// This function copies 1024 words from HI-6131 register/RAM address space 
// for console screen display. This only applies for HI-6131 since HI-6130
// can use Embedded Workbench "Memory Watch Window" for the same purpose. 
//
// Press button SW2 to prompt screen refresh, This interrupts C program 
// execution and uses memory address pointer MAP4. Interrupts will temporarily
// be disabled during each SPI word transfer. After completion, the incoming 
// memory address pointer will be re-enabled. If spi_busy = 1 (indicating a 
// multi-word SPI process was interrupted) flag spi_irq will be set so the
// interrupted process can resume, assuming interrupted process uses the same
// interrupt management as this function.

void Memory_watch(unsigned short address) {

    AT91S_SPI *spi = BOARD_6131_SPI_BASE;
    unsigned short i,j,k,addr;
    unsigned char savemap, data, mod4=0;
    unsigned short dummy;
	
     __disable_interrupt();
    // we will restore the active MAP when finished
    savemap = (char)(getMAPaddr() - 0x000A);
    // use MAP3 
    enaMAP(3);
    addr = address & 0xFFF0;
    Write_6131LowReg(MAP_3,addr,0); 
    
    // variable tested by vectored interrupt routine 
    spi_busy = 1;				
    // Assert SPI chip select
    AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
    // Wait for TDR and shifter = empty
    while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
    // Send SPI op code 
    spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
    // Wait for TDRE flag (Tx Data Register Empty)
    while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
    // Wait for RDRF flag (Rx Data Register Full)
    while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
    // Read and discard received data char in Rx buffer
    dummy = spi->SPI_RDR;// & 0xFFFF;
                            
    // 4 groups of 4 lines each
    for (i = 0; i < 4; i++) { 
	  // 4 lines preceded by header
	  printf("\n\rx%.2X%.2X    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F", (char)(addr>>8), (char)addr);
	  for (j = 0; j < 4; j++) {
		printf("\n\r");
		// 16 words / line 
		for (k = 0; k < 16; k++) {
                  
                        // Wait for TDR and shifter = empty
			while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
			// transmit dummy data to receive upper byte
			spi->SPI_TDR = (0x00 | SPI_PCS(BOARD_6131_NPCS));
			// Wait for TDRE flag (Tx Data Register Empty)
			while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
			// Wait for RDRF flag (Rx Data Register Full)
			while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
                        // without this next delay, the ARM SPI reads wrong value in RDR!
                        // I see good data shifting out from HI-6131 but RxData register value is wrong
                        for (dummy=0; dummy<5; dummy++);
			// Read upper byte
			data = spi->SPI_RDR;// & 0xFF;
			printf("%.2X", (char)data);

                        // Wait for TDR and shifter = empty
                        while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
			// transmit dummy data to receive lower byte
			spi->SPI_TDR = (0x00 | SPI_PCS(BOARD_6131_NPCS));
			// Wait for TDRE flag (Tx Data Register Empty)
			while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
			// Wait for RDRF flag (Rx Data Register Full)
			while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
                        // without this next delay, the ARM SPI reads wrong value in RDR!
                        // I see good data shifting out from HI-6131 but RxData register value is wrong
                        for (dummy=0; dummy<5; dummy++);
			// Read lower byte
			data = spi->SPI_RDR;// & 0xFF;
			printf("%.2X ", (char)data);
                                
                        // this next part provides normal operation when reading RT Descriptor tables.
                        // MAP does not auto-increment when the next word is a descriptor Control Word.
                        // every 4th word we reload MAP to force increment, d-table words 4,8,12, etc.
                        // Behavior applies even when not accessing d-tables, AND ASSUMES RT DESCRIPTOR
                        // TABLE(S) START AT BASE ADDRESS WITH 4-LSB'S = 0, any 0xNNN0: 0x0400, 0x04F0.
                        // DESCRIPTOR TABLE START ADDRESS CANNOT EXCEED 0x7E00...
			addr++;
                        mod4++;
                        if (mod4 == 4) {
                            mod4 = 0;
                            // negate slave chip select. Below we reload MAP to force increment, 
                            // then issue a new read op code 0x40 to resume read at next address
                            AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
                        }
			// Before reading the next word, momentarily enable IRQs...
			__enable_interrupt();
			// A pending IRQ that occurred since last disable_interrupt() will be recognized here.
			// Its int.service routine probably used SPI, disrupting our multi-word transfer. As long
			// as the interrupt routine sets the spi_irq flag, we can recover. When the interrupt is
                        // finished, execution resumes at this next disable_interrupt() call, ...
				
			__disable_interrupt();
                        
                        if(spi_irq) enaMAP(4);
                          
			if(spi_irq || (mod4==0)) {
				spi_irq = 0;
				// Reload MAP4 with the next read address, then issue a new SPI op code 0x40
                                // to resume our multi-word read process where it left off. This occurs after
                                // interrupt, or when new mod4 = 0, i.e., addr = 0xXXX0, 0xXXX4,0xXXX8 or 0xXXXC.
                                Write_6131LowReg(MAP_3,addr,0); 
				
				// Wait for TDR and shifter = empty in case spi not idle
				while ((spi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
				// Negate then assert SPI chip select since there was an interrupt
				AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; // faster than PIO_Set(pinNss);
				AT91C_BASE_PIOA->PIO_CODR = SPI_nCS; // faster than PIO_Clear(pinNss); 
				// Issue SPI op code 0x40: write using existing MAP pointer value.
				spi->SPI_TDR = 0x40 | SPI_PCS(BOARD_6131_NPCS);
				// Wait for TDRE flag (Tx Data Register Empty)
				while ((spi->SPI_SR & AT91C_SPI_TDRE) == 0);
				// Wait for RDRF flag (Rx Data Register Full)
				while ((spi->SPI_SR & AT91C_SPI_RDRF) == 0);
				// Read and discard received data char in Rx buffer
				dummy = spi->SPI_RDR & 0xFFFF;
                        } 
                        
                }   // end for(k = 0; k < 16; k++)
	  }         // end for(j = 0; j < 4; j++)
    }               // end for (i = 0; i < 4; i++)
    
	// negate slave chip select
        AT91C_BASE_PIOA->PIO_SODR = SPI_nCS; 

	printf("\n\r===============================================================================");
	printf("\n\rKeys: (W)atch On/Off  (D)own  (U)p  (R)efresh  (A)ddress  (M)enu  ");
        printf("0x%.2X%.2X-0x%.2X%.2X", (char)((addr-256)>>8),(char)(addr-256),(char)((addr-1)>>8),(char)(addr-1));
	printf("\n\r===============================================================================\n\r");
	//          
	spi_busy = 0;
        // prevent warning
        dummy = dummy;
        
	// restore original MAP
	enaMAP(savemap);
        
	__enable_interrupt();
        
}    // end 






/*	                 80 x 24 fomatting for above fctn
         1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

6000    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
6040    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
6080    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
60C0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
0000 1111 2222 3333 4444 5555 6666 7777 8888 9999 AAAA BBBB CCCC DDDD EEEE FFFF
===============================================================================
'M' toggles HI-6131 memory watch on/off.  'U' or 'D' scrolls addr range up/down
'A' for new start address entry.      '0' (zero) displays the main console menu
================    PRESS BOARD BUTTON SW1 TO REFRESH SCREEN   ================
*/



//      This function returns the address of the currently enabled Memory Address Pointer
//      register, either 0x000B, 0x000C, 0x000D or 0x000E
//
unsigned short getMAPaddr(void) {
      // bits 11-10 of Master Configuration reg
      unsigned short i;
      i = Read_6131LowReg(MASTER_CONFIG_REG, 1) >> 10;
      i = 0x000B + (i & 0x0003);
      return i;
}



//      This function enables the Memory Address Pointer specified by the map_num parameter.
//      param   map_num must be: 1 enables MAP1 at register address 0x000B, 
//                               2 enables MAP2 at register address 0x000C, 
//                               3 enables MAP3 at register address 0x000D 
//                            or 4 enables MAP4 at register address 0x000E
//
void enaMAP(unsigned char map_num) {
      // read-modify-write Master Configuration reg, updating bits 11-10
      unsigned short i;
      unsigned short j = (map_num-1) << 10;
      // j = 0 for MAP1, 0x0400 for MAP2, 0x0800 for MAP3, 0x0C00 for MAP4
      i = Read_6131LowReg(MASTER_CONFIG_REG, 1) & ~0x0C00;
      j |= i;
      Write_6131LowReg(MASTER_CONFIG_REG, j, 1);
}
      


//-----------------------------------------------------------------------------
/// Initialization of the ARM Cortex M3 SPI for communication with the HI-6131
//-----------------------------------------------------------------------------

void Configure_ARM_MCU_SPI(void) {
  
    volatile unsigned int uDummy;
      
    // Configure pins
    PIO_Configure(pinsSPI, PIO_LISTSIZE(pinsSPI));
                      
    // write SPI Mode register
    SPI_Configure(AT91C_BASE_SPI0,
                  AT91C_ID_SPI0,
                  ((1 << 0) |   // SPI Master
                   (0 << 1) |   // PS = fixed peripheral select
                   (0 << 2) |   // PSDEC = chip selects directly connect to peripheral devices
                   (1 << 4) |   // mode fault detect disabled
                   (0 << 5) |   // WDRBT = 0 Master can init new transfer whether/not RxD reg has unread data
                   (0 << 7) |   // LLB = 0 Local loopback path disabled
                   (0xE << 16)| // PCS for chip select 0.
                   (0x00 << 24))// DLYBCS delay from NPCS inactive to the activation of another NPCS
    );                          // If DLYBCS < or = 6 then 6 MCK periods are inserted by default.                     


    // write SPI chip select reg for NPCS0
    SPI_ConfigureNPCS(AT91C_BASE_SPI0, 
                      BOARD_6131_NPCS,
                      ((0 << 0)  | // CPOL = SCK inactive state = 0
                       (1 << 1)  | // NCPHA read data SCK leading edge, chg data trailing edge
                       (0 << 2)  | // CSNAAT = 0
                       (0 << 3)  | // CSAAT = 0
                       (0 << 4)  | // BITS = 8 bit transfers
                       (3 << 8)  | // SCBR gives SCK = MCLK/N = 48MHz/3 = 16MHz 
                       (12 << 16)| // DLYBS dly between nCS-SCK = N/MCLK = 12/48 = .25uS
                       (1 << 24) ) // DLYBCT dly between transfers N x 32/MCLK = 32/48 = .67uS
    );
    
    
    SPI_Enable(AT91C_BASE_SPI0);

    for (uDummy=0; uDummy<100000; uDummy++);

    uDummy = AT91C_BASE_SPI0->SPI_SR;
    uDummy = AT91C_BASE_SPI0->SPI_RDR;

}   // end Configure_ARM_MCU_SPI()



/*
//	next function was created specifically to demonstrate a method for
//	SPI interrupt management. The function performs a 32-word sequential
//	read from the top 32 words in HI-6131 RAM (0x7FE0-0x7FFF). The status
//	flags spi_busy and spi_int support controlled recognition of vectored 
//	message interrupts that can break into this foreground sequential access.
//	The memory data pointer is restored and a new op code is issued to
//	allow the foreground 32-word fetch to reach completion. To support
//	debug, the function ends by copying the 32 words read from HI-6131 RAM
//	into ZNEO processor RAM, where the Memory Watch debug window can confirm
//	proper data was read, 0x0101, 0x0202 ... 0x2121 to 0x3232. (set Memory
//	Watch window to start address M#00FFB00 and 4 bytes/row.
//	Data is displayed only when program execution is stopped.
//
//	Function spiTest_setup() should run once, before calling this test...
//	For convenience in testing, any receive command for subaddress 30 will
//	generate an IWA (interrupt when accessed) message interrupt that can
//	break into this process
//
//	IF USING VECTORED INTERRRUPTS WITH HI-6131, ALL N-WORD SEQUENTIAL 
//	READ/WRITE PROCESSES SHOULD USE AN IRQ MANAGEMENT SCHEME LIKE THIS
//
//  param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//	                  if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
///
void spiIRQtest(void) {

	unsigned short i, data, ram_array[32];
	unsigned char number_of_words = 32;

	__disable_interrupt();					// disable interrupts for about 11us, until
						// top of word counting "for loop" below 

	spi_busy = 1;				// global variable tested by vectored IRQ routine 

	// write memory address pointer, point to data in HI-6131 RAM
	// that was written earlier by function spiTest_setup() called in main()  
	Write_6131LowReg(MEM_ADDR_PTR,0x7FE0,0);

	ESPITDCR = 0x01;		// Assert SPI chip select but leave TEOF bit low 
	ESPIDATA = 0x40;		// Send SPI op code to read, w/o mem pointer pre-increment 
	while (!(ESPISTAT & RDRF));	// Wait for RDRF flag (Rx Data Register Full) 
	data = ESPIDATA;		// Read and discard received data char in Rx buffer 


	for (i = 0; i < number_of_words; i++) {

		__enable_interrupt();
		// Before reading the next word, momentarily enable IRQs...
		// A pending IRQ that occurred since last __disable_interrupt() will be recognized here.
		// Its ISR (int.svc routine) will execute, probably using SPI, disrupting 
		// our multi-word transfer. This example program briefly allows interrupts 
		// once every 4us or so while in this "for loop". The interrupt ISR sets 
		// the spi_irq flag, then execution resumes at this next __disable_interrupt() call...   
		__disable_interrupt();

		if(spi_irq) {	// detect IRQ that used SPI

			// the multi-word transfer was disturbed, but the interrupt's
			// ISR should have restored reg 15, the Memory Address Pointer,
			// so it points to the next word to be read. Issue a new SPI op
			// code to resume a multi-word read process, starting at the 
			// RAM location addressed by the restored Mem Address Pointer value. 

			ESPITDCR = 0x01;		// Assert SPI chip select but leave TEOF bit low 
			ESPIDATA = 0x40;		// Send SPI op code: READ = 0x40, WRITE = 0xC0 
			while (!(ESPISTAT & RDRF));	// Wait for RDRF flag (Rx Data Register Full) 
			data = ESPIDATA;		// Read and discard received data char in Rx buffer 
			spi_irq = 0;			// reset global variable set by IRQ 
		}
							// Read the next word 
		ESPIDATA = 0;				// Dummy data for transmit buffer 
		while (!(ESPISTAT & RDRF));		// Wait for RDRF flag (Rx Data Register Full) 	
		ESPIDATA = 0;				// Dummy data for transmit buffer 
		data = ESPIDATA << 8;			// Read upper byte from Rx buffer and left-shift 
		while (!(ESPISTAT & RDRF));		// Wait for RDRF flag (Rx Data Register Full) 
		data += ESPIDATA;			// Read lower byte from Rx buffer, append it to data word 
		ram_array[i] = data;
	}

	ESPITDCR = 0x00;				// Negate SPI chip select 
	spi_busy = 0;					// reset global variable 


	// FOR DEBUG VERIFICATION ONLY, copy the read data array to 
	// internal processor RAM so it can be	viewed in Memory Watch window.
	// Set breakpoint at __enable_interrupt() below, and set watch window start address to 
	// M#00FFB060 to confirm 32-word read data values 0x0101 - 0x3232 
	for (i = 0; i < number_of_words; i++) {
		// first overwrite RAM with 0xFFFF so old data 
		// doesn't mask a read failure in this cycle 
		*(far unsigned short *)(0xFFB060 + (i<<1)) = 0xFFFF;
		// copy read data 
		*(far unsigned short *)(0xFFB060 + (i<<1)) = ram_array[i];
	}


	__enable_interrupt();						// enable interrupts 
}



//
//	special test function loads data to top 32 words in HI-6131 RAM
//
//	here are the top 32 words in the RAM after running function below:
//		==========================================================
//		ADDR	0x7FE0,0x7FE1,0x7FE2,0x7FE3,0x7FE4,0x7FE5,0x7FE6,0x7FE7,
//		DATA 	0x0101,0x0202,0x0303,0x0404,0x0505,0x0606,0x0707,0x0808,
//		==========================================================
//		ADDR	0x7FE8,0x7FE9,0x7FEA,0x7FEB,0x7FEC,0x7FED,0x7FEE,0x7FEF
//		DATA	0x0909,0x1010,0x1111,0x1212,0x1313,0x1414,0x1515,0x1616
//		==========================================================		
// 		ADDR	0x7FF0,0x7FF1,0x7FF2,0x7FF3,0x7FF4,0x7FF5,0x7FF6,0x7FF7,
//		DATA 	0x1717,0x1818,0x1919,0x2020,0x2121,0x2222,0x2323,0x2424,
// 		==========================================================
//		ADDR	0x7FF8,0x7FF9,0x7FFA,0x7FFB,0x7FFC,0x7FFD,0x7FFE,0x7FFF
//		DATA	0x2525,0x2626,0x2727,0x2828,0x2929,0x3030,0x3131,0x3232
//		==========================================================
//  param	irq_mgmt. if zero, the calling routine manages irq enable/disable.
//                        if non-zero, this function calls __disable_interrupt() and __enable_interrupt() locally.
///
void spiTest_setup(void) {

	unsigned short num_words=32;

	unsigned short spi_write_data[32] = {0x0101,0x0202,0x0303,0x0404,
					0x0505,0x0606,0x0707,0x0808,
					0x0909,0x1010,0x1111,0x1212,
					0x1313,0x1414,0x1515,0x1616,
					0x1717,0x1818,0x1919,0x2020,
					0x2121,0x2222,0x2323,0x2424,
					0x2525,0x2626,0x2727,0x2828,
					0x2929,0x3030,0x3131,0x3232};

	// disable interrupts, manage IRQs at this level, 
	// no  EI/DI switching by the called SPI functions 
	__disable_interrupt();

	// write addr 0x7FE0 to mem addr pointer 
	Write_6131LowReg(MEM_ADDR_PTR,0x7FE0,0); 	
	
	// next write N consec words from spi_write_data[] array 
	Write_6131(spi_write_data,num_words,0,0);

	// enable interrupts 
	__enable_interrupt();
}

*/


// End of file 
