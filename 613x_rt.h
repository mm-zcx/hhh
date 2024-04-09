/* ----------------------------------------------------------------------------
 *                            HOLT Integrated Circuits 
 * ----------------------------------------------------------------------------
 *
 *    file	613x_rt.h
 *    brief     This file contains prototype functions and
 * 	        definitions used by functions in 613x_rt.c file. 
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

//------------------------------------------------------------------------
//                            Macro definitions
//------------------------------------------------------------------------

// These tables are relocatable...
#define RT1_ILLEGAL_TABLE_BASE_ADDR 0x200 // RT1 default illegalization table addr range 0x0200 to 0x02FF
#define RT2_ILLEGAL_TABLE_BASE_ADDR 0x300 // RT2 default illegalization table addr range 0x0300 to 0x03FF
#define RT1_DESCRIP_TABLE_BASE_ADDR 0x400 // RT1 default descriptor table addr range 0x0400 to 0x05FF
#define RT2_DESCRIP_TABLE_BASE_ADDR 0x600 // RT2 default descriptor table addr range 0x0600 to 0x07FF


//--------------------------------------------------------------------------------------
//
//
//   ( H O S T   S P I   I N T E R F A C E   H I - 6 1 3 1   S E C T I O N   B E L O W )
//
//---------------------------------------------------------------------------------------

// Macros for HI-6131 RAM addressing via the host SPI interface 


//================================================================
//                             RT1 

	/* RT1 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR RECEIVE SUBDDRESS COMMANDS
	  Note: SA0 and SA31 are not used. These 2
	   SA values indicate mode code commands. */
	#define RT1_CtrlWord_RxSA1	0x0404	
	#define RT1_CtrlWord_RxSA2	0x0408	 
	#define RT1_CtrlWord_RxSA3	0x040C	
	#define RT1_CtrlWord_RxSA4	0x0410	
	#define RT1_CtrlWord_RxSA5	0x0414	
	#define RT1_CtrlWord_RxSA6	0x0418	
	#define RT1_CtrlWord_RxSA7	0x041C	
	#define RT1_CtrlWord_RxSA8	0x0420	
	#define RT1_CtrlWord_RxSA9	0x0424	
	#define RT1_CtrlWord_RxSA10	0x0428	
	#define RT1_CtrlWord_RxSA11	0x042C	
	#define RT1_CtrlWord_RxSA12	0x0430	
	#define RT1_CtrlWord_RxSA13	0x0434
	#define RT1_CtrlWord_RxSA14	0x0438
	#define RT1_CtrlWord_RxSA15	0x043C
	#define RT1_CtrlWord_RxSA16	0x0440
	#define RT1_CtrlWord_RxSA17	0x0444
	#define RT1_CtrlWord_RxSA18	0x0448
	#define RT1_CtrlWord_RxSA19	0x044C
	#define RT1_CtrlWord_RxSA20	0x0450
	#define RT1_CtrlWord_RxSA21	0x0454
	#define RT1_CtrlWord_RxSA22	0x0458
	#define RT1_CtrlWord_RxSA23	0x045C
	#define RT1_CtrlWord_RxSA24	0x0460
	#define RT1_CtrlWord_RxSA25	0x0464	
	#define RT1_CtrlWord_RxSA26	0x0468	
	#define RT1_CtrlWord_RxSA27	0x046C	
	#define RT1_CtrlWord_RxSA28	0x0470	
	#define RT1_CtrlWord_RxSA29	0x0474	
	#define RT1_CtrlWord_RxSA30	0x0478	

	/* RT1 ESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	       FOR TRANSMIT SUBDDRESS COMMANDS
	  Note: SA0 and SA31 are not used. These 2
	   SA values indicate mode code commands. */
	#define RT1_CtrlWord_TxSA1	0x0484	
	#define RT1_CtrlWord_TxSA2	0x0488	 
	#define RT1_CtrlWord_TxSA3	0x048C	
	#define RT1_CtrlWord_TxSA4	0x0490	
	#define RT1_CtrlWord_TxSA5	0x0494	
	#define RT1_CtrlWord_TxSA6	0x0498	
	#define RT1_CtrlWord_TxSA7	0x049C	
	#define RT1_CtrlWord_TxSA8	0x04A0	
	#define RT1_CtrlWord_TxSA9	0x04A4	
	#define RT1_CtrlWord_TxSA10	0x04A8	
	#define RT1_CtrlWord_TxSA11	0x04AC	
	#define RT1_CtrlWord_TxSA12	0x04B0	
	#define RT1_CtrlWord_TxSA13	0x04B4	
	#define RT1_CtrlWord_TxSA14	0x04B8	
	#define RT1_CtrlWord_TxSA15	0x04BC	
	#define RT1_CtrlWord_TxSA16	0x04C0	
	#define RT1_CtrlWord_TxSA17	0x04C4	
	#define RT1_CtrlWord_TxSA18	0x04C8	
	#define RT1_CtrlWord_TxSA19	0x04CC	
	#define RT1_CtrlWord_TxSA20	0x04D0	
	#define RT1_CtrlWord_TxSA21	0x04D4	
	#define RT1_CtrlWord_TxSA22	0x04D8	
	#define RT1_CtrlWord_TxSA23	0x04DC	
	#define RT1_CtrlWord_TxSA24	0x04E0	
	#define RT1_CtrlWord_TxSA25	0x04E4	
	#define RT1_CtrlWord_TxSA26	0x04E8	
	#define RT1_CtrlWord_TxSA27	0x04EC	
	#define RT1_CtrlWord_TxSA28	0x04F0	
	#define RT1_CtrlWord_TxSA29	0x04F4	
	#define RT1_CtrlWord_TxSA30	0x04F8	

	/* RT1 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR RECEIVE MODE CODE COMMANDS
	  These apply when the Command Word Tx/Rx bit = 0 
	  and Command Word 5-bit subaddress field = 00000 or 11111.
      Note: Just 3 Receive Mode Code Commands are defined; these
	  are noted below. All others are undefined or reserved.  */
	#define RT1_CtrlWord_RxMC0	0x0500	
	#define RT1_CtrlWord_RxMC1	0x0504	
	#define RT1_CtrlWord_RxMC2	0x0508	 
	#define RT1_CtrlWord_RxMC3	0x050C	
	#define RT1_CtrlWord_RxMC4	0x0510	
	#define RT1_CtrlWord_RxMC5	0x0514	
	#define RT1_CtrlWord_RxMC6	0x0518	
	#define RT1_CtrlWord_RxMC7	0x051C	
	#define RT1_CtrlWord_RxMC8	0x0520	
	#define RT1_CtrlWord_RxMC9	0x0524	
	#define RT1_CtrlWord_RxMC10	0x0528	
	#define RT1_CtrlWord_RxMC11	0x052C	
	#define RT1_CtrlWord_RxMC12	0x0530	
	#define RT1_CtrlWord_RxMC13	0x0534	
	#define RT1_CtrlWord_RxMC14	0x0538	
	#define RT1_CtrlWord_RxMC15	0x053C	
	#define RT1_CtrlWord_RxMC16	0x0540	
	#define RT1_CtrlWord_RxMC17	0x0544	// Defined 
	#define RT1_CtrlWord_RxMC18	0x0548	
	#define RT1_CtrlWord_RxMC19	0x054C	
	#define RT1_CtrlWord_RxMC20	0x0550	// Defined 
	#define RT1_CtrlWord_RxMC21	0x0554	// Defined 
	#define RT1_CtrlWord_RxMC22	0x0558	
	#define RT1_CtrlWord_RxMC23	0x055C	
	#define RT1_CtrlWord_RxMC24	0x0560	
	#define RT1_CtrlWord_RxMC25	0x0564	
	#define RT1_CtrlWord_RxMC26	0x0568	
	#define RT1_CtrlWord_RxMC27	0x056C	
	#define RT1_CtrlWord_RxMC28	0x0570	
	#define RT1_CtrlWord_RxMC29	0x0574	
	#define RT1_CtrlWord_RxMC30	0x0578	
	#define RT1_CtrlWord_RxMC31	0x057C	
	
	/*   RT1 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR TRANSMIT MODE CODE COMMANDS
	  These apply when the Command Word Tx/Rx bit = 1
	  and Command Word 5-bit subaddress field = 00000 or 11111.
      Note: The defined Transmit Mode Codes are indicated; 
	  all other transmit mode codes are undefined or reserved. */
	#define RT1_CtrlWord_TxMC0	0x0580	// Defined 
	#define RT1_CtrlWord_TxMC1	0x0584	// Defined 
	#define RT1_CtrlWord_TxMC2	0x0588	// Defined 
	#define RT1_CtrlWord_TxMC3	0x058C	// Defined 
	#define RT1_CtrlWord_TxMC4	0x0590	// Defined 
	#define RT1_CtrlWord_TxMC5	0x0594	// Defined 
	#define RT1_CtrlWord_TxMC6	0x0598	// Defined 
	#define RT1_CtrlWord_TxMC7	0x059C	// Defined 
	#define RT1_CtrlWord_TxMC8	0x05A0	// Defined 
	#define RT1_CtrlWord_TxMC9	0x05A4	
	#define RT1_CtrlWord_TxMC10	0x05A8	
	#define RT1_CtrlWord_TxMC11	0x05AC	
	#define RT1_CtrlWord_TxMC12	0x05B0	
	#define RT1_CtrlWord_TxMC13	0x05B4	
	#define RT1_CtrlWord_TxMC14	0x05B8	
	#define RT1_CtrlWord_TxMC15	0x05BC	
	#define RT1_CtrlWord_TxMC16	0x05C0	// Defined 
	#define RT1_CtrlWord_TxMC17	0x05C4	
	#define RT1_CtrlWord_TxMC18	0x05C8	// Defined 
	#define RT1_CtrlWord_TxMC19	0x05CC	// Defined 
	#define RT1_CtrlWord_TxMC20	0x05D0	
	#define RT1_CtrlWord_TxMC21	0x05D4	
	#define RT1_CtrlWord_TxMC22	0x05D8	
	#define RT1_CtrlWord_TxMC23	0x05DC	
	#define RT1_CtrlWord_TxMC24	0x05E0	
	#define RT1_CtrlWord_TxMC25	0x05E4	
	#define RT1_CtrlWord_TxMC26	0x05E8	
	#define RT1_CtrlWord_TxMC27	0x05EC	
	#define RT1_CtrlWord_TxMC28	0x05F0	
	#define RT1_CtrlWord_TxMC29	0x05F4	
	#define RT1_CtrlWord_TxMC30	0x05F8	
	#define RT1_CtrlWord_TxMC31	0x05FC


//================================================================
//                             RT2 


	/* RT2 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR RECEIVE SUBDDRESS COMMANDS
	  Note: SA0 and SA31 are not used. These 2
	   SA values indicate mode code commands. */
	#define RT2_CtrlWord_RxSA1	0x0604	
	#define RT2_CtrlWord_RxSA2	0x0608	 
	#define RT2_CtrlWord_RxSA3	0x060C	
	#define RT2_CtrlWord_RxSA4	0x0610	
	#define RT2_CtrlWord_RxSA5	0x0614	
	#define RT2_CtrlWord_RxSA6	0x0618	
	#define RT2_CtrlWord_RxSA7	0x061C	
	#define RT2_CtrlWord_RxSA8	0x0620	
	#define RT2_CtrlWord_RxSA9	0x0624	
	#define RT2_CtrlWord_RxSA10	0x0628	
	#define RT2_CtrlWord_RxSA11	0x062C	
	#define RT2_CtrlWord_RxSA12	0x0630	
	#define RT2_CtrlWord_RxSA13	0x0634
	#define RT2_CtrlWord_RxSA14	0x0638
	#define RT2_CtrlWord_RxSA15	0x063C
	#define RT2_CtrlWord_RxSA16	0x0640
	#define RT2_CtrlWord_RxSA17	0x0644
	#define RT2_CtrlWord_RxSA18	0x0648
	#define RT2_CtrlWord_RxSA19	0x064C
	#define RT2_CtrlWord_RxSA20	0x0650
	#define RT2_CtrlWord_RxSA21	0x0654
	#define RT2_CtrlWord_RxSA22	0x0658
	#define RT2_CtrlWord_RxSA23	0x065C
	#define RT2_CtrlWord_RxSA24	0x0660
	#define RT2_CtrlWord_RxSA25	0x0664	
	#define RT2_CtrlWord_RxSA26	0x0668	
	#define RT2_CtrlWord_RxSA27	0x066C	
	#define RT2_CtrlWord_RxSA28	0x0670	
	#define RT2_CtrlWord_RxSA29	0x0674	
	#define RT2_CtrlWord_RxSA30	0x0678	

	/* RT2 ESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	       FOR TRANSMIT SUBDDRESS COMMANDS
	  Note: SA0 and SA31 are not used. These 2
	   SA values indicate mode code commands. */
	#define RT2_CtrlWord_TxSA1	0x0684	
	#define RT2_CtrlWord_TxSA2	0x0688	 
	#define RT2_CtrlWord_TxSA3	0x068C	
	#define RT2_CtrlWord_TxSA4	0x0690	
	#define RT2_CtrlWord_TxSA5	0x0694	
	#define RT2_CtrlWord_TxSA6	0x0698	
	#define RT2_CtrlWord_TxSA7	0x069C	
	#define RT2_CtrlWord_TxSA8	0x06A0	
	#define RT2_CtrlWord_TxSA9	0x06A4	
	#define RT2_CtrlWord_TxSA10	0x06A8	
	#define RT2_CtrlWord_TxSA11	0x06AC	
	#define RT2_CtrlWord_TxSA12	0x06B0	
	#define RT2_CtrlWord_TxSA13	0x06B4	
	#define RT2_CtrlWord_TxSA14	0x06B8	
	#define RT2_CtrlWord_TxSA15	0x06BC	
	#define RT2_CtrlWord_TxSA16	0x06C0	
	#define RT2_CtrlWord_TxSA17	0x06C4	
	#define RT2_CtrlWord_TxSA18	0x06C8	
	#define RT2_CtrlWord_TxSA19	0x06CC	
	#define RT2_CtrlWord_TxSA20	0x06D0	
	#define RT2_CtrlWord_TxSA21	0x06D4	
	#define RT2_CtrlWord_TxSA22	0x06D8	
	#define RT2_CtrlWord_TxSA23	0x06DC	
	#define RT2_CtrlWord_TxSA24	0x06E0	
	#define RT2_CtrlWord_TxSA25	0x06E4	
	#define RT2_CtrlWord_TxSA26	0x06E8	
	#define RT2_CtrlWord_TxSA27	0x06EC	
	#define RT2_CtrlWord_TxSA28	0x06F0	
	#define RT2_CtrlWord_TxSA29	0x06F4	
	#define RT2_CtrlWord_TxSA30	0x06F8	

	/* RT2 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR RECEIVE MODE CODE COMMANDS
	  These apply when the Command Word Tx/Rx bit = 0 
	  and Command Word 5-bit subaddress field = 00000 or 11111.
      Note: Just 3 Receive Mode Code Commands are defined; these
	  are noted below. All others are undefined or reserved.  */
	#define RT2_CtrlWord_RxMC0	0x0700	
	#define RT2_CtrlWord_RxMC1	0x0704	
	#define RT2_CtrlWord_RxMC2	0x0708	 
	#define RT2_CtrlWord_RxMC3	0x070C	
	#define RT2_CtrlWord_RxMC4	0x0710	
	#define RT2_CtrlWord_RxMC5	0x0714	
	#define RT2_CtrlWord_RxMC6	0x0718	
	#define RT2_CtrlWord_RxMC7	0x071C	
	#define RT2_CtrlWord_RxMC8	0x0720	
	#define RT2_CtrlWord_RxMC9	0x0724	
	#define RT2_CtrlWord_RxMC10	0x0728	
	#define RT2_CtrlWord_RxMC11	0x072C	
	#define RT2_CtrlWord_RxMC12	0x0730	
	#define RT2_CtrlWord_RxMC13	0x0734	
	#define RT2_CtrlWord_RxMC14	0x0738	
	#define RT2_CtrlWord_RxMC15	0x073C	
	#define RT2_CtrlWord_RxMC16	0x0740	
	#define RT2_CtrlWord_RxMC17	0x0744	// Defined 
	#define RT2_CtrlWord_RxMC18	0x0748	
	#define RT2_CtrlWord_RxMC19	0x074C	
	#define RT2_CtrlWord_RxMC20	0x0750	// Defined 
	#define RT2_CtrlWord_RxMC21	0x0754	// Defined 
	#define RT2_CtrlWord_RxMC22	0x0758	
	#define RT2_CtrlWord_RxMC23	0x075C	
	#define RT2_CtrlWord_RxMC24	0x0760	
	#define RT2_CtrlWord_RxMC25	0x0764	
	#define RT2_CtrlWord_RxMC26	0x0768	
	#define RT2_CtrlWord_RxMC27	0x076C	
	#define RT2_CtrlWord_RxMC28	0x0770	
	#define RT2_CtrlWord_RxMC29	0x0774	
	#define RT2_CtrlWord_RxMC30	0x0778	
	#define RT2_CtrlWord_RxMC31	0x077C	
	
	/*   RT2 DESCRIPTOR TABLE CONTROL WORD ADDRESSES  
	        FOR TRANSMIT MODE CODE COMMANDS
	  These apply when the Command Word Tx/Rx bit = 1
	  and Command Word 5-bit subaddress field = 00000 or 11111.
      Note: The defined Transmit Mode Codes are indicated; 
	  all other transmit mode codes are undefined or reserved. */
	#define RT2_CtrlWord_TxMC0	0x0780	// Defined 
	#define RT2_CtrlWord_TxMC1	0x0784	// Defined 
	#define RT2_CtrlWord_TxMC2	0x0788	// Defined 
	#define RT2_CtrlWord_TxMC3	0x078C	// Defined 
	#define RT2_CtrlWord_TxMC4	0x0790	// Defined 
	#define RT2_CtrlWord_TxMC5	0x0794	// Defined 
	#define RT2_CtrlWord_TxMC6	0x0798	// Defined 
	#define RT2_CtrlWord_TxMC7	0x079C	// Defined 
	#define RT2_CtrlWord_TxMC8	0x07A0	// Defined 
	#define RT2_CtrlWord_TxMC9	0x07A4	
	#define RT2_CtrlWord_TxMC10	0x07A8	
	#define RT2_CtrlWord_TxMC11	0x07AC	
	#define RT2_CtrlWord_TxMC12	0x07B0	
	#define RT2_CtrlWord_TxMC13	0x07B4	
	#define RT2_CtrlWord_TxMC14	0x07B8	
	#define RT2_CtrlWord_TxMC15	0x07BC	
	#define RT2_CtrlWord_TxMC16	0x07C0	// Defined 
	#define RT2_CtrlWord_TxMC17	0x07C4	
	#define RT2_CtrlWord_TxMC18	0x07C8	// Defined 
	#define RT2_CtrlWord_TxMC19	0x07CC	// Defined 
	#define RT2_CtrlWord_TxMC20	0x07D0	
	#define RT2_CtrlWord_TxMC21	0x07D4	
	#define RT2_CtrlWord_TxMC22	0x07D8	
	#define RT2_CtrlWord_TxMC23	0x07DC	
	#define RT2_CtrlWord_TxMC24	0x07E0	
	#define RT2_CtrlWord_TxMC25	0x07E4	
	#define RT2_CtrlWord_TxMC26	0x07E8	
	#define RT2_CtrlWord_TxMC27	0x07EC	
	#define RT2_CtrlWord_TxMC28	0x07F0	
	#define RT2_CtrlWord_TxMC29	0x07F4	
	#define RT2_CtrlWord_TxMC30	0x07F8	
	#define RT2_CtrlWord_TxMC31	0x07FC


///#endif // (HOST_SPI_INTERFACE)






//-----------------------------------------------------------------------------
//                     Function Prototypes
//-----------------------------------------------------------------------------


// self explanatory, HI-6130 ONLY
//
void RT_bus_addressing_examples(void);


//  	This function initializes the Holt HI-613x RT1 by writing 
//	configuration registers in the device. Only RT1 mode option 
//	bits are affected. The program has already called function 
// 	initialize_613x_shared() to initialize the common parameters
//	shared by BC, RT1, RT2 and/or Bus Monitor
// 
void initialize_613x_RT1(void);


//  	This function initializes the Holt HI-613x RT2 by writing 
//	configuration registers in the device. Only RT2 mode option 
//	bits are affected. The program has already called function 
// 	initialize_613x_shared() to initialize the common parameters
//	shared by BC, RT1, RT2 and/or Bus Monitor
// 
void initialize_613x_RT2(void);


//	This function checks the selected (1 or 2) RT's Status reg for RT addr parity error.	
// 
char RTAddr_okay(char RTnum);


//	This function polls pushbutton SW1 to detect/perform requested status update.	
// 
void modify_RT_status_bits(void);


// 	These functions load dummy data into the limited set of RT1 or RT2 transmit
//	buffers assigned above during initialization. This is only used for testing.
// 
void write_dummy_tx_data_RT1(void);
void write_dummy_tx_data_RT2(void);


//	This function reads BUSY and TFLAG DIP switch settings then updates HI-613x 
//      RT1 and RT2 "1553 Status Word Bits Registers" 
// 
void RTstatusUpdate(void);



// End of File

