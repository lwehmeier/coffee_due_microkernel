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
//#define DEBUG
static void _emptyPrint(const char *fmt, ...){}
#ifdef DEBUG
#define PRINT           printk
#else
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
#define TANK_VOLUME_ML 600
#define TANK_HEIGTH_MM 150
#define TANK_IR_CHNL 0
#define CUP_SMALL_IR_CHNL 1
#define CUP_LARGE_IR_CHNL 2
#define WASTE_IR_CHNL 3
#define COFFEE_IR_CHNL 4
#define COFFEEBOX_HEIGTH 75
#define WASTEBOX_HEIGTH 80
//number of half waves
#define PPC_NUMPACKETS 100
#define PPC_FREQ 100
#define MSPERTICK 10
#define FLOW_TICKSPERML 2
#define MSPERGCOFFEE 10
#ifdef __cplusplus
extern "C" {
#endif
    
enum cupType {noCup=1, smallCup=2, largeCup=4};

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
void adc_init();
uint16_t adc_get(uint8_t);
uint16_t calcDistanceGP120_mm(uint16_t adcVal);
uint16_t getRemainingWater_ml();
uint8_t getWaste_percent();
uint8_t getRemainingCoffee_percent();
enum cupType getCupSize();
uint8_t getCupCapacity_ml();
#ifdef __cplusplus
};
#endif

#endif /* SRC_CAL_H_ */
