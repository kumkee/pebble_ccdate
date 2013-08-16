#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "lylunar.h"


#define MY_UUID { 0xDF, 0x64, 0x37, 0x11, 0x9B, 0xF3, 0x43, 0x49, 0xBA, 0x47, 0xA4, 0x3D, 0x37, 0xB7, 0xDC, 0x8A }
PBL_APP_INFO(MY_UUID,
             "Chinese Calendar Date", "kumkee",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer text_cdate_layer; //Tradictional chinese date display
TextLayer text_gdate_layer; //Gregorian calendar date
TextLayer text_time_layer;

void handle_init(AppContextRef ctx) {

  window_init(&window, "ChineseCalendar");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);


  text_layer_init(&text_cdate_layer, window.layer.frame);
  text_layer_set_text_color(&text_cdate_layer, GColorWhite);
  text_layer_set_background_color(&text_cdate_layer, GColorClear);
  layer_set_frame(&text_cdate_layer.layer, GRect(8, 56, 128, 56));
  text_layer_set_font(&text_cdate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_cdate_layer.layer);

}


void GenerateCDateText(PblTm *t, char* cdtext, bool ZhChars)
{

  Date today;
  int i;

  today.year  = t->tm_year + 1900;
  today.month = t->tm_mon + 1;
  today.day   = t->tm_mday;
  today.hour  = t->tm_hour;

  Solar2Lunar(&today);
  i = today.leap?1:0;

  if(i)
	{ cdtext[0] = 'L'; }
  cdtext[i+0] = today.month/10 + '0';
  cdtext[i+1] = today.month%10 + '0';
  cdtext[i+2] = '|';
  cdtext[i+3] = today.day/10 + '0';
  cdtext[i+4] = today.day%10 + '0';
  cdtext[i+5] = 0;
  
}


void handle_hour_tick(AppContextRef ctx, PebbleTickEvent *evt) {

  static char ccd_text[] = "L00|00";

  GenerateCDateText(evt->tick_time, ccd_text, false);

  text_layer_set_text(&text_cdate_layer, ccd_text);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_hour_tick,
      .tick_units = HOUR_UNIT
    }

  };

  app_event_loop(params, &handlers);
}
