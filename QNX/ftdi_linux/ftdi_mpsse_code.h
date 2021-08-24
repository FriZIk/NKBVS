#ifndef _INCLUDE_471271033B8C72DC
#define _INCLUDE_471271033B8C72DC
/** @file
*********************************************************************
*  @brief Определение класса 
*
*  Автор       : Буркин И.А. (c) АО НКБ ВС
*
*  Дата        : 13.10.2017
*
*********************************************************************
**/

#define FTDI_BAD_ACK  0xFA // <CMD>


// 0 длины соотвествует  единице информации

//Clock Data Bytes Out on +ve clock edge MSB first (no read)
#define MPSSE_MSB_RE_OUT_BYTE 0x10  // <LenLo> <LenHi> <data> .. <data>  
//Clock Data Bytes Out on -ve clock edge MSB first (no read)
#define MPSSE_MSB_FE_OUT_BYTE 0x11  // <LenLo> <LenHi> <data> .. <data> 
//Clock Data Bits Out on +ve clock edge MSB first (no read)
#define MPSSE_MSB_RE_OUT_BIT  0x12  // <Len> <data>
//Clock Data Bits Out on -ve clock edge MSB first (no read)
#define MPSSE_MSB_FE_OUT_BIT  0x13  // <Len> <data>

//Clock Data Bytes In on +ve clock edge MSB first (no write)
#define MPSSE_MSB_RE_IN_BYTE 0x20  // <LenLo> <LenHi> 
//Clock Data Bytes In on -ve clock edge MSB first (no write)
#define MPSSE_MSB_FE_IN_BYTE 0x24  // <LenLo> <LenHi> 
//Clock Data Bits In on +ve clock edge MSB first (no write)
#define MPSSE_MSB_RE_IN_BIT  0x22  // <Len> 
//Clock Data Bits In on -ve clock edge MSB first (no write)
#define MPSSE_MSB_FE_IN_BIT  0x26  // <Len> 

//Clock Data Bytes In and Out MSB first out on -ve edge, in on +ve edge
#define MPSSE_MSB_FE_OUT_RE_IN_BYTE 0x31  // <LenLo> <LenHi> <data> .. <data>  
//Clock Data Bytes In and Out MSB first out on +ve edge, in on -ve edge
#define MPSSE_MSB_RE_OUT_FE_IN_BYTE 0x34  // <LenLo> <LenHi> <data> .. <data> 
//Clock Data Bits In and Out MSB first out on -ve edge, in on +ve edge
#define MPSSE_MSB_FE_OUT_RE_IN_BIT  0x33  // <Len> <data>
//Clock Data Bits In and Out MSB first out on +ve edge, in on -ve edge
#define MPSSE_MSB_RE_OUT_FE_IN_BIT  0x36  // <Len> <data>


//Clock Data Bytes Out on +ve clock edge LSB first (no read)
#define MPSSE_LSB_RE_OUT_BYTE 0x18
//Clock Data Bytes Out on -ve clock edge LSB first (no read)
#define MPSSE_LSB_FE_OUT_BYTE 0x19
//Clock Data Bits Out on +ve clock edge LSB first (no read) 
#define MPSSE_LSB_RE_OUT_BIT  0x1a
//Clock Data Bits Out on -ve clock edge LSB first (no read)
#define MPSSE_LSB_FE_OUT_BIT  0x1b

//Clock Data Bytes In on +ve clock edge LSB first (no write)
#define MPSSE_LSB_RE_IN_BYTE  0x28
//Clock Data Bytes In on -ve clock edge LSB first (no write)
#define MPSSE_LSB_FE_IN_BYTE  0x2c
//Clock Data Bytes In on -ve clock edge LSB first (no write)
#define MPSSE_LSB_RE_IN_BIT   0x2a
//Clock Data Bits In on -ve clock edge LSB first (no write)
#define MPSSE_LSB_FE_IN_BIT   0x2e

//Clock Data Bytes In and Out LSB first out on -ve edge, in on +ve edge
#define MPSSE_LSB_RE_OUT_FE_IN_BYTE 0x39
//Clock Data Bytes In and Out LSB first out on +ve edge, in on -ve edge
#define MPSSE_LSB_FE_OUT_RE_IN_BYTE 0x3c 
//Clock Data Bits In and Out LSB first out on -ve edge, in on +ve edge
#define MPSSE_LSB_RE_OUT_FE_IN_BIT  0x3b
//Clock Data Bits In and Out LSB first out on +ve edge, in on -ve edge
#define MPSSE_LSB_FE_OUT_RE_IN_BIT  0x3e

// JTAG
// 0x4a
// 0x4b
// 0x6a
// 0x6b
// 0x6e
// 0x6f
//Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bits
//0x8E
//Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bytes
//0x8F  
//Allows for a clock to be output without transferring data until a logic 1 input on GPIOL1 stops the clock.
//0x94  
//Allows for a clock to be output without transferring data until a logic 0 input on GPIOL1 stops the clock.
//0x95
//Allows for a clock to be output without transferring data until a logic 1 input on GPIOL1 stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes
//0x9C 
//Allows for a clock to be output without transferring data until a logic 0 input on GPIOL1 stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes
//0x9D



// GPIO
//Set Data bits LowByte
#define MPSSE_GPIO_SET_LO 0x80 //<value> <direction>
//Set Data bits HighByte
#define MPSSE_GPIO_SET_HI 0x82 //<value> <direction>
//Read Data bits LowByte
#define MPSSE_GPIO_GET_LO 0x81
//Read Data bits HighByte
#define MPSSE_GPIO_GET_HI 0x83
  
// LoopBack
#define MPSSE_LOOPBACK_ON  0x84
#define MPSSE_LOOPBACK_OFF 0x85


#define MPSSE_SET_CLK_DIV  0x86  // <ValueL> <ValueH>
//TCK/SK period = 12MHz / (( 1 +[(0xValueH * 256) OR 0xValueL] ) * 2)
//TCK/SK period = 60MHz / (( 1 +[(0xValueH * 256) OR 0xValueL] ) * 2)

// 5.1 Send Immediate
#define MPSSE_SEND_IMM   0x87

  
// 5.2 Wait On I/O High
#define MPSSE_WAIT_D1_HI 0x88
//  5.3 Wait On I/O Low
#define MPSSE_WAIT_D1_LO 0x89
  


// FT232H, FT2232H & FT4232H ONLY
// Disables the clk divide by 5 to allow for a 60MHz master clock.
#define FTDI_CLK_DIV5_OFF 0x8A
//Enables the clk divide by 5 to allow for backward compatibility with FT2232D
#define FTDI_CLK_DIV5_ON  0x8B

//Enables 3 phase data clocking. Used by I2C interfaces to allow data on both clock edges.
#define FTDI_CLK_3PHASE_ON  0x8C
//Disables 3 phase data clocking.Used by I2C interfaces to allow data on both clock edges.
#define FTDI_CLK_3PHASE_OFF  0x8D


//Enable adaptive clocking
#define FTDI_CLK_ADAPTIVE_ON  0x96
//Disable adaptive clocking
#define FTDI_CLK_ADAPTIVE_OFF 0x97


#endif 
