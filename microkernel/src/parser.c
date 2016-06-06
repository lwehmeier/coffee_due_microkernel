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
#include "jsmn.h"
#include "utils.h"
#include "jobstorage-connector.h"

uint8_t parser_checkString(char* data, int len)
{
	char *start=0, *end=0;
	if(data[len]!=0)//catch unterminated strings
	{
		PRINT("PARSER: inv. string\r\n");
		return false;
	}
	start=strchr(data,'{');
	end=strrchr(data,'}');
	if(end <= start)
	{
		PRINT("PARSER.1: inv. string\r\n");
		return false;
	}
	PRINT("PARSER: valid\r\n");
	return true;
}
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}
uint8_t parser_addJob(char* data, int len)
{
	if(!parser_checkString(data,len))
		return false;
	coffeeData c;
	c.ammountCoffee=0;
	c.ammountWater=0;
	c.ammountSteam=0;
	c.waterTemperature=0;

	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[10]; /* We expect no more than 128 tokens */
	char* start=strchr(data,'{');

	jsmn_init(&p);
	r = jsmn_parse(&p, start, strlen(start), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		PRINT("Failed to parse JSON: %d\r\n", r);
		return 1;
	}
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		PRINT("Object expected\r\n");
		return 1;
	}
	for (i = 1; i < r; i++)
	{
		if (jsoneq(start, &t[i], "ammountCoffee") == 0) {
			/* We may use strndup() to fetch string value */
			PRINT("- aC: %d\r\n", antoi(start + t[i+1].start,t[i+1].end-t[i+1].start));
			c.ammountCoffee=antoi(start + t[i+1].start,t[i+1].end-t[i+1].start);
			i++;
		}
		else if (jsoneq(start, &t[i], "type") == 0)
		{
			/* We may want to do strtol() here to get numeric value */
			PRINT("- type: %d\n",antoi(start + t[i+1].start,t[i+1].end-t[i+1].start));
			i++;
		}
		else if (jsoneq(start, &t[i], "ammountWater") == 0)
		{
			/* We may want to do strtol() here to get numeric value */
			PRINT("- water: %d\n",antoi(start + t[i+1].start,t[i+1].end-t[i+1].start));
			c.ammountWater=antoi(start + t[i+1].start,t[i+1].end-t[i+1].start);
			i++;
		}
		else if (jsoneq(start, &t[i], "temperatureWater") == 0)
		{
			/* We may want to do strtol() here to get numeric value */
			PRINT("- temperature: %d\n",antoi(start + t[i+1].start,t[i+1].end-t[i+1].start));
			c.waterTemperature=antoi(start + t[i+1].start,t[i+1].end-t[i+1].start);
			i++;
		}
		else if (jsoneq(start, &t[i], "ammountSteam") == 0)
		{
			/* We may want to do strtol() here to get numeric value */
			PRINT("- steam: %d\n",antoi(start + t[i+1].start,t[i+1].end-t[i+1].start));
			c.ammountSteam=antoi(start + t[i+1].start,t[i+1].end-t[i+1].start);
			i++;
		}
		else
		{
			PRINT("Unexpected key: \r\n");
		}
	}
	return 0;
}
