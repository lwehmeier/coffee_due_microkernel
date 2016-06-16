/* kernel_event_collector_sample.c - Kernel event collector sample project */

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zephyr.h>
#include <misc/kernel_event_logger.h>
#include <string.h>
#include <stdint.h>
#include <misc/shell.h>
#include <irq.h>
#define DEVICE_NAME "test shell"
#include "cal.h"
#include "parser.h"
#include "runner.h"
#include "utils.h"
#include "jobstorage-connector.h"


#ifdef PRINT
#undef PRINT
#endif
#define PRINT _emptyPrint

static uint8_t initialized=0;

void init()
{

	parser_addJob("{}",3);
	PRINT("loading kernel ....... done\r\n");
	init_cal();
	PRINT("CAL init .... done\r\n");
	task_sleep(50);
	transport_dir(1);
	PRINT("initialising transport unit............");
	//transport_bottom();
	PRINT(" done\r\n");
	PRINT("init ........... done\r\n");
	initialized=true;
}

static void shell_add_job(int argc, char *argv[])
{
	if(argc<1)
		return;
	PRINT("adding %s\r\n",argv[1]);
	parser_addJob(argv[1],strlen(argv[1]));
}
static void shell_add_coffee(int argc, char *argv[])
{
	if(argc<4)
	{
		PRINT("too few arguments. Usage \n coffee $water $coffee $temperature\r\n");
		return;
	}
	coffeeData c;
	c.ammountCoffee=antoi(argv[1],3);
	c.ammountWater=antoi(argv[2],3);
	c.waterTemperature=antoi(argv[3],3);
	c.ammountSteam=0;
	registerJobCoffee(c);
}
static void shell_list_jobs(int argc, char *argv[])
{
	printk("currently available jobtypes:\r\n");
	printk("    coffee: %d\r\n", availableJobs()&job_coffee);
	printk("    espresso: \r\n", availableJobs()&job_coffee);
	printk("    tea: \r\n", availableJobs()&job_coffee);
}
void adc_init();
void adc_print(int,char**);
const struct shell_cmd commands[] = {
		{ "add", shell_add_job },
		{ "coffee", shell_add_coffee },
		{ "ls", shell_list_jobs },
		{ "adc_init", adc_init },
		{ "adc_get", adc_print },
	{ NULL, NULL }
};
void coffee_test_entry(void)
{
	init();
	shell_init("shell> ", commands);
	task_sleep(1000);
	while(1)
	{
		PRINT("trying next Job \r\n");
		run_coffee();
		task_sleep(100);
	}

	while(1)
		task_sleep(1000);
}

/**
 * @brief Microkernel task.
 *
 * @details Start the kernel event data colector fiber. Then
 * do wait forever.
 *
 * @return No return value.
 */
void k_event_logger_demo(void)
{
	task_group_start(PHI);
}

