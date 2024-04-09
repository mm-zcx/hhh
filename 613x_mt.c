/*
 *  file	613x_mt.c
 *
 *  brief	This file contains initialization functions and arrays for 
 *		writing initialization values to RAM tables inside the Holt 
 *		HI-6131 device. 
 *
 *
 *		IMPORTANT: Edit file 613x_initialization.h to define either:
 *
 *		Most functional procedures in this project are written for
 *		both parallel bus or SPI host interface schemes, this runtime 
 *		switch determines which are procedures are compiled.
 * 
 *
 *		HOLT DISCLAIMER
 *
 *		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *		EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *		OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *		NONINFRINGEMENT. 
 *		IN NO EVENT SHALL HOLT, INC BE LIABLE FOR ANY CLAIM, DAMAGES
 *		OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 *		OTHERWISE,ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 *		SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *		Copyright (C) 2011 by  HOLT, Inc.
 *		All Rights Reserved.
 *
 */

// standard Atmel/IAR headers
#include <pio/pio.h>

// Holt project headers
#include "613x_initialization.h"
#include "613x_regs.h"
#include "board_613x.h"
#include "613x_mt.h"


#include "board_6131.h"
#include "device_6131.h"

//================================================================================================


// The following function initializes either the Simple or IRIG-106 monitor
// (SMT or IMT) as enabled in file 613x_initialization.h. Same file also
// specifies HI-6131 (SPI) configuration...
//
// (3) SMT for HI-6131 (SPI) applications
//
void initialize_613x_MT(void) {     
    unsigned short i,j;
    
    
    #if(SMT_ena)
    volatile unsigned short smt_addr_list[8] = {

    //  =============  Command Stack ==============
    //  Start     Current   End       Interrupt
    //  Address   Address   Address   Address
        0x5400,   0x5400,   0x5FFF,   0x5DFF, // end - 512 
		
    //  ==============  Data Stack  ================
    //  Start     Current   End       Interrupt 
    //  Address   Address   Address   Address   
        0x6000,   0x6000,   0x7FFF,   0x7DFF }; // end - 512 

    #else // (IMT_ena)
     volatile unsigned short imt_addr_list[8] = {

    //  =============  Combined Stack ==============
    //  Start     Current   End       Interrupt
    //  Address   Address   Address   Address
        0x5400,   0x5400,   0x6400,   0, 

    //  =============  Combined Stack ==============
    //  Last Msg  Reserved  Reserved  Interrupt N Words
    //  Address   Address   Address   before End-of-Stack
        0,        0,        0,        512 };
     
    #endif

    volatile unsigned short mt_filter_table[128] = {

    // bit = 0: all msgs to that subaddress are recorded, 
    // bit = 1: all messages to that subaddress are ignored.
    // default: (even if table not written) all messages are recorded  
    // RxSA = receive subaddresses,     TxSA = tranmsit subaddresses   
    //====================================================================
    // RT address 0   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 1   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 2   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 3   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 4   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 5   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 6   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 7   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 8   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 9   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 10   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 11   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 12   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 13   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 14   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 15   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 16   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 17   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 18   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 19   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 20   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 21  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 22   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 23  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 24  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 25  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 26  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 27  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 28  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 29   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 30  

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000,
    //====================================================================
    // RT address 31   

    //   ____RxSA 15-12  ____TxSA 15-12  ____RxSA 31-28  ____TxSA 31-28
    //  |               |               |               |
    //  | ___RxSA 11-8  | ___TxSA 11-8  | ___RxSA 27-24 | ___TxSA 27-24
    //  ||              ||              ||              ||
    //  || __RxSA 7-4   || __TxSA 7-4   || __RxSA 23-20 || __TxSA 23-20
    //  |||             |||             |||             |||
    //  ||| _RxSA 3-0   ||| _TxSA 3-0   ||| _RxSA 19-16 ||| _TxSA 19-16
    //  ||||            ||||            ||||            ||||
      0x0000,         0x0000,         0x0000,         0x0000  };
    // ====================================================================
    // end of mt_filter_table[128]

   
    // HI-6130 uses host bus interface, HI-6131 uses host SPI interface. From 
    // here, we use bus interface to initialize HI-6130 registers and RAM tables,
    // or we use SPI to initialize HI-6131 registers and RAM tables 

    // ======================================================================================

        // Here, we use SPI interface to initialize HI-6131 registers and RAM 
        // The SPI can directly read registers 0-31 decimal, without using the mem address pointer.
        // The SPI can directly write registers 0-63 decimal, without using the mem address pointer.
        // For higher addresses, SPI read/write accesses must use a memory address pointer. 
                                    
        // enable Memory Address Pointer 1
        enaMAP(1);

        // Initialize MT Filter table in RAM using values in array above.
        // Skip this if all messages shall be recorded (since Master Reset clears RAM) 

        Write_6131LowReg(MAP_1,0x0100,0);	
        for ( i = 0; i < 128; i++) {	
            Write_6131_1word(mt_filter_table[i],0);
        }
                    
            
	// ================== Simple Monitor ======================= 

	#if(SMT_ena)

            // Initialize base address for MT Address List at 0x00B0  
            Write_6131LowReg(MT_ADDR_LIST_POINTER,0x00B0,0);

            // Load same address to mem address ptr 
            Write_6131LowReg(MAP_1,0x00B0,0);	

            // initialize MT address list using array declared at top of function 
            for ( i = 0; i < 8; i++) {
                Write_6131_1word(smt_addr_list[i],0);
            }

            // Set up SMT interrupts:
            //
            // STKROVR   = command stack end-address written, ptr rolled-over to start address
            //             (MT address list word 2 contains fixed end address)
            // DSTKROVR  = data stack end-address written, ptr rolled-over to start address
            //             (MT address list word 6 contains fixed end address)
            // STKADRSS  = command stack addr match, level-attained int
            //             (MT address list word 3 contains fixed int address)
            // DSTKADRSS = data stack address match,
            //             (MT address list word 7 contains fixed int address)
            // MSG_ERR   = detected RT status word with message error
            // MT_EOM    = end-of-message interrupt

            // Interrupt Enable, both hardware ints and polled ints  
            Write_6131LowReg(MT_INT_ENABLE_REG,
            STKROVR|DSTKROVR|STKADRSS|DSTKADRSS|MSG_ERR|MT_EOM, 0);

            // Output Enable for hardware ints  
            Write_6131LowReg(MT_INT_OUTPUT_ENABLE_REG,
            STKROVR|DSTKROVR|STKADRSS|DSTKADRSS|MSG_ERR|MT_EOM, 0);

            // Optional: assert IMTA bit in the Master Config Reg 0 
            // so the ACTIVE pin reflects MT activity 
            j = Read_6131LowReg(MASTER_CONFIG_REG, 0);
            Write_6131LowReg(MASTER_CONFIG_REG, j|IMTA, 0);
		
            // Config options for Simple monitor, 
            // option bit SELECT_SMT must be 1. 
            i = SELECT_SMT;
            
            // Choose 1 from each of the following choices:
            //   GAPCHKON or GAPCHKOFF
            //   STOR_INVWD or STOP_INVWD
            //   EXTD_STATUS or IRIG_STATUS
            //   REC_CW or REC_CWDW or REC_CS or REC_CSDS
            //   TAG_LWLB or TAG_FWFB or TAG_FWLB or TAG_OFF
            //   MTTO_15U or MTTO_20U or MTTO_58U or MTTO_138U 
            //  -----------------------------------------------
            //  Note: SMT time tag clock is selected in main.c,
            //  as well as this SMT time tag resolution option:
            //   SMT_TTAG16 or SMT_TTAG48

            i |= GAPCHKOFF|STOR_INVWD|EXTD_STATUS|REC_CSDS|TAG_OFF|MTTO_20U;
                       
            Write_6131LowReg(MT_CONFIG_REG,i,0);

        // end SMT_ena 

        // ================== IRIG-106 Monitor ======================= 

        #else // (IMT_ena) 

            // Initialize base address for MT Address List at 0x00B0  
            Write_6131LowReg(MT_ADDR_LIST_POINTER,0x00B0,0);

            // Load same address to mem address ptr 
            Write_6131LowReg(MAP_1,0x00B0,0);	

            // initialize MT address list using array declared at top of function 
            for ( i = 0; i < 8; i++) {
		Write_6131_1word(imt_addr_list[i],0);
            }

            // In addition to these packet size limits, a stack rollover trips packet finalization... 
            Write_6131LowReg(IMT_MAX_1553_MSGS,4545,0); // max possible in 100ms = 4,545
            Write_6131LowReg(IMT_MAX_1553_WORDS,0,0); 	// 32320,0);
            Write_6131LowReg(IMT_MAX_PKT_TIME,0,0);	// 10000,0); // 10us resolution
            Write_6131LowReg(IMT_MAX_GAP_TIME,0,0); 	// max deadtime = 10(N-2)us
            Write_6131LowReg(IMT_CHANNEL_ID,0xABCD,0);

            // Set up IRIG-106 MT interrupts. 
            // 
            // FULL_EOP  = stack full, end-of-packet (words left < 64)
            // FULL_OFS  = stack offset from FULL_EOP interrupt 
            //             (IRIG-106 HEADER MUST BE ENABLED)
            //             (MT address list word 7 contains # words offset)
            // MAXWORDS  = end-of-packet, hit 1553 word count limit per pkt
            // MAXMSGS   = end-of-packet, hit message count limit per pkt
            // MAXGAP    = end-of-packet, exceeded max bus deadtime per pkt
            // MAXTIME   = end-of-packet, hit maximum allowed pkt time
            // HPKTSTOP  = host packet stop interrupt
            // MT_EOM    = end-of-message interrupt
            // STKROVR   = stack end-address written, ptr rolled-over to start address 
            //             (MT address list word 2 contains stack end address)
            // STKADRSS  = stack address written, equal to MT address list word 3
            // PKTRDY    = packet ready interrupt
		
            // Interrupt Enable, vectored ints and polled ints  
            Write_6131LowReg(MT_INT_ENABLE_REG, 
            FULL_EOP|MAXWORDS|MAXMSGS|MAXGAP|MAXTIME|HPKTSTOP|PKTREADY|STKROVR, 0); 
                           //not used: |FULL_OFS|STKADRSS|MT_EOM);

            // Output enable for vectored ints  
            Write_6131LowReg(MT_INT_OUTPUT_ENABLE_REG, 
            FULL_EOP|MAXWORDS|MAXMSGS|MAXGAP|MAXTIME|HPKTSTOP|PKTREADY|STKROVR, 0); 
                           //not used: |FULL_OFS|STKADRSS|MT_EOM);

            // Optional: assert IMTA bit in the Master Config Reg 0 
            // so the ACTIVE pin reflects MT activity 
            j = Read_6131LowReg(MASTER_CONFIG_REG, 0);
            Write_6131LowReg(MASTER_CONFIG_REG, j|IMTA, 0);
		
            // Config options (IMT automatically uses TTAG48) 		 
            // for IRIG-106, option SELECT_IMT must be 1. 
            i = SELECT_IMT;

            // Choose 1 from each of the following choices:
            //		 
            // GAPCHKON or GAPCHKOFF
            // STOR_INVWD or STOP_INVWD
            // PKTSTRT_CW or PKTSTRT_ENA
            // IMT_HDR_ON or IMT_HDR_OFF
            // EXTD_STATUS or IRIG_STATUS
            // IMT_CKSUM_ON or IMT_CKSUM_OFF
            // REC_CW or REC_CWDW or REC_CS or REC_CSDS
            // TAG_LWLB or TAG_FWFB or TAG_FWLB or TAG_OFF
            // MTTO_15U or MTTO_20U or MTTO_58U or MTTO_138U 
            // IMT_DTYPE9 or IMT_DTYPE4 or IMT_DTYPE5 or IMT_DTYPE7  
            
            i |= GAPCHKOFF|STOR_INVWD|PKTSTRT_CW
                 |IMT_HDR_OFF|EXTD_STATUS|IMT_CKSUM_ON
                 |REC_CSDS|TAG_OFF|MTTO_20U|IMT_DTYPE9;

            Write_6131LowReg(MT_CONFIG_REG,i,0);

        #endif // end: IMT_ena

    // ====================================================================================== 

}   // end initialize_613x_MT(void)


// end of file 

