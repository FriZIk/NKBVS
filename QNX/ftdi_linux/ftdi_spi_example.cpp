//
// SPITEST.cpp : VC++ console application.
// this example project use port A of FT2232H to access SPI EEPROM 93C56
// we send 16 word data to 93C56 and read them back, user can see the test
// result in command mode.
//#include "stdafx.h"
// #include <windows.h>
// #include <libftdi1/ftdi.h>

#include <stdlib.h>
#include <stdio.h>

// Мои приколы
#include <unistd.h> // Для замены функции Sleep на usleep
#include <string.h> // Для memset
#include <stdint.h> // Для макросов

// #include "ftd2xx.h" // Библиотека для работы с ftdi
#include "ftdi_spi.h" 
#include "ftd2xx.h"

FT_STATUS ftStatus; //Status defined in D2XX to indicate operation result

static int my_test();

int main()
{
  FT_HANDLE ftdiHandle;
  DWORD numDevs;
  FT_DEVICE_LIST_INFO_NODE *devInfo;

  for(;;)
  {
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK)
      printf("Number of devices is %d\n",numDevs);
    else
      return 1;
    //if (numDevs > 0) 
    //if (numDevs >= 4) 
    if (numDevs >= 2) 
    {
      usleep(1000) ;
      ftStatus = FT_CreateDeviceInfoList(&numDevs);
      // allocate storage for list based on numDevs
      devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);


      // get the device information list
      ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
      if (ftStatus == FT_OK) 
      {
        for (int i = 0; i < numDevs; i++) 
        {
          printf("Dev %d:\n",i);
          printf(" Flags=0x%x\n",devInfo[i].Flags);
          printf(" Type=0x%x\n",devInfo[i].Type);
          printf(" ID=0x%x\n",devInfo[i].ID);
          printf(" LocId=0x%x\n",devInfo[i].LocId);
          printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
          printf(" Description=%s\n",devInfo[i].Description);
          printf(" ftHandle=0x%x\n",devInfo[i].ftHandle);
        }
      }
      break ;
    }
    //else
    //  return 1;

    usleep(100) ;
  }

  return my_test() ;
}


static FTDI_SPI_retval __exch_wo
(
  FTDI_SPI_h handle, 
  const uint_least8_t* cmd, uint_least8_t cmd_sz
)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {      
    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_enable(handle)))
      break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_wo(
      handle,cmd_sz<<3,cmd
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_disable(handle)))
      break ;
  } while(0) ;
  #undef error

  return retval ;
}


static FTDI_SPI_retval __exch_rw
(
  FTDI_SPI_h handle, 
  const uint_least8_t* cmd, uint_least8_t cmd_sz,
  uint_least8_t* ack, uint_least8_t ack_sz
)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {      
    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_enable(handle)))
      break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_wo(
      handle,cmd_sz<<3,cmd
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_ro(
      handle,ack_sz<<3,ack
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_disable(handle)))
      break ;
  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval rd_axi(FTDI_SPI_h handle, uint_least32_t addr, uint_least32_t* p_val)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {      
    if (!p_val)
      error(0xA3B18D3FuL) ;
    
    {
      uint_least8_t cmd[5] = 
      { 
        0x0b , 
        (addr>>16) & 0xFF , (addr>>8) & 0xFF, addr & 0xFF, 
        0x00 
      } ;
      uint_least8_t ack[4] ;

      if (FTDI_SPI_retval_ok != (retval = __exch_rw(
        handle,cmd,sizeof(cmd),ack,sizeof(ack)
      ))) break ;

      p_val[0] = (ack[0]<<24UL) | (ack[1]<<16UL) | (ack[2]<<8UL) | ack[3] ;
    }
  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval wr_axi(FTDI_SPI_h handle, uint_least32_t addr, uint_least32_t val)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {          
    {
      uint_least8_t cmd[8] = 
      { 
        0x02, 
        (addr>>16) & 0xFF , (addr>>8) & 0xFF, addr & 0xFF, 
        (val>>24) & 0xFF , (val>>16) & 0xFF , (val>>8) & 0xFF, val & 0xFF, 
      } ;

      if (FTDI_SPI_retval_ok != (retval = __exch_wo(
        handle,cmd,sizeof(cmd)
      ))) break ;
    }
  } while(0) ;
  #undef error

  return retval ;
}


FTDI_SPI_retval rd_id(FTDI_SPI_h handle, uint_least32_t* p_val)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {      
    if (!p_val)
      error(0x51A209A3uL) ;
    
    {
      uint_least8_t cmd[1] = { 0x9f} ;
      uint_least8_t ack[2] ;

      if (FTDI_SPI_retval_ok != (retval = __exch_rw(
        handle,cmd,sizeof(cmd),ack,sizeof(ack)
      ))) break ;

      p_val[0] = (ack[0]<<8UL) |  ack[1] ;
    }
  } while(0) ;
  #undef error

  return retval ;
}


static int my_test()
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 
  FTDI_SPI_h handle = 0 ;

  #define error(x) { retval = x ; break ; }
  do {
    FTDI_SPI_open_param param ;
    memset(&param,0,sizeof(param)) ;

    param.inst_no = 0 ; // ����� A
    //param.inst_no = 1 ; // ����� B
    //param.inst_no = 2 ; // ����� C // ???
    //param.inst_no = 3 ; // ����� D // ???
    param.cs_bit  = 3 ;
        
    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_open(
      &handle,&param
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_mode_set(
      handle,FTDI_SPI_mode_FE_OUT_RE_IN
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_baudrate_set(
      handle,1000000
    ))) break ;

    if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_loopback(
      handle,0
    ))) break ;

    for(int j=0 ; j<2 ; ++j)
    {
      uint_least32_t id ;

      //for(;;) { do {
      if (FTDI_SPI_retval_ok != (retval = rd_id(
        handle,&id
      ))) break ;
      //} while(0) ; if (!retval) printf("ID=0x%04x\n",id) ; usleep(200) ; }
      //printf("ID=0x%04x\n",id) ;

      // Поменял типы
      typedef struct { uint_least32_t addr ; uint_least32_t val ; } Pair ;
      Pair rd_mas[] = 
      { 
        { 0x000000 },
        { 0x000004 },
        { 0x000008 },
        { 0x00000c },
        { 0x000010 },
        { 0x000014 },
        { 0x000018 },
        { 0x00001c },
        { 0x000020 },
        { 0x000024 },
        { 0x000028 },
        { 0x00002c },
        { 0x000030 },
        { 0x000034 },
        { 0x000038 },
        { 0x00003c },

        { 0x100000 },
        { 0x100004 },
        { 0x100008 },
        { 0x10000c },
        { 0x100010 },
        { 0x100014 },
        { 0x100018 },
        { 0x10001c },
        { 0x100020 },
        { 0x100024 },


        { 0x300114 },
        { 0x300100 },
        { 0x300104 },
 
        { 0x300314 },
        { 0x300300 },
        { 0x300304 },
        // ������������ 
         { (uint_least32_t)-1 }
      } ;

      for(int i=0 ; (uint_least32_t)-1 != rd_mas[i].addr ; ++i) 
      {
        if (FTDI_SPI_retval_ok != (retval = rd_axi(
          handle,rd_mas[i].addr,&rd_mas[i].val
        ))) break ;
      }
      if (FTDI_SPI_retval_ok != retval)
        break ;

      Pair wr_mas[] = 
      {        
        { 0x0008bc, 0x00000001 },
        { 0x100010, 0x00000000 }, // ������������
        //{ 0x100010, 0xfff00000 },
        //{ 0x100010, 0xfff00001 },

        
        #if 1
        // ������� ���������� bar
        { 0x300114, 0xc0000000 },
        { 0x300100, 0x00000000 },
        { 0x300104, 0xc0000200 },
        
        { 0x300314, 0xc0800000 }, //{ 0x300314, 0xc0000000 },
        { 0x300300, 0x00000000 }, //{ 0x300300, 0x00000001 },
        { 0x300304, 0xc0000400 },
        #else
        // ������� ���������� ������
        { 0x300108, 0x90000000 }, // ��������� �����
        { 0x300110, 0x907fffff }, // ��������  �����
        { 0x300114, 0xc0200000 },
        { 0x300100, 0x00000000 },
        { 0x300104, 0x80000000 },

        { 0x300308, 0x90800000 }, // ��������� �����
        { 0x300310, 0x908fffff }, // ��������  �����
        { 0x300314, 0xc0100000 },
        { 0x300300, 0x00000000 },
        { 0x300304, 0x80000000 },
        #endif
        // ������������ 
         { (uint_least32_t)-1 }        
      } ;

      for(int i=0 ; (uint_least32_t)-1 != wr_mas[i].addr ; ++i) 
      {
        if (FTDI_SPI_retval_ok != (retval = wr_axi(
          handle,wr_mas[i].addr,wr_mas[i].val
        ))) break ;
      }
      if (FTDI_SPI_retval_ok != retval)
        break ;
    }

    if(0)
    {
      uint_least8_t m1[4] = { 0x9f} ;
      uint_least8_t m2[4] = { 0,0 } ;

      if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_enable(handle)))
        break ;

      if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_wo(
        handle,8*sizeof(m1),m1
      ))) break ;

      
      if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_ro(
        handle,8*sizeof(m2),m2
      ))) break ;
    
      if (FTDI_SPI_retval_ok != (retval = FTDI_SPI_CS_disable(handle)))
        break ;
    }

  } while(0) ;
  #undef error


  if (handle)
    FTDI_SPI_close(handle) ;

  return retval ;
}
