/*
 * jobStorage.cpp
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#include <stdlib.h>
#include <stdint.h>
#include "jobStorage.h"

#define MAXJOBS 5
volatile jobData_struct m_cd[MAXJOBS];
volatile uint8_t m_numCd=0;
int returns[5];
void storage_pushJob(int type, int ammountWater, int ammountCoffee, int tempWater, int steam)
{
	if(m_numCd>=MAXJOBS)
		return;
	m_numCd++;
	m_cd[m_numCd-1].ammountWater=ammountWater;
	m_cd[m_numCd-1].ammountCoffee=ammountCoffee;
	m_cd[m_numCd-1].ammountSteam=steam;
	m_cd[m_numCd-1].waterTemperature=tempWater;
	m_cd[m_numCd-1].jobType=type;

}
int* storage_popJob(int type)
{
	if(m_numCd==0)
		return 0;
	int i=0;
	for(;i<m_numCd&&m_cd[i].jobType!= type;i++);
	if(i>=m_numCd)
		return 0;
	returns[1]=m_cd[i].ammountWater;
	returns[2]=m_cd[i].ammountCoffee;
	returns[4]=m_cd[i].ammountSteam;
	returns[3]=m_cd[i].waterTemperature;
	returns[0]=m_cd[i].jobType;
	m_numCd--;
	return returns;
}
int* storage_getJob(int type)
{
	if(m_numCd==0)
		return 0;
	int i=0;
	for(;i<m_numCd&&m_cd[i].jobType!= type;i++);
	if(i>=m_numCd)
		return 0;
	returns[1]=m_cd[i].ammountWater;
	returns[2]=m_cd[i].ammountCoffee;
	returns[4]=m_cd[i].ammountSteam;
	returns[3]=m_cd[i].waterTemperature;
	returns[0]=m_cd[i].jobType;
	return returns;
}
