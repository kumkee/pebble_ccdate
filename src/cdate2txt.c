#include "lylunar.h"


void CDateDisplayNo(Date *d, char* text)
{
  int i;
  i = d->leap?1:0;
	
  if(i)	text[0] = 'L';
  text[i+0] = d->month/10 + '0';
  text[i+1] = d->month%10 + '0';
  text[i+2] = 'M';
  text[i+3] = '/';
  text[i+4] = d->day/10 + '0';
  text[i+5] = d->day%10 + '0';
  text[i+6] = 'D';
  text[i+7] = 0;
}


#define zhLen 3
#define MvChar(zhi) memcpy(text + place*zhLen, zhi, zhLen)
void CDateDisplayZh(Date *d, char* text)
{
  char ZhDigit[10][zhLen+1] = { "正", "一", "二", "三", "四", "五", "六", "七", "八", "九" };
  char ZhDigit2[3][zhLen+1] = { "初", "十", "廿" }; //digit in ten's place
  char ZhLeap[] = "閏";
  char ZhMonth[] = "月";
  char* ZhTen[3] = { ZhDigit2[0], ZhDigit[2], ZhDigit[3] }; //初, 二, 三

  int i,j;
  int place = 0;
  i = d->leap?1:0;
  j = (d->month-1)/10==0 ? 0 : 1;

  if(i)	MvChar(ZhLeap);	//閏

  place += i;
  if(j) MvChar(ZhDigit2[1]);		//十 of 十某月

  place += j;
  if(d->month==1)	MvChar(ZhDigit[0]);		//正 of 正月
  else if(d->month==10)	MvChar(ZhDigit2[1]);		//十 of 十月
  else 			MvChar(ZhDigit[d->month%10]);	//某 of 十某月 or 某月

  place++;

  MvChar(ZhMonth);	//月
  place++;

  if(d->day%10==0){
	MvChar(ZhTen[d->day/10 - 1]);	//某 of 某十日
	place++;
	MvChar(ZhDigit2[1]);		//十 of 某十日
	place++;
  }
  else{
	MvChar(ZhDigit2[d->day/10]);	//某 of 某甲日
	place++;
	MvChar(ZhDigit[d->day%10]);	//甲 of 某甲日
	place++;
  }

  text[place*zhLen] = 0;
  
}


void GenerateCDateText(PblTm *t, char* cdtext, bool ZhDisplay)
{

  Date today;

  today.year  = t->tm_year + 1900;
  today.month = t->tm_mon + 1;
  today.day   = t->tm_mday;
  today.hour  = t->tm_hour;

  Solar2Lunar(&today);

  if(ZhDisplay) 
	CDateDisplayZh(&today,cdtext);

  else
	CDateDisplayNo(&today,cdtext);
  
}

