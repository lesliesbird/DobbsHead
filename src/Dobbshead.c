#include "pebble.h"

Window *window;
TextLayer *text_time_layer;
TextLayer *AMPMLayer;
GBitmap *bob;
GBitmap *subliminal;
BitmapLayer *background_layer;
BitmapLayer *subliminal_layer;

static char time_text[] = "00:00";
static char AMPM[] = "XX";
int sub_chance = 4;
int show_sub = 0;
int subgenius, show_duration;
int toggle_sub = 1;

void boo (void) {
    
    subgenius = rand() % 8;
    gbitmap_destroy(subliminal);
    subliminal = gbitmap_create_with_resource(subgenius + 3);
    bitmap_layer_set_bitmap(subliminal_layer, subliminal);
    layer_set_hidden(bitmap_layer_get_layer(subliminal_layer), false);
    text_layer_set_background_color(text_time_layer, GColorWhite);
    show_sub = 1;
    show_duration = 2;
    
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    
    if ((axis == 1) && (direction == -1)) {
        
        if (toggle_sub == 1) {
            
            
            toggle_sub = 0; } else {
                toggle_sub = 1;
            }
        
        boo();
    }
    
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
    
    // section based on Simplicity by Pebble Team begins
    char *time_format;
    if (clock_is_24h_style()) {
        time_format = "%R";
    } else {
        time_format = "%I:%M";
    }
    
    strftime(time_text, sizeof(time_text), time_format, tick_time);
    
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
    text_layer_set_text(text_time_layer, time_text);
    
    if (!clock_is_24h_style()) {
        strftime(AMPM, sizeof(AMPM), "%p", tick_time);
        text_layer_set_text(AMPMLayer, AMPM);
    }
    
    
    
    if (show_sub) {
        show_duration = show_duration - 1;
        if (show_duration == 0) {
            layer_set_hidden(bitmap_layer_get_layer(subliminal_layer), true);
            text_layer_set_background_color(text_time_layer, GColorClear);
            show_sub = 0;
        }
    }
    
    if ((!show_sub) && (toggle_sub == 1)) {
        if ((rand() % 100) <= sub_chance) {
            boo();
        }
        
    }
}



void handle_init(void) {
    
    window = window_create();
    window_stack_push(window, true /* Animated */);
    Layer *window_layer = window_get_root_layer(window);
    window_set_background_color(window, GColorWhite);
    bob = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BOB);
    background_layer = bitmap_layer_create(layer_get_frame(window_layer));
    bitmap_layer_set_bitmap(background_layer, bob);
    layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
  
#ifdef PBL_ROUND
    text_time_layer = text_layer_create(GRect(18, 144, 144, 34));
#else
#ifdef PBL_PLATFORM_EMERY
    text_time_layer = text_layer_create(GRect(0, 196, 200, 34));
#else
    text_time_layer = text_layer_create(GRect(0, 138, 144, 30));
#endif
#endif
    text_layer_set_text_color(text_time_layer, GColorBlack);
    text_layer_set_background_color(text_time_layer, GColorClear);
    text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
    text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
    
    
    if (!clock_is_24h_style()) {
#ifdef PBL_ROUND
        AMPMLayer = text_layer_create(GRect(122, 144, 30, 18));
#else
#ifdef PBL_PLATFORM_EMERY
        AMPMLayer = text_layer_create(GRect(140, 196, 30, 18));
#else
        AMPMLayer = text_layer_create(GRect(104, 138, 30, 18));
#endif
#endif
        text_layer_set_text_color(AMPMLayer, GColorBlack);
        text_layer_set_background_color(AMPMLayer, GColorClear);
        text_layer_set_text_alignment(AMPMLayer, GTextAlignmentCenter);
        text_layer_set_font(AMPMLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        layer_add_child(window_layer, text_layer_get_layer(AMPMLayer));
    }
  
    subliminal = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUB0);
#ifdef PBL_ROUND
    subliminal_layer = bitmap_layer_create(GRect(0, 0, 180, 150));
#else
#ifdef PBL_PLATFORM_EMERY
    subliminal_layer = bitmap_layer_create(GRect(0, 0, 200, 200));
#else
    subliminal_layer = bitmap_layer_create(GRect(0, 0, 144, 144));
#endif
#endif
    bitmap_layer_set_bitmap(subliminal_layer, subliminal);
    layer_add_child(window_layer, bitmap_layer_get_layer(subliminal_layer));
    layer_set_hidden(bitmap_layer_get_layer(subliminal_layer), true);
    
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_second_tick(current_time, SECOND_UNIT);
    
    
    srand(time(NULL));
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
    accel_tap_service_subscribe(accel_tap_handler);
    
}



void handle_deinit(void) {
    
    tick_timer_service_unsubscribe();
    accel_tap_service_unsubscribe();
    text_layer_destroy(text_time_layer);
    text_layer_destroy(AMPMLayer);
    bitmap_layer_destroy(background_layer);
    window_destroy(window);
    
}


int main(void) {
    
    handle_init();
    app_event_loop();
    handle_deinit();
    
}
