/** @file
*********************************************************************
*  @brief ����������� ������ 
*
*  �����       : ������ �.�. (c) �� ��� ��
*
*  ����        : 13.10.2017
*
*********************************************************************
**/
// #include "ftd2xx.h" // Библиотека для работы с ftdi
#include "ftdi_spi.h"
#include "ftdi_mpsse_code.h"
#include <stdlib.h>
#include <unistd.h> // Для замены функции Sleep на usleep

#include "ftd2xx.h"
#include <stdio.h>

struct MPSSE_Cmd
{
  uint_least8_t wr_byte ;
  uint_least8_t wr_bit  ;
  uint_least8_t rd_byte ;
  uint_least8_t rd_bit  ;
};
typedef struct MPSSE_Cmd MPSSE_Cmd ;


static char magic[] = "FTDI_SPI_magic" ;
struct FTDI_SPI_s
{
  const char* magic ;
  FT_HANDLE  ft_h ;
  FTDI_SPI_open_param param ;
  const MPSSE_Cmd* p_cmd ;
} ;

static int handle_is_valid(FTDI_SPI_h handle)
{
  return handle && magic == handle->magic ;
}

void FTDI_SPI_close(FTDI_SPI_h handle)
{
  if (handle_is_valid(handle))
  {  
    if (handle->ft_h)
      FT_Close(handle->ft_h) ;
    
    handle->magic = 0 ;
    free(handle) ;    
  }
}

static FTDI_SPI_retval check_echo(FT_HANDLE ftHandle, uint_least8_t cmd)
{
  printf("-----------------------------\nCheck echo \n"); 
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {
    BYTE  cmd_buf[0x4] ;
    BYTE  ack_buf[0x4] ;
    DWORD cnt = 0 ;
    DWORD dwNumBytesSent,dwNumBytesRead,xxx ;

    // Send off the BAD commands
    cmd_buf[cnt++] = cmd ;

    // Шлём байты, получаем их количество
    if (FT_OK != FT_Write(ftHandle,cmd_buf,cnt,&dwNumBytesSent))
    {
      error(0x82A64B78uL);
    }
    else 
    {
      printf("dwNumBytesSent:%d\nЗаписанный буфер:",dwNumBytesSent); 
      for(int i = 0; i < 0x4;i++)
        printf("%d ",cmd_buf[i]);
      printf("\n");
    }

    usleep(1000) ;

    // Количество байт в очереди приёма
    if (FT_OK != FT_GetQueueStatus(ftHandle, &xxx))
    {
      error(0x7F5F6C4BuL);
    }
    else printf("xxx:%d\n",xxx);

    // Читаем байты, получаем количество
    if (FT_OK != FT_Read(ftHandle,ack_buf,xxx,&dwNumBytesRead))
    {
      error(0x132072D1uL);
    }
    else 
    {
      printf("dwNumBytesRead:%d\n",dwNumBytesRead);
      for(int i = 0; i < 0x4;i++)
        printf("%d ",ack_buf[i]);
      printf("\n");
    }
    // Сравниваем принятое и полученное
    if (xxx!=dwNumBytesRead)
    {
      error(0x4470A118uL) ;
    }

    // Ошибка
    #if 1 // другие команды :-(
    if (FTDI_BAD_ACK!=ack_buf[xxx-2])
      error(0xA3C0EFF1uL) ;

    // 0xAA(170) != ack_buff
    if (cmd!=ack_buf[xxx-1])
      error(0xD82341D0uL) ;
    printf("cmd:%d\nack_buf[%d]:%d\n",cmd,xxx-1,ack_buf[xxx-1]);
    #endif

    //if (!((2==dwNumBytesRead) && (FTDI_BAD_ACK==ack_buf[0]) && (cmd==ack_buf[1])))
    //  error(0x17D51BB9uL) ;
  } while(0) ;
  #undef error

  return retval ;
}

static FTDI_SPI_retval spi_inital(FT_HANDLE ftHandle)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {
    //Reset USB device 
    if (FT_OK != FT_ResetDevice(ftHandle))
      error(0xCBAC01E5uL); 

    for(;;)
    { //Purge USB receive buffer first by reading out all old data from FT2232H receive buffer
      DWORD dwNumInputBuffer, dwNumBytesRead ;
      uint_least8_t trash_buf[0x80] ;

      // Get the number of bytes in the FT2232H receive buffer
      if (FT_OK != FT_GetQueueStatus(ftHandle, &dwNumInputBuffer))
        error(0x6C85EE11uL) ;

      if (0==dwNumInputBuffer)
        break ;

      if (sizeof(trash_buf) < dwNumInputBuffer)
        dwNumInputBuffer = sizeof(trash_buf) ;

      if (FT_OK != FT_Read(ftHandle, trash_buf, dwNumInputBuffer, &dwNumBytesRead))
        error(0x03A45F3CuL) ;
    }
    if (retval)
      break ;

    //Set USB request transfer size
    if (FT_OK != FT_SetUSBParameters(ftHandle, 65535, 65535))
      error(0xDCFE06B5uL); 

    //Disable event and error characters
    if (FT_OK != FT_SetChars(ftHandle, FALSE, 0, FALSE, 0))
      error(0x7643EDF1uL); 

    //Sets the read and write timeouts in 3 sec for the FT2232H
    if (FT_OK != FT_SetTimeouts(ftHandle, 3000, 3000))
      error(0xB1598BF6uL); 

    //Set the latency timer
    if (FT_OK != FT_SetLatencyTimer(ftHandle, 1))
      error(0x731A343CuL); 

    //Reset controller
    if (FT_OK != FT_SetBitMode(ftHandle, 0x0, 0x00))
      error(0x92733BF7uL); 

    //Enable MPSSE mode
    if (FT_OK != FT_SetBitMode(ftHandle, 0x0, 0x02))
      error(0xBDA46E6EuL); 
    
    usleep(50); // Wait 50ms for all the USB stuff to complete and work

    {
      BYTE  cmd_buf[0x8] ;
      DWORD cnt = 0 ;
      DWORD bytesWritten ;

      cmd_buf[cnt++] = FTDI_CLK_3PHASE_OFF  ;
      cmd_buf[cnt++] = FTDI_CLK_ADAPTIVE_OFF ;
      cmd_buf[cnt++] = MPSSE_GPIO_SET_LO ;
      cmd_buf[cnt++] = 0xFC ;
      cmd_buf[cnt++] = 0x03 ;

      if (FT_OK != FT_Write(ftHandle,cmd_buf,cnt,&bytesWritten))
        error(0x3D7BD9B0uL) ;
    }      
  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval FTDI_SPI_open(FTDI_SPI_h* p_handle, const FTDI_SPI_open_param* param)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 
  FTDI_SPI_h handle = 0 ;
  FT_HANDLE ft_h = 0 ;
  
  #define error(x) { retval = x ; break ; }
  do {
    if (!p_handle)
      error(0x8E1301D9uL) ;
    if (!param)
      error(0x12C40DCBuL) ;
    if ((param->cs_bit < 3) || (param->cs_bit > 15)) 
      error(0xE8A41C94uL) ;
      
    // if (FT_OK != FT_Open(param->inst_no, &ft_h)
    //   error(0x9A43C17FuL) ;

    // Моя самодеятельность
    FT_STATUS ftStatus = FT_Open(param->inst_no, &ft_h);
    if (FT_OK != ftStatus)
    {
      printf("Open Failed with error %d\n",  ftStatus);
      error(0x9A43C17FuL) ;
    }

    if (0 == (handle = (FTDI_SPI_s *)malloc(sizeof(((FTDI_SPI_s)handle[0])))))
      error(0xDBE458B4uL) ;

    if (FTDI_SPI_retval_ok != (retval = spi_inital(ft_h)))
      break ; 

    // Тут возникает ошибка
    if (FTDI_SPI_retval_ok != (retval = check_echo(ft_h, 0xAA)))
      break ;
    if (FTDI_SPI_retval_ok != (retval = check_echo(ft_h, 0xAB)))
      break ;

    handle->magic = magic ;
    handle->ft_h  = ft_h  ; 
    handle->param = param[0] ;
    
    p_handle[0] = handle ;
  } while(0) ;
  #undef error

  if (retval)
  {  
    if (ft_h)
      FT_Close(ft_h) ;
    if (handle)
    {
      handle->magic = 0 ;
      free(handle) ;
    }
  }

  return retval ;  
}


FTDI_SPI_retval FTDI_SPI_mode_set(FTDI_SPI_h handle, FTDI_SPI_mode mode)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {
    static const MPSSE_Cmd __cmd[FTDI_SPI_mode_max] =
    {
      //FTDI_SPI_mode_RE_OUT_RE_IN
      { MPSSE_MSB_RE_OUT_BYTE , MPSSE_MSB_RE_OUT_BIT , MPSSE_MSB_RE_IN_BYTE, MPSSE_MSB_RE_IN_BIT } ,
      //FTDI_SPI_mode_RE_OUT_FE_IN
      { MPSSE_MSB_RE_OUT_BYTE , MPSSE_MSB_RE_OUT_BIT , MPSSE_MSB_FE_IN_BYTE, MPSSE_MSB_FE_IN_BIT } ,
      //FTDI_SPI_mode_FE_OUT_RE_IN
      { MPSSE_MSB_FE_OUT_BYTE , MPSSE_MSB_FE_OUT_BIT , MPSSE_MSB_RE_IN_BYTE, MPSSE_MSB_RE_IN_BIT } ,
      //FTDI_SPI_mode_FE_OUT_FE_IN
      { MPSSE_MSB_FE_OUT_BYTE , MPSSE_MSB_FE_OUT_BIT , MPSSE_MSB_FE_IN_BYTE, MPSSE_MSB_FE_IN_BIT } ,
    } ;

    if (!handle_is_valid(handle))
      error(0x33E35947uL) ;

    if ((uint_least32_t)mode >= FTDI_SPI_mode_max)
      error(0xF5CFC48CuL) ;

    handle->p_cmd = __cmd + (uint_least32_t)mode ;
  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval FTDI_SPI_baudrate_set(FTDI_SPI_h handle, uint_least32_t bps)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {    
    if (!handle_is_valid(handle))
      error(0xD4A9E547uL) ;
    if (0==bps)
      error(0x6D65569AuL) ;
    
    {
      enum { freq = 60000000 };
      BYTE  cmd_buf[0x4] ;
      DWORD cnt = 0 ;
      DWORD bytesWritten ;
      uint_least32_t div_f = (freq + bps)/(2*bps) ;
      if (0 == div_f)
        error(0x6D65569AuL) ;

      if (div_f > 0x10000)
      { 
        cmd_buf[cnt++] = FTDI_CLK_DIV5_ON  ;
        div_f = div_f/5 - 1 ;      
      }
      else
      {
        cmd_buf[cnt++] = FTDI_CLK_DIV5_OFF  ;
        div_f -= 1 ;      
      }

      cmd_buf[cnt++] = MPSSE_SET_CLK_DIV ;
      cmd_buf[cnt++] = div_f & 0xFF ;
      cmd_buf[cnt++] = div_f >>8 ;
      
      if (FT_OK !=  FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
        error(0xE1328EFAuL) ;
      if (cnt != bytesWritten) 
        error(0xB738F6F1uL) ;

    }
  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval FTDI_SPI_loopback(FTDI_SPI_h handle, uint_least8_t enable)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 

  #define error(x) { retval = x ; break ; }
  do {
    if (!handle_is_valid(handle))
      error(0xD4A9E547uL) ;

    {
      BYTE  cmd_buf[0x4] ;
      DWORD cnt = 0 ;
      DWORD bytesWritten ;

      cmd_buf[cnt++] = enable ? MPSSE_LOOPBACK_ON : MPSSE_LOOPBACK_OFF ; 

      if (FT_OK !=  FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
        error(0xE1328EFAuL) ;
    }
  } while(0) ;
  #undef error

  return retval ;
}

static FTDI_SPI_retval __CS_set(FT_HANDLE ftHandle, uint_least8_t nbit, uint_least8_t value)
{ 
  BYTE  cmd_buf[0x4] ;
  DWORD cnt = 0 ;
  DWORD bytesWritten ;
  BYTE  pio = ~((value ? 1 : 0) << nbit) ;


  if (nbit < 8)
  {
    cmd_buf[cnt++] = MPSSE_GPIO_SET_LO ;
    cmd_buf[cnt++] = pio & ~0x03 ;
    cmd_buf[cnt++] = 0x03 | (1<<nbit);    
  }
  else
  {
    nbit -= 8 ;
    cmd_buf[cnt++] = MPSSE_GPIO_SET_HI ;
    cmd_buf[cnt++] = pio ;
    cmd_buf[cnt++] = (1<<nbit);        
  }

  return FT_OK != FT_Write(ftHandle,cmd_buf,cnt,&bytesWritten) 
    ? 0x08915A55uL : FTDI_SPI_retval_ok ;
}

FTDI_SPI_retval FTDI_SPI_CS_enable(FTDI_SPI_h handle)
{
  return handle_is_valid(handle)
    ? __CS_set(handle->ft_h,handle->param.cs_bit,1) 
    : 0x437A0B63uL ;
}

FTDI_SPI_retval FTDI_SPI_CS_disable(FTDI_SPI_h handle)
{
  return handle_is_valid(handle)
    ? __CS_set(handle->ft_h,handle->param.cs_bit,0) 
    : 0x04DA3B4AuL;
}


FTDI_SPI_retval FTDI_SPI_ro(FTDI_SPI_h handle, uint_least32_t n_bits, uint_least8_t* p_buf)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 
  uint_least32_t n_bits_readed = 0 ;

  #define error(x) { retval = x ; break ; }
  do {   
    if (0==n_bits)
      error(0x4B3216E4uL) ;
    if (!p_buf)
      error(0x952D817AuL) ;

    if (!handle_is_valid(handle))
      error(0x6B8F99FCuL) ;

    {
      uint_least32_t _n_bytes = n_bits>>3 ;
      uint_least32_t _n_bits  = n_bits&7 ;

      if (_n_bytes)
      { 
        BYTE  cmd_buf[0x4] ;      
        DWORD cnt = 0 ;
        DWORD bytesWritten,bytesReaded ;

        cmd_buf[cnt++] = handle->p_cmd->rd_byte ;
        cmd_buf[cnt++] = (_n_bytes-1) & 0xFF ;
        cmd_buf[cnt++] = (_n_bytes-1) >>8 ;

        if (FT_OK != FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
          error(0x3FA622CEuL) ;

        if (FT_OK != FT_Read(handle->ft_h,p_buf,_n_bytes,&bytesReaded)) 
          error(0xE7FAC559uL) ;

        n_bits_readed += bytesReaded<<3 ;
      }

      if (_n_bits)
      {      
        BYTE  cmd_buf[0x4] ;      
        DWORD cnt = 0 ;
        DWORD bytesWritten,bytesReaded ;

        cmd_buf[cnt++] = handle->p_cmd->rd_bit ;
        cmd_buf[cnt++] = _n_bits-1;

        if (FT_OK != FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
          error(0x299A5C43uL) ;

        if (FT_OK != FT_Read(handle->ft_h,p_buf+_n_bytes,1,&bytesReaded)) 
          error(0x720D60F3uL) ;

        bytesReaded += bytesReaded ? _n_bits : 0 ;
      }
    }

  } while(0) ;
  #undef error

  return retval ;
}

FTDI_SPI_retval FTDI_SPI_wo(FTDI_SPI_h handle, uint_least32_t n_bits, const uint_least8_t* p_buf)
{
  FTDI_SPI_retval retval = FTDI_SPI_retval_ok ; 
  uint_least32_t n_bits_written = 0 ;

  #define error(x) { retval = x ; break ; }
  do {    
    if (0==n_bits)
      error(0x4B3216E4uL) ;
    if (!p_buf)
      error(0x2CF00EE9uL) ;

    if (!handle_is_valid(handle))
      error(0x2A085159uL) ;

    {
      uint_least32_t _n_bytes = n_bits>>3 ;
      uint_least32_t _n_bits  = n_bits&7 ;

      if (_n_bytes)
      { 
        BYTE  cmd_buf[0x4] ;      
        DWORD cnt = 0 ;
        DWORD bytesWritten ;

        cmd_buf[cnt++] = handle->p_cmd->wr_byte ;
        cmd_buf[cnt++] = (_n_bytes-1) & 0xFF ;
        cmd_buf[cnt++] = (_n_bytes-1) >>8 ;

        if (FT_OK != FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
          error(0x11444596uL) ;

        if (FT_OK != FT_Write(handle->ft_h,(LPVOID)p_buf,_n_bytes,&bytesWritten))
          error(0x210906CDuL) ;
        n_bits_written += bytesWritten<<3 ;
      }

      if (_n_bits)
      {      
        BYTE  cmd_buf[0x4] ;      
        DWORD cnt = 0 ;
        DWORD bytesWritten ;

        cmd_buf[cnt++] = handle->p_cmd->wr_bit ;
        cmd_buf[cnt++] = _n_bits-1;
        cmd_buf[cnt++] = p_buf[_n_bytes];

        if (FT_OK != FT_Write(handle->ft_h,cmd_buf,cnt,&bytesWritten))
          error(0x8122B74CuL) ;        

        n_bits_written += bytesWritten ? _n_bits : 0 ;
      }
    }
  } while(0) ;
  #undef error

  return retval ;
}
