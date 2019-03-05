/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_usart_sync.h>
#include <tc_lite.h>
#include <tc_lite.h>
#include <hal_can_async.h>

extern struct usart_sync_descriptor UART_EDBG;
extern struct can_async_descriptor  CAN_0;

void UART_EDBG_PORT_init(void);
void UART_EDBG_CLOCK_init(void);
void UART_EDBG_init(void);

void TC_0_CLOCK_init(void);

int8_t TC_0_init(void);

void TC_0_SLAVE_CLOCK_init(void);

int8_t TC_0_SLAVE_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
