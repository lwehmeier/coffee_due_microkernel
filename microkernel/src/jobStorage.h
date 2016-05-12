/*
 * jobStorage.h
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#ifndef SRC_JOBSTORAGE_H_
#define SRC_JOBSTORAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

void storage_pushJob(int type, int ammountWater, int ammountCoffee, int tempWater, int steam);
int* storage_popJob(int type);
int* storage_getJob(int type);

#ifdef __cplusplus
};
#endif
#endif /* SRC_JOBSTORAGE_H_ */
