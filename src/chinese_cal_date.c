#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "lylunar.h"

const bool ZhDisplay = true;


#define MY_UUID { 0xDF, 0x64, 0x37, 0x11, 0x9B, 0xF3, 0x43, 0x49, 0xBA, 0x47, 0xA4, 0x3D, 0x37, 0xB7, 0xDC, 0x8A }
PBL_APP_INFO(MY_UUID,
             "Chinese Calendar Date", "kumkee",
             1, 2 /* App version */,
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer text_cdate_layer; //Tradictional chinese date display
TextLayer text_gdate_layer; //Gregorian calendar date
TextLayer text_time_layer;

static bool is_cdate_drawn = false;
static bool is_gdate_drawn = false;

Layer line_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}


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

}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *evt) {

  static char ccd_text[] = "LLLTTTDDDYYYCCCDDD";
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }


  if( ((evt->units_changed & HOUR_UNIT) &&evt->tick_time->tm_hour==23) || (!is_cdate_drawn)){
    GenerateCDateText(evt->tick_time, ccd_text, ZhDisplay);
    text_layer_set_text(&text_cdate_layer, ccd_text);
    is_cdate_drawn = true;
  }


  if( (evt->units_changed & DAY_UNIT) || (!is_gdate_drawn) ){
    string_format_time(date_text, sizeof(date_text), "%B %e", evt->tick_time);
    text_layer_set_text(&text_gdate_layer, date_text);
    is_gdate_drawn = true;
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
