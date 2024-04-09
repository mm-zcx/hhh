/* ----------------------------------------------------------------------------
 *                            HOLT Integrated Circuits 
 * ----------------------------------------------------------------------------
 *
 *    file	device_6131.h
 *    brief     For projects using the Holt HI-6131 device with SPI interface, 
 *              this file contains definitions applicable to all terminal modes
 *              (BC, RT, SMT, IMT). (Does not apply for SPI HI-6130 projects.)
 *              File includes definitions for HI-6131 register addressing.
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
 *              SPI interface, file "device_6130.h" contains definitions needed 
 *              for register addressing and functions for using the bus HI-6130.
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





//------------------------------------------------------------------------------
//               Macro Definitions for HI-6131 Register Addresses
//------------------------------------------------------------------------------

//-----------------------------------------------------
//	Registers Used by All Modes: BC, RT, SMT, IMT
//-----------------------------------------------------
	// Fast Access read/write registers 
	#define MASTER_CONFIG_REG		0x00	// Master Configuration Register 
	#define STATUS_AND_RESET_REG		0x01	// Master Status and Reset Register 
        #define MAP_1				0x0B	// Memory Addr Pointer #1
        #define MAP_2				0x0C	// Memory Addr Pointer #2
        #define MAP_3   			0x0D	// Memory Addr Pointer #3
        #define MAP_4				0x0E	// Memory Addr Pointer #4

	// Fast Access write but read using any Memory Addr Pointer 
	#define TTAG_CONFIG_REG			0x39	// Time Tag Counters Configuration Register 

	// Read and write using any Memory Addr Pointer 
	#define TEST_CONTROL_REG		0x4D	// Test Control Register 
	#define WRITE_EEPROM_UNLOCK_REG		0x4E	// Write EEPROM Unlock Register 
	#define CHKSUM_FAIL_ADDR_REG		0x4E	// Checksum Fail Address Register 

//-----------------------------------------------------
//	Interrupt Management Registers 
//-----------------------------------------------------
	// all are Fast Access read/write registers 
	#define INT_COUNT_AND_LOG_ADDR_REG	0x0A	// Interrupt Count & Int Log Addr Pointer 
	#define HDW_PENDING_INT_REG		0x06	// Hardware Pending Interrupt Register 
	#define BC_PENDING_INT_REG		0x07	// Bus Controller Pending Interrupt Register 
	#define MT_PENDING_INT_REG		0x08	// Bus Monitor Pending Interrupt Register 
	#define RT_PENDING_INT_REG		0x09	// Remote Terminal RT1-2 Pending Interrupt Register 

	#define HDW_INT_ENABLE_REG 		0x0F	// Hardware Interrupt Enable Register 
	#define HDW_INT_OUTPUT_ENABLE_REG 	0x13	// Hardware Interrupt Output Enable Register 

	#define BC_INT_ENABLE_REG		0x10	// Bus Controller Interrupt Enable Register 
	#define BC_INT_OUTPUT_ENABLE_REG	0x14	// Bus Controller Interrupt Output Enable Register 

	#define MT_INT_ENABLE_REG		0x11	// Bus Monitor Interrupt Enable Register 
	#define MT_INT_OUTPUT_ENABLE_REG	0x15	// Bus Monitor Interrupt Output Enable Register 

	#define RT_INT_ENABLE_REG		0x12	// Remote Terminal RT1-2 Interrupt Enable Register 
	#define RT_INT_OUTPUT_ENABLE_REG	0x16	// Remote Terminal RT1-2 Interrupt Output Enable Register 

//-----------------------------------------------------
//	Remote Terminal 1 Registers
//-----------------------------------------------------
	// Fast Access read/write registers 
	#define RT1_CURR_CMD_REG		0x02	// RT1 Current Command Register 
	#define RT1_CURR_CTRL_WORD_ADDR_REG	0x03	// RT1 Current Control Word Address Register 

	// Fast Access write but read using any Memory Addr Pointer 
	#define RT1_CONFIG_REG			0x17	// RT1 Configuration Register 
	#define RT1_OP_STATUS_REG		0x18	// RT1 Operational Status Register 
	#define RT1_DESC_TBL_BASE_ADDR_REG	0x19	// RT1 Descriptor Table Base Address Register 
	#define RT1_1553_STATUS_BITS_REG	0x1A	// RT1 1553 Status Word Bits Register 
	#define RT1_MSG_INFO_WD_ADDR_REG	0x1B	// RT1 Current Message Information Word Register 
	#define RT1_BUSA_SELECT_REG		0x1C	// RT1 Bus A Select Register for mode cmds 20-21
	#define RT1_BUSB_SELECT_REG		0x1D	// RT1 Bus B Select Register for mode cmds 20-21 
	#define RT1_BIT_WORD_REG		0x1E	// RT1 Built-In Test (BIT) Word Register 
	#define RT1_ALT_BIT_WORD_REG		0x1F	// RT1 Alternate Built-In Test (BIT) Word Register 

	// Read and write using any Memory Addr Pointer 
	#define RT1_TTAG_COUNT			0x49	// Remote Terminal 1, Time Tag Count 
	#define RT1_TTAG_UTILITY_REG		0x4A	// Remote Terminal 1, Time Tag Utility Register 

//-----------------------------------------------------
//	Remote Terminal 2 Registers
//-----------------------------------------------------
	// Fast Access read/write registers 
	#define RT2_CURR_CMD_REG		0x04	// RT2 Current Command Register 
	#define RT2_CURR_CTRL_WORD_ADDR_REG	0x05	// RT2 Current Control Word Address Register 

	// Fast Access write but read using any Memory Addr Pointer 
	#define RT2_CONFIG_REG			0x20	// RT2 Configuration Register 
	#define RT2_OP_STATUS_REG		0x21	// RT2 Operational Status Register 
	#define RT2_DESC_TBL_BASE_ADDR_REG	0x22	// RT2 Descriptor Table Base Address Register 
	#define RT2_1553_STATUS_BITS_REG	0x23	// RT2 1553 Status Word Bits Register 
	#define RT2_MSG_INFO_WD_ADDR_REG	0x24	// RT2 Current Message Information Word Register 
	#define RT2_BUSA_SELECT_REG		0x25	// RT2 Bus A Select Register for mode cmds 20-21 
	#define RT2_BUSB_SELECT_REG		0x26	// RT2 Bus B Select Register for mode cmds 20-21 
	#define RT2_BIT_WORD_REG		0x27	// RT2 Built-In Test (BIT) Word Register 
	#define RT2_ALT_BIT_WORD_REG		0x28	// RT2 Alternate Built-In Test (BIT) Word Register 

	// Read and write using any Memory Addr Pointer 
	#define RT2_TTAG_COUNT			0x4B	// Remote Terminal 2, Time Tag Count 
	#define RT2_TTAG_UTILITY_REG		0x4C	// Remote Terminal 2, Time Tag Utility Register 

//-----------------------------------------------------
//	Bus Monitor Registers
//-----------------------------------------------------
	// Fast Access read/write registers 
        //      none

	// Fast Access write but read using any Memory Addr Pointer 
	#define MT_CONFIG_REG			0x29	// IRIG-106 or Simple Bus Monitor: Configuration Register 
	#define MT_ADDR_LIST_POINTER		0x2F	// IRIG-106 or Simple Monitor: Address List Pointer 
	#define MT_NEXT_MSG_STACK_ADDR_REG	0x30	// IRIG-106 or Simple Monitor: Next Msg Stack Addr Reg 
	#define MT_LAST_MSG_STACK_ADDR_REG	0x31	// IRIG-106 or Simple Monitor: Last Msg Stack Addr Reg 
	#define MT_TTAG_COUNT_LOW		0x3A	// IRIG-106 or Simple MT, Time Tag Count, Low 16 Bits 
	#define MT_TTAG_COUNT_MID		0x3B	// IRIG-106 or Simple MT, Time Tag Count, Mid 16 Bits 
	#define MT_TTAG_COUNT_HIGH		0x3C	// IRIG-106 or Simple MT, Time Tag Count, High 16 Bits 
	#define MT_TTAG_UTILITY_REG_LOW		0x3D	// IRIG-106 or Simple MT, Time Tag Utility, Low 16 Bits 
	#define MT_TTAG_UTILITY_REG_MID		0x3E	// IRIG-106 or Simple MT, Time Tag Utility, Mid 16 Bits 
	#define MT_TTAG_UTILITY_REG_HIGH	0x3F	// IRIG-106 or Simple MT, Time Tag Utility, High 16 Bits 
	#define IMT_MAX_1553_MSGS		0x2A	// IRIG-106 Monitor Only: Packet Max Msg Count Reg 
	#define IMT_MAX_1553_WORDS		0x2B	// IRIG-106 Monitor Only: Packet Max Word Count Reg 
	#define IMT_MAX_PKT_TIME		0x2C	// IRIG-106 Monitor Only: Max Packet Time Reg 
	#define IMT_MAX_GAP_TIME		0x2D	// IRIG-106 Monitor Only: Max Intermessage Gap Time Reg 
	#define IMT_CHANNEL_ID			0x2E	// IRIG-106 Monitor Only: Packet Header Channel ID Reg 

	// Read and write using any Memory Addr Pointer 
	#define MT_TTAG_MATCH_LOW		0x40	// IRIG-106 or Simple MT, Time Tag Match, Low 16 Bits 
	#define MT_TTAG_MATCH_MID		0x41	// IRIG-106 or Simple MT, Time Tag Match, Mid 16 Bits 
	#define MT_TTAG_MATCH_HIGH		0x42	// IRIG-106 or Simple MT, Time Tag Match, High 16 Bits 

//-----------------------------------------------------
//	Bus Controller Registers
//-----------------------------------------------------
	// Fast Access read/write registers 
        //      none

	// Fast Access write but read using any Memory Addr Pointer 
	#define BC_CONFIG_REG			0x32	// Bus Controller Configuration Register 
	#define BC_INST_LIST_BASE_ADDR_REG	0x33	// Bus Controller Inst List Start Address Register 
	#define BC_INST_LIST_POINTER		0x34	// Bus Controller Inst List Pointer 
	#define BC_FRAME_TIME_LEFT_REG		0x35	// Bus Controller Frame Time Remaining Register 
	#define BC_TIME_TO_NEXT_MSG_REG		0x36	// Bus Controller Time to Next Message Register 
	#define BC_CCODE_AND_GPF_REG		0x37	// Bus Controller Cond Code and GP Flag Register 
	#define BC_GP_QUEUE_POINTER		0x38	// Bus Controller General Purpose Queue Pointer 

	// Read and write using any Memory Addr Pointer 
	#define BC_TTAG_COUNT_LOW		0x43	// Bus Controller, Time Tag Count, Low 16 Bits 
	#define BC_TTAG_COUNT_HIGH		0x44	// Bus Controller, Time Tag Count, High 16 Bits 
	#define BC_TTAG_UTILITY_REG_LOW		0x45	// Bus Controller, Time Tag Utility, Low 16 Bits 
	#define BC_TTAG_UTILITY_REG_HIGH	0x46	// Bus Controller, Time Tag Utility, High 16 Bits 
	#define BC_TTAG_MATCH_LOW		0x47	// Bus Controller, Time Tag Match, Low 16 Bits 
	#define BC_TTAG_MATCH_HIGH		0x48	// Bus Controller, Time Tag Match, High 16 Bits 
	#define BC_LAST_MSG_BLOCK_ADDR_REG	0x4F	// Bus Controller, Last Message Block Address Register 
	

	#define HI613x_RAM_START_ADDR  		0x51 


// end of file













