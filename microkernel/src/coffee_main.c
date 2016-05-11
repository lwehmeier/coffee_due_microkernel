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
#include "cal.h"

#define TAKE(x) task_mutex_lock(x, TICKS_UNLIMITED)
#define GIVE(x) task_mutex_unlock(x)
#define SLEEP(x) task_sleep(x)

#define RANDDELAY(x) myDelay(((sys_tick_get_32() * ((x) + 1)) & 0x2f) + 1)

#define TEST_EVENT_ID 255

#define STSIZE 1024
char __stack kernel_event_logger_stack[2][STSIZE];

struct context_switch_data_t {
	uint32_t thread_id;
	uint32_t last_time_executed;
	uint32_t count;
};

int total_dropped_counter;

#define MAX_BUFFER_CONTEXT_DATA       20

struct context_switch_data_t
	context_switch_summary_data[MAX_BUFFER_CONTEXT_DATA];

unsigned int interrupt_counters[255];


struct sleep_data_t {
	uint32_t awake_cause;
	uint32_t last_time_slept;
	uint32_t last_duration;
};

struct sleep_data_t sleep_event_data;

int is_busy_task_awake;
int forks_available = 1;

struct tmon_data_t {
	uint32_t event_type;
	uint32_t timestamp;
	uint32_t task_id;
	uint32_t data;
};

uint32_t tmon_index;

struct tmon_data_t
	tmon_summary_data[MAX_BUFFER_CONTEXT_DATA];
void busy_task_entry(void)
{
	int ticks_when_awake;
	int i;

	while (1) {
		/*
		 * go to sleep for 1000 ticks allowing the system entering to sleep
		 * mode if required.
		 */
		is_busy_task_awake = 0;
		SLEEP(1000);
		ticks_when_awake = sys_tick_get_32();

		/*
		 * keep the cpu busy for 1000 ticks preventing the system entering
		 * to sleep mode.
		 */
		is_busy_task_awake = 1;
		while (sys_tick_get_32() - ticks_when_awake < 1000) {
			i++;
		}
	}
}
void register_context_switch_data(uint32_t timestamp, uint32_t thread_id)
{
	int found;
	int i;

	found = 0;
	for (i = 0; (i < MAX_BUFFER_CONTEXT_DATA) && (found == 0); i++) {
		if (context_switch_summary_data[i].thread_id == thread_id) {
			context_switch_summary_data[i].last_time_executed = timestamp;
			context_switch_summary_data[i].count += 1;
			found = 1;
		}
	}

	if (!found) {
		for (i = 0; i < MAX_BUFFER_CONTEXT_DATA; i++) {
			if (context_switch_summary_data[i].thread_id == 0) {
				context_switch_summary_data[i].thread_id = thread_id;
				context_switch_summary_data[i].last_time_executed = timestamp;
				context_switch_summary_data[i].count = 1;
				break;
			}
		}
	}
}

void register_interrupt_event_data(uint32_t timestamp, uint32_t irq)
{
	if ((irq >= 0) && (irq < 255)) {
		interrupt_counters[irq] += 1;
	}
}


void register_sleep_event_data(uint32_t time_start, uint32_t duration,
	uint32_t cause)
{
	sleep_event_data.awake_cause = cause;
	sleep_event_data.last_time_slept = time_start;
	sleep_event_data.last_duration = duration;
}

void register_tmon_data(uint32_t event_type, uint32_t timestamp,
	uint32_t task_id, uint32_t data)
{
	tmon_summary_data[tmon_index].event_type = event_type;
	tmon_summary_data[tmon_index].timestamp = timestamp;
	tmon_summary_data[tmon_index].task_id = task_id;
	tmon_summary_data[tmon_index].data = data;

	if (++tmon_index == MAX_BUFFER_CONTEXT_DATA) {
		tmon_index = 0;
	}
}
void coffee_test_entry(void)
{
	PRINT("loading kernel ....... done\r\n");
	init_cal();
	PRINT("gpio init .... done\r\n");
	task_sleep(50);
	transport_dir(1);
	PRINT("init ........... done\r\n");
	PRINT("initialising transport unit............");
	transport_bottom();
	PRINT(" done\r\n");
	PRINT("starting coffee brewing..");
	transport_dir(1);
	transport_on();
	PRINT("transport on\r\n");
	task_sleep(200);
	transport_off();
	mill_on();
	PRINT("mill on\r\n");
	task_sleep(650);
	mill_off();
	task_sleep(200);//wait
	transport_top();
	task_sleep(75);
	PRINT("heating on\r\n");
	heating_on();
	task_sleep(500);
	pump_ml(200);
	heating_off();
	task_sleep(125);
	pump_ml(20);
	task_sleep(350);
	transport_bottom();
	PRINT("done\r\n");
	PRINT("total flow ticks were: %d\r\n\r\n",flowticks);

	while(1)
		task_sleep(1000);
}
/**
 * @brief Start the demo fibers
 *
 * @details Start the kernel event data colector fiber and the summary printer
 * fiber that shows the context switch data.
 *
 * @return No return value.
 */
void kernel_event_logger_fiber_start(void)
{
/*	PRINTF("\x1b[2J\x1b[15;1H");
	task_fiber_start(&kernel_event_logger_stack[0][0], STSIZE,
		(nano_fiber_entry_t) profiling_data_collector, 0, 0, 6, 0);
	task_fiber_start(&kernel_event_logger_stack[1][0], STSIZE,
		(nano_fiber_entry_t) summary_data_printer, 0, 0, 6, 0);*/
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
	kernel_event_logger_fiber_start();

	task_group_start(PHI);
}
