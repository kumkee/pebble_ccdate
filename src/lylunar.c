/* $Id: lunar.c,v 1.6 2001/10/29 05:55:39 fflee Exp $ */

//char version[] = "Lunar Version 2.2 (October 28, 2001)";

/*----------------------------------------------------------------------------


             Lunar 2.2: A Calendar Conversion Program
                                for
        Gregorian Solar Calendar and Chinese Lunar Calendar
        ---------------------------------------------------

# Copyright (C) 1988,1989,1991,1992,2001 Fung F. Lee and Ricky Yeung
#
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
#
# Revision History:
# The last version of this program was released on July 23, 1992 as
# version 2.1a.  This program was first released under the terms of 
# GNU GPL on October 28, 2001 as version 2.2. Both versions are identical
# except for the license text.
#
#
# Please send your comments and suggestions to the authors:
# Fung F. Lee	lee@umunhum.stanford.edu
# Ricky Yeung	cryeung@hotmail.com
#	
# The special "bitmap" file "lunar.bitmap" was contributed
# by Weimin Liu (weimin@alpha.ece.jhu.edu).
#
# Special thanks to Hwei Chen Ti (chetihc@nuscc.nus.sg or
# chetihc@nusvm.bitnet) who extended the tables from 2001 to 2049. 
#
----------------------------------------------------------------------------*/

/*
This document contains Highest-bit-set GuoBiao (HGB) code, as adopted by
CCDOS on IBM PC compatibles, ChineseTalk 6.0 (GB version) on Macintosh,
and cxterm on UNIX and X window.  Hence, one may need to transfer this 
document as a **binary** file.

References:
1. "Zhong1guo2 yin1yang2 ri4yue4 dui4zhao4 wan4nian2li4" by Lin2 Qi3yuan2.
   《中国阴阳日月对照万年历》．林
2. "Ming4li3 ge2xin1 zi3ping2 cui4yan2" by Xu2 Le4wu2.
   《命理革新子平粹言》．徐
3. Da1zhong4 wan4nian2li4.
   《大众万年历》
*/ 

//#include <stdlib.h>
//#include <stdio.h>
#include "lylunar.h"
#include "tables.h"


static int daysInSolarMonth[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int moon[2] = {29,30}; /* a short (long) lunar month has 29 (30) days */



int ymonth[Nyear];	/* number of lunar months in the years */
int yday[Nyear];	/* number of lunar days in the years */
int mday[Nmonth+1];	/* number of days in the months of the lunar year */

long    Solar2Day(Date*), Solar2Day1(Date*);
void	Day2Lunar(long,Date*);
int	make_yday(), make_mday(int);
void	Error(char*);


void Solar2Lunar(Date *d)
{

    long offset;

    offset = Solar2Day(d);
    
    /* A lunar day begins at 11 p.m. */
    if (d->hour == 23)
	offset++;

    Day2Lunar(offset, d);
}




#define	LeapYear(y)	( (((y)%4==0) && ((y)%100!=0)) || ((y)%400==0))
#define BYEAR		1201
/* BYEAR % 4 == 1  and BYEAR % 400 == 1 for easy calculation of leap years */
/* assert(BYEAR <= SolarFirstDate.year) */

long Solar2Day(d)
Date *d;
{
    return (Solar2Day1(d) - Solar2Day1(&SolarFirstDate));
}


/* Compute the number of days from the Solar date BYEAR.1.1 */
long Solar2Day1(d)
Date *d;
{
    long offset, delta;
    int i;

    delta = d->year - BYEAR;
    if (delta<0) Error("Internal error: pick a larger constant for BYEAR.");
    offset = delta * 365 + delta / 4 - delta / 100 + delta / 400;
    for (i=1; i< d->month; i++)
    	offset += daysInSolarMonth[i];
    if ((d->month > 2) && LeapYear(d->year))
	offset++;
    offset += d->day - 1;

    if ((d->month == 2) && LeapYear(d->year))
    {
	if (d->day > 29) Error("Day out of range.");
    }
    else if (d->day > daysInSolarMonth[d->month]) Error("Day out of range.");
    return offset;
}




void Day2Lunar(offset, d)
long offset;
Date *d;

{
    int i, m, nYear, leapMonth;
    
    nYear = make_yday();
    for (i=0; i<nYear && offset > 0; i++)
	offset -= yday[i];
    if (offset<0)
	offset += yday[--i];
    if (i==Nyear) Error("Year out of range.");
    d->year = i + LunarFirstDate.year;

    leapMonth = make_mday(i);
    for (m=1; m<=Nmonth && offset>0; m++)
	offset -= mday[m];
    if (offset<0)
	offset += mday[--m];

    d->leap = 0;	/* don't know leap or not yet */

    if (leapMonth>0)	/* has leap month */
    {
	/* if preceeding month number is the leap month,
	   this month is the actual extra leap month */
	d->leap = (leapMonth == (m - 1));

	/* month > leapMonth is off by 1, so adjust it */
	if (m > leapMonth) --m;
    }

    d->month = m;
    d->day = offset + 1;
}






void Error(s)
char	*s;
{
    //printf("%s\n",s);
    //exit(1);
}


/* Compare two dates and return <,=,> 0 if the 1st is <,=,> the 2nd */
int CmpDate(month1, day1, month2, day2)
int month1, day1, month2, day2;
{
    if (month1!=month2) return(month1-month2);
    if (day1!=day2) return(day1-day2);
    return(0);
}




/* Compute the number of days in each lunar year in the table */
int make_yday()
{
    int year, i, leap;
    long code;
    
    for (year = 0; year < Nyear; year++)
    {
	code = yearInfo[year];
	leap = code & 0xf;
	yday[year] = 0;
	if (leap != 0)
	{
	    i = (code >> 16) & 0x1;
	    yday[year] += moon[i];
	}
	code >>= 4;
	for (i = 0; i < Nmonth-1; i++)
	{
	    yday[year] += moon[code&0x1];
	    code >>= 1;
	}
	ymonth[year] = 12;
	if (leap != 0) ymonth[year]++;
    }
    return Nyear;
}


/* Compute the days of each month in the given lunar year */
int make_mday(year)
int year;
{
    int i, leapMonth;
    long code;
    
    code = yearInfo[year];
    leapMonth = code & 0xf;
    /* leapMonth == 0 means no leap month */
    code >>= 4;
    if (leapMonth == 0)
    {
	mday[Nmonth] = 0;
	for (i = Nmonth-1; i >= 1; i--)
	{
	    mday[i] = moon[code&0x1];
	    code >>= 1;
	}
    }
    else
    {
	/* 
	  There is a leap month (run4 yue4) L in this year.
	  mday[1] contains the number of days in the 1-st month;
	  mday[L] contains the number of days in the L-th month;
	  mday[L+1] contains the number of days in the L-th leap month;
	  mday[L+2] contains the number of days in the L+1 month, etc.

	  cf. yearInfo[]: info about the leap month is encoded differently.
	*/
	i = (yearInfo[year] >> 16) & 0x1;
	mday[leapMonth+1] = moon[i];
	for (i = Nmonth; i >= 1; i--)
	{
	    if (i == leapMonth+1) i--;
	    mday[i] = moon[code&0x1];
	    code >>= 1;
	}
    }
    return leapMonth;
}
