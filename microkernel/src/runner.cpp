/*
 * runner.cpp
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#include "runner.h"
bool runner<coffeeData_struct, job_coffee, 0>::active = 0;
coffeeData_struct runner<coffeeData_struct, job_coffee, 0>::currentJob, runner<coffeeData_struct, job_coffee, 0>::nextJob;
extern "C" void run_coffee()
{
	runner<coffeeData_struct, job_coffee, 0>::getNextJob();
	runner<coffeeData_struct, job_coffee, 0>::runJob();
}
