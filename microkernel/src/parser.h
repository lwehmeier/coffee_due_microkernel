/*
 * parser.h
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_
#include <stdint.h>

uint8_t parser_checkString(char*, int len);
uint8_t parser_addJob(char*, int len);

#endif /* SRC_PARSER_H_ */
