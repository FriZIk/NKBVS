#ifndef _INCLUDE_2D1E817E423A3674
#define _INCLUDE_2D1E817E423A3674
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
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

struct FTDI_SPI_s ;
typedef struct FTDI_SPI_s FTDI_SPI_s ;
typedef FTDI_SPI_s* FTDI_SPI_h ;

typedef uint_least32_t FTDI_SPI_retval ;

enum { FTDI_SPI_retval_ok =  0 } ;


struct FTDI_SPI_open_param
{
  uint_least32_t inst_no ;
  uint_least32_t cs_bit ;
};
typedef struct FTDI_SPI_open_param FTDI_SPI_open_param ;

FTDI_SPI_retval FTDI_SPI_open(FTDI_SPI_h* p_handle, const FTDI_SPI_open_param* param) ;
void FTDI_SPI_close(FTDI_SPI_h handle) ;

enum FTDI_SPI_mode
{
  FTDI_SPI_mode_RE_OUT_RE_IN,
  FTDI_SPI_mode_RE_OUT_FE_IN,
  FTDI_SPI_mode_FE_OUT_RE_IN,
  FTDI_SPI_mode_FE_OUT_FE_IN,

  FTDI_SPI_mode_max
};
typedef enum FTDI_SPI_mode FTDI_SPI_mode ;

FTDI_SPI_retval FTDI_SPI_mode_set(FTDI_SPI_h handle, FTDI_SPI_mode mode) ;
FTDI_SPI_retval FTDI_SPI_baudrate_set(FTDI_SPI_h handle, uint_least32_t bps) ;
FTDI_SPI_retval FTDI_SPI_loopback(FTDI_SPI_h handle, uint_least8_t enable) ;

FTDI_SPI_retval FTDI_SPI_CS_enable(FTDI_SPI_h handle) ;
FTDI_SPI_retval FTDI_SPI_CS_disable(FTDI_SPI_h handle) ;

FTDI_SPI_retval FTDI_SPI_ro(FTDI_SPI_h handle, uint_least32_t n_bits, uint_least8_t* p_buf) ;
FTDI_SPI_retval FTDI_SPI_wo(FTDI_SPI_h handle, uint_least32_t n_bits, const uint_least8_t* p_buf) ;
FTDI_SPI_retval FTDI_SPI_wr(FTDI_SPI_h handle, uint_least32_t n_bits, const uint_least8_t* p_buf_o, uint_least8_t* p_buf_i) ;


#if defined(__cplusplus)
}
#endif

#endif 
