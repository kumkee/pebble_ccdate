#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "lylunar.h"

#define zhLen 3

//TODO: move these to function scope
char ZhDigit[10][zhLen+1] = { "正", "一", "二", "三", "四", "五", "六", "七", "八", "九" };

char ZhDigit2[3][zhLen+1] = { "初", "十", "廿" }; //digit in ten's place

char ZhLeap[] = "閏";

char ZhMonth[] = "月";

char* ZhTen[3] = { ZhDigit2[0], ZhDigit[2], ZhDigit[3] }; //初, 二, 三


#define MY_UUID { 0xDF, 0x64, 0x37, 0x11, 0x9B, 0xF3, 0x43, 0x49, 0xBA, 0x47, 0xA4, 0x3D, 0x37, 0xB7, 0xDC, 0x8A }
PBL_APP_INFO(MY_UUID,
             "Chinese Calendar Date", "kumkee",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer text_cdate_layer; //Tradictional chinese date display
TextLayer text_gdate_layer; //Gregorian calendar date
TextLayer text_time_layer;

Layer line_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}


const bool ZhDisplay = true;

void handle_init(AppContextRef ctx) {

  window_init(&window, "ChineseCalendar");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);


  text_layer_init(&text_cdate_layer, window.layer.frame);
  text_layer_set_text_color(&text_cdate_layer, GColorWhite);
  text_layer_set_background_color(&text_cdate_layer, GColorClear);
  layer_set_frame(&text_cdate_layer.layer, GRect(6, 46, 128, 56));
  text_layer_set_font(&text_cdate_layer, 
	fonts_load_custom_font(resource_get_handle(
		ZhDisplay ? RESOURCE_ID_FONT_IPA_SUBSET_21 : RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_cdate_layer.layer);


  text_layer_init(&text_gdate_layer, window.layer.frame);
  text_layer_set_text_color(&text_gdate_layer, GColorWhite);
  text_layer_set_background_color(&text_gdate_layer, GColorClear);
  layer_set_frame(&text_gdate_layer.layer, GRect(8, 68, 144-8, 168-68));
  text_layer_set_font(&text_gdate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_gdate_layer.layer);


  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);


  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);


  // TODO: Update display here to avoid blank display on launch?

}


void CDateDisplayNo(Date *d, char* text)
{
  int i;
  i = d->leap?1:0;
	
  if(i)	text[0] = 'L';
  text[i+0] = d->month/10 + '0';
  text[i+1] = d->month%10 + '0';
  text[i+2] = 'm';
  text[i+3] = '/';
  text[i+4] = d->day/10 + '0';
  text[i+5] = d->day%10 + '0';
  text[i+6] = 'd';
  text[i+7] = 0;
}


void CDateDisplayZh(Date *d, char* text)
{
  int i,j;
  int place = 0;
  i = d->leap?1:0;
  j = (d->month-1)/10==0 ? 0 : 1;

  if(i)	memcpy(text, ZhLeap, zhLen);

  if(j) memcpy(text + i*zhLen, ZhDigit2[1], zhLen); //十 of 十某月

  if(d->month==1)	memcpy(text + (place+i+j)*zhLen, ZhDigit[0], zhLen); //正 of 正月
  else if(d->month==10)	memcpy(text + (place+i+j)*zhLen, ZhDigit2[1], zhLen);   //十 of 十月
  else 			memcpy(text + (place+i+j)*zhLen, ZhDigit[d->month%10], zhLen); //某 of 十某月 or 某月

  place++;

  memcpy(text + (place+i+j)*zhLen, ZhMonth, zhLen);
  place++;

  if(d->day%10==0){
	memcpy(text + (place+i+j)*zhLen, ZhTen[d->day/10 - 1], zhLen); //某 of 某十
	place++;
	memcpy(text + (place+i+j)*zhLen, ZhDigit2[1], zhLen); //十 of 某十
	place++;
  }
  else{
	memcpy(text + (place+i+j)*zhLen, ZhDigit2[d->day/10], zhLen); //某 of 某甲
	place++;
	memcpy(text + (place+i+j)*zhLen, ZhDigit[d->day%10], zhLen); //甲 of 某甲
	place++;
  }

  text[(place+i+j)*zhLen] = 0;
  
}


void GenerateCDateText(PblTm *t, char* cdtext)
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


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *evt) {

  static char ccd_text[] = "LLLTTTDDDYYYCCCDDD";
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;


  GenerateCDateText(evt->tick_time, ccd_text);

  text_layer_set_text(&text_cdate_layer, ccd_text);


  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%B %e", evt->tick_time);
  text_layer_set_text(&text_gdate_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, evt->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };

  app_event_loop(params, &handlers);
}
