/*
 * jobstorage-connector.h
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#pragma once

#ifdef __cplusplus
extern "C"{
#endif

struct coffeeData_struct {int ammountCoffee/*gramm*/; int waterTemperature; int ammountWater;/*ml*/; int ammountSteam;};
struct teaData_struct {int waterTemperature; int ammountWater;/*ml*/};
typedef struct coffeeData_struct coffeeData;
typedef struct teaData_struct teaData;
typedef coffeeData espressoData;
enum jobType {job_coffee=1, job_espresso=2, job_tea=4};
enum jobType availableJobs();
coffeeData getJobCoffee();
espressoData getJobEspresso();
teaData getJobTea();

uint8_t registerJobCoffee(coffeeData);
uint8_t registerJobEspresso(espressoData);
uint8_t registerJobTea(teaData);


#ifdef __cplusplus
};
#endif
