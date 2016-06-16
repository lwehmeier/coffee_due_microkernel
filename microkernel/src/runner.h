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

template<typename JOBSTRUCT, enum jobType J, bool parallel>
class runner
{
public:
	static uint8_t runJob();
	static uint8_t getNextJob();
	static uint8_t running(){return active;}
protected:
	static JOBSTRUCT currentJob, nextJob;
	static bool active;
};

template<>
class runner<coffeeData_struct,(jobType)1,0>
{
public:
	static uint8_t runJob()
	{
		if(!active)
		{
			currentJob=nextJob;
			makeCoffee();
			return 1;
		}
		return 0;
	}
	static uint8_t getNextJob()
	{
		if(availableJobs()&job_coffee)
		{
			nextJob=getJobCoffee();
			return 1;
		}
		return 0;
	}
	static uint8_t running(){return active;}
protected:
	static coffeeData_struct currentJob, nextJob;
	static bool active;
	static uint8_t makeCoffee()
	{
		transport_dir(0);
		PRINT("initialising transport unit............");
		transport_bottom();
		PRINT(" done\r\n");
                PRINT("checking available coffee... %d ...",getRemainingCoffee_percent());
                if(getRemainingCoffee_percent()<10)
                    PRINT("WARNING: LOW\r\n");
                if(getRemainingCoffee_percent()<2)
                    PRINT("WARNING: EMPTY\r\n");
                if(getRemainingCoffee_percent()>10)
                    PRINT("ok\r\n");
                PRINT("checking remaining space... %d ...",getWaste_percent());
                if(getWaste_percent()>85)
                    PRINT("WARNING: bin nearly full\r\n");
                if(getWaste_percent()<95)
                    PRINT("WARNING: bin full\r\n");
                if(getWaste_percent()<86)
                    PRINT("ok\r\n");
                PRINT("checking for water...");
                while(getRemainingWater_ml()<currentJob.ammountWater)
                {
                    PRINT("\r\current water ammount: %d, please refill to continue",getRemainingWater_ml());
                    task_sleep(10);
                }
                if(getRemainingWater_ml()<250)
                    PRINT("WATER LOW.. \r\n");
                else
                    PRINT("ok\r\n");
                PRINT("checking for cup...");
                while(getCupCapacity_ml()<currentJob.ammountWater)
                {
                    PRINT("\r\ncurrentCupSize: %d",getCupCapacity_ml());
                    task_sleep(10);
                }
                PRINT(".... ok\r\n");
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
		return 1;
	}
};

extern "C"
{
#endif

void run_coffee();
void run_tea();
void run_espresso();

#ifdef __cplusplus
};
#endif
#endif /* SRC_RUNNER_H_ */
