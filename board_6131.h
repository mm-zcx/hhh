/* ----------------------------------------------------------------------------
 *                            HOLT Integrated Circuits 
 * ----------------------------------------------------------------------------
 *
 *    file	board_6131.h
 *    brief     This file contains ARM Cortex M3 hardware definitions for 
 *              initializing the processor SPI for Holt HI-6130 projects. 
 *              (Not used for HI-6130 projects.)
 *
 *		IMPORTANT: Edit file 613x_initialization.h to define either:
 *
 *		HOST_BUS_INTERFACE  YES  sets compilation for HI-6130 (bus)
 *		 		    NO   sets compilation for HI-6131 (SPI)
 *
 *		Most functional procedures in this project are written for
 *		both parallel bus or SPI host interface schemes, this runtime 
 *		switch determines which are procedures are compiled.
 *
 *              IMPORTANT: Because register addressing differs for Bus and 
 *              SPI interface, files "device_6130.h" and "device_6131.h" 
 *              contain the definitions needed for register addressing.
 *
 *	   	HOLT DISCLAIMER
 *      	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
 *      	KIND, EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 *      	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 *      	PURPOSE AND NONINFRINGEMENT. 
 *      	IN NO EVENT SHALL HOLT, INC BE LIABLE FOR ANY CLAIM, DAMAGES
 *      	OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 *      	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 *      	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 *
 *              Copyright (C) 2009-2011 by  HOLT, Inc.
 *              All Rights Reserved
 */

#include "613x_initialization.h"

//------------------------------------------------------------------------------
//               Atmel ARM Cortex M3 SPI Hardware Definitions
//------------------------------------------------------------------------------


/// The SPI (Serial Peripheral Interface) is used only with HI-6131, not HI-6130.
/// For the Holt Evaluation Board, this section replaces SPI definitions found in 
/// Atmel file board.h which references their SAM3U Evaluation Board. Names were 
/// changed so both files can coexist in IAR example project structure, and redefined
/// so the Holt files do not affect operation of the Atmel example projects. 
///
/// The original Atmel pin names:
/// PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SPCK, PIN_SPI0_NPCS2_PC14, PINS_SPI0.
///
/// Holt revised the definitions in the original Atmel board.h file:
/// (1) New names to avoid duplicate defs. 
/// (2) Using NPCS0 (PA16) not NPCS2 (PC14)


/// only MCU SPI0 peripheral is used.

#define PIN_SPI_MISO           {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI_MOSI           {1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI_SCK            {1 << 15, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI_NPCS0_PA16     {1 <<  16, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_PULLUP}
//#define PIN_SPI_NPCS0_PA16   {1 <<  16, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// List of SPI pin definitions (MISO, MOSI & SCK)
#define PINS_SPI               PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SCK

/// Base address of SPI peripheral connected to the HI-6131 SPI port.
#define BOARD_6131_SPI_BASE         AT91C_BASE_SPI0

/// Identifier of SPI peripheral connected to the HI-6131 SPI port.
#define BOARD_6131_SPI_ID           AT91C_ID_SPI0

/// Pins of the SPI peripheral connected to the HI-6131 SPI port.
#define BOARD_6131_SPI_PINS         PINS_SPI

/// Chip select connected to the HI-6131 SPI port.
#define BOARD_6131_NPCS             0 

/// Chip select pin connected to the HI-6131 SPI port.
#define BOARD_6131_NPCS_PIN         PIN_SPI_NPCS0_PA16

// speed up chip select, assert/negate by direct addressing
#define SPI_nCS	1<<16


//------------------------------------------------------------------------------
//    THE FOLLOWING MACROS IN THIS FILE ONLY APPLY TO SPI-INTERFACE HI-6131.    
//
//   THE SELECTION OF BUS OR SPI INTERFACE IS SET IN FILE 613X_INITIALIZATION.H 
//------------------------------------------------------------------------------

// Macros for using the SPIopcode(macro) command

// example 1: SPIopcode(enMAP4) enables Memory Address Pointer 4
//            or a enaMAP(4) function call does the same thing

#define enMAP1     0xD8     //  Enable Memory Address Pointer 1 (MAP1) reg address 0x000B
#define enMAP2     0xD9     //  Enable Memory Address Pointer 2 (MAP2) reg address 0x000C
#define enMAP3     0xDA     //  Enable Memory Address Pointer 3 (MAP3) reg address 0x000D
#define enMAP4     0xDB     //  Enable Memory Address Pointer 4 (MAP4) reg address 0x000E

// example 2: SPIopcode(MAPadd2) adds 2 to the value in the currently-enabled Memory Address Pointer

#define MAPadd1    0xD0     //  Add 1 to currently-enabled Memory Address Pointer value
#define MAPadd2    0xD2     //  Add 2 to currently-enabled Memory Address Pointer value
#define MAPadd4    0xD4     //  Add 4 to currently-enabled Memory Address Pointer value





//------------------------------------------------------------------------------
//      Global Function Prototypes
//------------------------------------------------------------------------------

void SPIopcode( unsigned char opcode) ;
unsigned char Write_6131LowReg(unsigned char reg_number, unsigned short data, unsigned char irq_mgmt) ;
unsigned short Read_6131LowReg(unsigned char reg_number, unsigned char irq_mgmt) ;
void Write_6131_1word(unsigned short data, unsigned char irq_mgmt) ;
unsigned short Read_6131_1word(unsigned char irq_mgmt) ;
void Write_6131(unsigned short write_data[], unsigned char inc_pointer_first, unsigned char irq_mgmt) ;
//void Read_6131(unsigned short number_of_words, unsigned char irq_mgmt) ;
unsigned char Write_6131_Buffer(unsigned short write_data[], unsigned char inc_pointer_first, unsigned char irq_mgmt) ;
unsigned char Read_6131_Buffer(unsigned short number_of_words, unsigned char inc_pointer_first, unsigned char irq_mgmt) ;
unsigned short Read_Current_Control_Word(unsigned char rt_num, unsigned char irq_mgmt) ;
unsigned short getMAPaddr(void) ;
void enaMAP(unsigned char map_num) ;
unsigned short Read_Current_Control_Word(unsigned char rt_num, unsigned char irq_mgmt);
unsigned short Read_RT1_Control_Word(unsigned char txrx, unsigned char samc, unsigned char number, unsigned char irq_mgmt);
unsigned short Read_RT2_Control_Word(unsigned char txrx, unsigned char samc, unsigned char number, unsigned char irq_mgmt);
unsigned short ReadWord_Adv4(unsigned char irq_mgmt) ;
unsigned short Read_Last_Interrupt(unsigned char irq_mgmt) ;
void Fill_6131RAM_Offset(void) ;
void Fill_6131RAM(unsigned short addr, unsigned short num_words, unsigned short fill_value) ;
void Memory_watch(unsigned short address);
void Configure_ARM_MCU_SPI(void);
void Read_6131(unsigned short address, unsigned short number_of_words);


// end of file

