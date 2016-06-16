/*
 * cal.c Coffee Abstraction Layer
 *
 *  Created on: 15.04.2016
 *      Author: wehmeier
 */
#include <zephyr.h>
#include <misc/printk.h>
#include <stdint.h>
#include <device.h>
#include <gpio.h>
#include <sys_clock.h>
#include "cal.h"
#include <misc/ring_buffer.h>
#include <uart.h>

DEFINE_SEMAPHORE(sem_on);
/*typedef pulse packet callback*/
typedef void (*ppcHndlr)(void);
static float ppc_dutyCycle=0.5;
static volatile uint8_t ppc_active=0;
volatile uint32_t flowticks=0;
static volatile char arrived_data[UART_BUFFERSIZE];
static struct ring_buf uart_rb;
void flow_counter_callback(struct device *port, struct gpio_callback *c, unsigned int pin)
{
	flowticks++;
	PRINT(GPIO_NAME "%d triggered\n", pin);
}

void register_flow_callback()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	if (!gpio_dev) {
		PRINT("Cannot find %s!\n", GPIO_DRV_NAME);
	}
	/* Setup GPIO input, and triggers on rising edge. */
	uint8_t ret = gpio_pin_configure(gpio_dev, GPIO_FLOW_INT,
			(GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE | GPIO_PUD_PULL_UP
			 | GPIO_INT_ACTIVE_HIGH | GPIO_INT_DEBOUNCE));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_FLOW_INT);
	}
	struct gpio_callback cb;
	gpio_init_callback(&cb, flow_counter_callback, (1<<GPIO_FLOW_INT));
	ret=gpio_add_callback(gpio_dev, &cb);
	if (ret) {
		PRINT("Cannot setup callback!\n");
	}

	ret = gpio_pin_enable_callback(gpio_dev, GPIO_FLOW_INT);
	if (ret) {
		PRINT("Error enabling callback!\n");
	}
	PRINT("registered flow counter callback");
}
void init_gpio()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	if (!gpio_dev) {
		PRINT("Cannot find %s!\n", GPIO_DRV_NAME);
	}

	/* Setup GPIO output */
	uint8_t ret = gpio_pin_configure(gpio_dev, GPIO_HEAT_PIN, (GPIO_DIR_OUT));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_HEAT_PIN);
	}
	ret = gpio_pin_configure(gpio_dev, GPIO_MILL_PIN, (GPIO_DIR_OUT));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_MILL_PIN);
	}
	ret = gpio_pin_configure(gpio_dev, GPIO_TRANSPORT_PIN, (GPIO_DIR_OUT));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_TRANSPORT_PIN);
	}
	ret = gpio_pin_configure(gpio_dev, GPIO_TRANSPORT_DIR_PIN, (GPIO_DIR_OUT));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_TRANSPORT_DIR_PIN);
	}
	ret = gpio_pin_configure(gpio_dev, GPIO_PUMP_PIN, (GPIO_DIR_OUT));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_PUMP_PIN);
	}
	/* Setup GPIO input, and triggers on rising edge. */
	ret = gpio_pin_configure(gpio_dev, GPIO_TRANSPORT_INT,
			(GPIO_DIR_IN | GPIO_INT_ACTIVE_LOW | GPIO_INT_DEBOUNCE | GPIO_PUD_PULL_UP));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_TRANSPORT_INT);
	}
	/* Setup GPIO input, and triggers on rising edge. */
	ret = gpio_pin_configure(gpio_dev, GPIO_TRANSPORT_LOW_INT,
			(GPIO_DIR_IN | GPIO_INT_ACTIVE_LOW | GPIO_INT_DEBOUNCE | GPIO_PUD_PULL_UP));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_TRANSPORT_LOW_INT);
	}
}
void heating_on()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_HEAT_PIN, 1);
}
void heating_off()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_HEAT_PIN, 0);
}
/*0-100*/
void pump_on_ppc(uint8_t dutyCycle)
{
	ppc_dutyCycle=dutyCycle/100.0f;
	PRINT("duty cycle %f", ppc_dutyCycle);
	ppc_active=1;
}
void pump_off_ppc()
{
	ppc_active=0;
	pump_off();
}
void pump_on()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_PUMP_PIN, 1);
}
void pump_off()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_PUMP_PIN, 0);
}
void transport_on()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_TRANSPORT_PIN, 1);
}
void transport_off()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_TRANSPORT_PIN, 0);
}
void mill_on()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_MILL_PIN, 1);
}
void mill_off()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_MILL_PIN, 0);
}
void mill(int ammount)
{
	if(ammount<0)
		return;
	mill_on();
	PRINT("milling %d g coffee",ammount*(MSPERGCOFFEE/MSPERTICK));
	task_sleep(ammount*(MSPERGCOFFEE/MSPERTICK));
	mill_off();
}
void transport_dir(uint8_t dir)
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	gpio_pin_write(gpio_dev, GPIO_TRANSPORT_DIR_PIN, dir);
}
void zero_crossing_callback(struct device *port, uint32_t pin)
{
	PRINT(GPIO_NAME "zero crossing\n");
}
uint8_t transport_get_posSwitch()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	uint32_t a;
	gpio_pin_read(gpio_dev,GPIO_TRANSPORT_INT,&a);
	return a?1:0;//if not pressed ->gnd
}
uint8_t transport_get_lowPosSwitch()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	uint32_t a;
	gpio_pin_read(gpio_dev,GPIO_TRANSPORT_LOW_INT,&a);
	return a?0:1;//if pressed ->gnd
}
void register_zero_crossing_callback()
{
	struct device *gpio_dev = device_get_binding(GPIO_DRV_NAME);
	if (!gpio_dev) {
		PRINT("Cannot find %s!\n", GPIO_DRV_NAME);
	}
	/* Setup GPIO input, and triggers on rising edge. */
	uint8_t ret = gpio_pin_configure(gpio_dev, GPIO_ZERO_INT,
			(GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE
			 | GPIO_INT_ACTIVE_HIGH | GPIO_INT_DEBOUNCE));
	if (ret) {
		PRINT("Error configuring " GPIO_NAME "%d!\n", GPIO_ZERO_INT);
	}
		ret = gpio_set_callback(gpio_dev, flow_counter_callback);
	if (ret) {
		PRINT("Cannot setup callback!\n");
	}
		ret = gpio_pin_enable_callback(gpio_dev, GPIO_ZERO_INT);
	if (ret) {
		PRINT("Error enabling callback!\n");
	}
}
uint16_t getFlow_ml()
{
	return flowticks/FLOW_TICKSPERML;
}
void reset_flow()
{
	flowticks=0;
}
void transport_bottom()
{
	transport_dir(0);
	transport_on();
	PRINT("transport down on\r\n");
	while(!transport_get_lowPosSwitch())
		task_sleep(10);
	PRINT("transport switch hit\r\n");
	transport_off();
}
void transport_top()
{
	transport_dir(1);
	transport_on();
	PRINT("transport top on\r\n");
	while(!transport_get_posSwitch())
		task_sleep(10);
	PRINT("transport switch hit\r\n");
	transport_off();
}
void pump_ml(uint16_t ml)
{
	pump_on();
	PRINT("pump on\r\n");
	while(getFlow_ml()<ml)
		task_sleep(10);
	pump_off();
}
void run_gpio_callbacks()
{
}
void ppc_task(void)
{
	PRINT("ppc task started");
	ppc_dutyCycle=0.5;
	ktimer_t t_on=task_timer_alloc();
	task_timer_start(t_on, 50,100,sem_on);//1000.0f/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ*ppc_dutyCycle, 1000.0f/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ, sem_on);
	PRINT("started timer, ticks %d, cycle %d\r\n",100,((int)ppc_dutyCycle*1000));//((1000/MSPERTICK)*(((float)PPC_NUMPACKETS)/PPC_FREQ))*ppc_dutyCycle));
	while(1)
	{
		task_sem_take(sem_on,TICKS_UNLIMITED);
		if(ppc_active)
		{
			pump_on();
			PRINT("ppc on\r\n");
		}
		task_timer_start(t_on, 50,100,sem_on);//, 1000/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ*(1-ppc_dutyCycle), 1000/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ, sem_on);
		task_sem_take(sem_on,TICKS_UNLIMITED);
		if(ppc_active)
		{
			pump_off();
			PRINT("ppc off\r\n");
		}
		task_timer_start(t_on, 50,100,sem_on);//, 1000/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ*(ppc_dutyCycle), 1000/MSPERTICK*PPC_NUMPACKETS/PPC_FREQ, sem_on);
	}
}
void init_cal()
{
	init_gpio();
	register_flow_callback();
	printk("cal init done..\r\n");
}

/* Registers */
#define ADC_CR	        (*((volatile uint32_t *)(0x400C0000)))
#define ADC_MR	        (*((volatile uint32_t *)(0x400C0004)))
#define ADC_CHER	(*((volatile uint32_t *)(0x400C0010)))
#define ADC_CHDR	(*((volatile uint32_t *)(0x400C0014)))
#define ADC_LCDR	(*((volatile uint32_t *)(0x400C0020)))
#define ADC_CDR0	(*((volatile uint32_t *)(0x400C0050)))
#define ADC_CDR1	(*((volatile uint32_t *)(0x400C0054)))
#define ADC_CDR2	(*((volatile uint32_t *)(0x400C0058)))
#define ADC_CDR3	(*((volatile uint32_t *)(0x400C005c)))
#define ADC_CDR4	(*((volatile uint32_t *)(0x400C0060)))
#define ADC_CDR5	(*((volatile uint32_t *)(0x400C0064)))
#define ADC_CDR6	(*((volatile uint32_t *)(0x400C0068)))
#define ADC_CDR7	(*((volatile uint32_t *)(0x400C006c)))
#define ADC_WPMR	(*((volatile uint32_t *)(0x400C00E4)))
#define ADC_IER 	(*((volatile uint32_t *)(0x400C0024)))
#define ADC_IDR 	(*((volatile uint32_t *)(0x400C0028)))
#define ADC_IMR 	(*((volatile uint32_t *)(0x400C002c)))
#define ADC_EMR 	(*((volatile uint32_t *)(0x400C0040)))
#define NVIC_IABR       (*(volatile uint32_t *)(0xE000E300))
#define PMC_PCER1       (*(volatile uint32_t *)(0x400E0700))

/* bits */
#define ADC_CR_START	(1 << 1)


#define ADC_MR_LOWRES	(1 << 4)
#define ADC_MR_FREERUN	(1 << 7)
#define ADC_MR_PRESCAL(val)	(val << 8)
#define ADC_MR_STARTUP(val)	(1 << 16)

#define ADC_CH0	         (1 << 0)
#define ADC_CH1	         (1 << 1)
#define ADC_CH2	         (1 << 2)
#define ADC_CH3	         (1 << 3)
#define ADC_CH4	         (1 << 4)
#define ADC_CH5	         (1 << 5)
#define ADC_CH6	         (1 << 6)
#define ADC_CH7	         (1 << 7)


#define ADC_LCDR_VAL		(ADC_LCDR&0x7ff)
#define ADC_LCDR_CH		(ADC_LCDR&0xF000)

#define ADC_WPMR_WE     (0x41444300);

#define PMC_ADCEN     (1<<5)

#define ADC_EMR_TAG   (1<<24)

#define ADC_IR_DRDY   (1<<24)
#define ADC_IR_EOC0   (1<<0)
#define ADC_IR_EOC1   (1<<1)
#define ADC_IR_EOC2   (1<<2)

volatile uint32_t* adc_buf=&ADC_CDR0;

void adc_isr()
{
    printk("\r\nADC%d\r\n",ADC_LCDR_CH);
}

void adc_init()
{
    PMC_PCER1|=PMC_ADCEN;
    for(int i=0;i<0x1fff;i++);
    ADC_WPMR=ADC_WPMR_WE;
    ADC_MR |= ADC_MR_LOWRES | ADC_MR_FREERUN | ADC_MR_PRESCAL(255) | ADC_MR_STARTUP(8);
    ADC_CHER |= ADC_CH0 | ADC_CH1 | ADC_CH2 | ADC_CH3 | ADC_CH4 | ADC_CH5 | ADC_CH6 | ADC_CH7;
    ADC_EMR|=ADC_EMR_TAG;
    ADC_IER = ADC_IR_DRDY;
//    IRQ_CONNECT(37,9,adc_isr,0,0);
//    irq_enable(37);
    ADC_CR |= ADC_CR_START;
}
void adc_print(int argc, char *argv[])
{
    if(argc==1)
        for(int i=0;i<8;i++)
            printk("ADC%d val: %d \r\n",i,adc_buf[i]);
    else
        printk("ADC val: %d\r\n",adc_buf[antoi(argv[1],1)]);
}
uint16_t adc_get(uint8_t channel)
{
    return adc_buf[channel&0x7];
}

uint16_t calcDistanceGP120_mm(uint16_t adcVal)
{
    return adcVal;
}
uint16_t getRemainingWater_ml()
{
    return TANK_VOLUME_ML*(1-(((float)TANK_HEIGTH_MM)-calcDistanceGP120_mm(adc_get(TANK_IR_CHNL)))/TANK_HEIGTH_MM);
}
uint8_t getWaste_percent()
{
    return ((100*WASTEBOX_HEIGTH-100*calcDistanceGP120_mm(adc_get(WASTE_IR_CHNL))))/WASTEBOX_HEIGTH;
}
uint8_t getRemainingCoffee_percent()
{
    return ((100*COFFEEBOX_HEIGTH)-(100*calcDistanceGP120_mm(adc_get(COFFEE_IR_CHNL))))/COFFEEBOX_HEIGTH;
}
enum cupType getCupSize()
{
    if(calcDistanceGP120_mm(adc_get(CUP_SMALL_IR_CHNL))>25)
        return 0;
    enum cupType ret=smallCup;
    if(calcDistanceGP120_mm(adc_get(CUP_LARGE_IR_CHNL))<25)
        ret|=largeCup;
    return ret;
}
uint8_t getCupCapacity_ml()
{
    enum cupType c=getCupSize();
    if(c|largeCup)
        return 222;
    if(c|smallCup)
        return 111;
    if(c|noCup)
        return 0;
    return 0;
}