#ifndef LYLUNAR_H
#define LYLUNAR_H
#include "pebble_os.h"

typedef char byte;

typedef struct {
	int year, month, day, hour, weekday;
	int leap;	/* the lunar month is a leap month */
} Date;

void Solar2Lunar(Date*);

void CDateDisplayNo(Date*, char*);
void CDateDisplayZh(Date*, char*);
void GenerateCDateText(PblTm*, char*,bool);

#endif
