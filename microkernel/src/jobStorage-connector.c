/*
 * jobStorage-connector.c
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#include <stdint.h>
#include "jobstorage-connector.h"
#include "jobStorage.h"

enum jobType availableJobs()
{
	return (storage_getJob(job_coffee)?job_coffee:0) | (storage_getJob(job_tea)?job_tea:0) | (storage_getJob(job_espresso)?job_espresso:0);
}
coffeeData getJobCoffee()
{
	coffeeData dat;
	int *res=storage_getJob(job_coffee);
	dat.ammountCoffee=res[2];
	dat.ammountWater=res[1];
	dat.waterTemperature=res[3];
	dat.ammountSteam=res[4];
	return dat;
}
espressoData getJobEspresso()
{
	espressoData dat;
	int *res=storage_getJob(job_espresso);
	dat.ammountCoffee=res[2];
	dat.ammountWater=res[1];
	dat.waterTemperature=res[3];
	dat.ammountSteam=res[4];
	return dat;
}
teaData getJobTea()
{
	teaData dat;
	int *res=storage_getJob(job_tea);
	dat.ammountWater=res[1];
	dat.waterTemperature=res[3];
	return dat;
}

uint8_t registerJobCoffee(coffeeData dat)
{
	storage_pushJob(job_coffee,dat.ammountWater,dat.ammountCoffee,dat.waterTemperature,dat.ammountSteam);
	return 1;
}
uint8_t registerJobEspresso(espressoData dat)
{
	storage_pushJob(job_espresso,dat.ammountWater,dat.ammountCoffee,dat.waterTemperature,dat.ammountSteam);
	return 1;
}
uint8_t registerJobTea(teaData dat)
{
	storage_pushJob(job_tea,dat.ammountWater,-1,dat.waterTemperature,-1);
	return 1;
}
