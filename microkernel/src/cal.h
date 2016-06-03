/*
 * cal.h
 *
 *  Created on: 15.04.2016
 *      Author: wehmeier
 */

#ifndef SRC_CAL_H_
#define SRC_CAL_H_

#include <zephyr.h>
#include <misc/printk.h>
#include <stdint.h>
#define DEBUG
#ifdef DEBUG
#define PRINT           printk
#else
static void _emptyPrint(const char *fmt, ...){}
#define PRINT _emptyPrint
#endif
#include <device.h>
#include <gpio.h>
#include <sys_clock.h>
#define GPIO_HEAT_PIN	19
#define GPIO_FLOW_INT	14
#define GPIO_ZERO_INT	27
#define GPIO_TRANSPORT_INT	13
#define GPIO_TRANSPORT_LOW_INT	25
#define GPIO_PUMP_PIN	18
#define GPIO_TRANSPORT_DIR_PIN	26
#define GPIO_TRANSPORT_PIN	21
#define GPIO_MILL_PIN	17
#define GPIO_NAME	"GPIO_"
#define UART_BUFFERSIZE 256
#define GPIO_DRV_NAME CONFIG_GPIO_ATMEL_SAM3_PORTB_DEV_NAME
#define UART_DEV_NAME "UART_0"

//number of half waves
#define PPC_NUMPACKETS 100
#define PPC_FREQ 100
#define MSPERTICK 10
#define FLOW_TICKSPERML 2
#define MSPERGCOFFEE 10
#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint32_t flowticks;
void flow_counter_callback(struct device *port, struct gpio_callback *c, unsigned int pin);
void register_flow_callback();
void init_gpio();
void heating_on();
void heating_off();
void pump_on();
void pump_off();
void transport_on();
void transport_off();
void mill_on();
void mill(int ammount);
void uart_get(char* data, unsigned *size);
void transport_bottom();
void transport_top();
void pump_ml(uint16_t ml);
void mill_off();
uint8_t transport_get_posSwitch();
uint8_t transport_get_lowPosSwitch();
void transport_dir(uint8_t dir);
void zero_crossing_callback(struct device *port, uint32_t pin);
void register_zero_crossing_callback();
void run_gpio_callbacks();
uint16_t getFlow_ml();
void reset_flow();
void init_cal();
void pump_on_ppc(uint8_t);
void pump_off_ppc();

#ifdef __cplusplus
};
#endif

#endif /* SRC_CAL_H_ */
