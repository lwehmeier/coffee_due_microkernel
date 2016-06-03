/*
 * parser.c
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#include <stdlib.h>
#include <stdio.h>
#include <zephyr.h>
#include <stdint.h>
#include <string.h>
#include "cal.h"
#include "frozen.h"
#include "utils.h"
#include "jobstorage-connector.h"

uint8_t parser_checkString(char* data, int len)
{
	char *start=0, *end=0;
	if(data[len]!=0)//catch unterminated strings
		return false;
	start=strchr(data,'{');
	end=strrchr(data,'}');
	if(end <= start)
		return false;
	if(parse_json(start,end-start,0,0)==end-data)//returns end of parsing, iff json is valid == closing "}"
		return true;
	return false;
}

uint8_t parser_addJob(char* data, int len)
{
	if(!parser_checkString(data,len))
		return false;
	char *start=0, *end=0;
	start=strchr(data,'{');
	end=strrchr(data,'}');
	struct json_token *arr=0, *tok=0;
	arr = parse_json2(start,end-start);
	if(arr)
		tok = find_json_token(arr,"Type");
	if(tok)
	{
		char token[100];
		snprintf(token, sizeof(token),"%.*s",tok->len, tok->ptr);
		PRINT("Valid Type found, type is: %s",token);
		switch(antoi(tok->ptr, tok->len))//switch type (1 coffee, 2 espresso, etc
		{
		case 1: //coffee
			tok = find_json_token(arr,"ammountCoffee");
			coffeeData c;
			if(tok->ptr)
				c.ammountCoffee=antoi(tok->ptr,tok->len);
			tok = find_json_token(arr,"ammountWater");
			if(tok->ptr)
				c.ammountWater=antoi(tok->ptr,tok->len);
			tok = find_json_token(arr,"temperature");
			if(tok->ptr)
				c.waterTemperature=antoi(tok->ptr,tok->len);
			registerJobCoffee(c);
			break;
		default:
			PRINT("INVALID JOBTYPE");
		}
	}
	return 0;
}
