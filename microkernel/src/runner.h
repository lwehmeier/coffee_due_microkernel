/*
 * runner.h
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include "jobstorage-connector.h"
#include "cal.h"
#ifdef __cplusplus

template<typename JOBSTRUCT, jobType J, bool parallel=0>
class runner
{
public:
	static uint8_t runJob()=0;
	static uint8_t getNextJob()=0;
	static uint8_t running(){return active;}
protected:
	static JOBSTRUCT currentJob, nextJob;
	static bool active;
};

template<bool parallel=0>
class runner<coffeeData,job_coffee>
{
public:
	static uint8_t runJob()
	{
		if(!active)
		{
			currentJob=nextJob;
		}
	}
	static uint8_t getNextJob()
	{
		if(availableJobs()&job_coffee)
			nextJob=getJobCoffee();
	}
protected:
	uint8_t makeCoffee()
	{
		transport_dir(0);
		PRINT("initialising transport unit............");
		transport_bottom();
		PRINT(" done\r\n");
		PRINT("starting coffee brewing..");
		transport_dir(1);
		transport_on();
		PRINT("transport on\r\n");
		task_sleep(200);
		transport_off();
		PRINT("milling coffee\r\n");
		mill(currentJob.ammountCoffee);
		task_sleep(20);//wait
		PRINT("compressing coffee\r\n");
		transport_top();
		task_sleep(75);
		PRINT("preheating on\r\n");
		heating_on();
		task_sleep(500);
		PRINT("pump @ %d percent, %d ml", 100,currentJob.ammountWater);
		pump_ml(currentJob.ammountWater-25);
		heating_off();
		task_sleep(100);
		pump_ml(25);
		task_sleep(25);
		transport_bottom();
		PRINT("done\r\n");
		PRINT("total flow ticks were: %d\r\n\r\n",flowticks);
	}
};


#endif



#endif /* SRC_RUNNER_H_ */
