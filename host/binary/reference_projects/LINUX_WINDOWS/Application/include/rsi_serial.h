/**
 *  @file     rsi_serial.h
 *  @version  1.0
 *  @date     2014-Sep-25
 *
 *  Copyright(C) Redpine Signals 2014
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief API: Definitions of Various UART related parameters
 *
 *  @section Description
 *  This file contain definition of various application defined macros.
 *
 *  @section Improvements
 *
 */


/**
 * Includes
 * */
#ifndef _RSI_SERIAL_H
#define _RSI_SERIAL_H

#define BAUDRATE            B115200
#ifndef RSI_UART_DEVICE
#define RSI_UART_DEVICE     "/dev/ttyUSB0"
#endif

#ifdef WINDOWS
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
#endif

/* Application Specific Defines */
#define RSI_PRE_DESC_LEN         4

/* RX Progrss States
 * 0 - IDLE STATE
 * 1 - LENGTH DESCRIPTOR RECEIVE STATE
 * 2 - PAYLOAD RECEIVE STATE
 */
typedef enum {
  IDLE_STATE = 0,
  LEN_DESC_RECV_STATE,
  WAIT_FOR_DESC_STATE,
  PAYLOAD_RECV_STATE
} RX_PROG_STATE;

int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_serial_init(void);
uint8_t rsi_uart_byte_read(void);
uint8_t rsi_serial_fifo_status(void);

#endif
 
