#include <pebble.h>
extern const char *dict[];

static Window *window;
static TextLayer *text_layer;
static TextLayer *desc_layer;
static GBitmap *bitmap;
static BitmapLayer *bitmap_layer;

static int word = 0;

static void show_word() {
  const char *str = dict[word];
  const char *dsc = dict[word+1];
  text_layer_set_text(text_layer, str);
  text_layer_set_text(desc_layer, dsc);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  char letter = dict[word][0];
  // go to next letter
  for (;;) {
    if (dict[word+2]) {
      word += 2;
    } else {
      word = 0;
      break;
    }
    if (dict[word][0] != letter) {
      break;
    }
  }
  show_word ();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // go to prev word
  if (word > 0) {
    word -= 2;
  }
  show_word ();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // go to next word
  if (dict[word+2]) {
    word += 2;
  }
  show_word ();
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 100, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

#if 0
  bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TOKIPONA);
  bitmap_layer = bitmap_layer_create(GRect(5, 5, 48, 48));
  bitmap_layer_set_bitmap(bitmap_layer, bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));
  layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), 0);
#endif

  text_layer = text_layer_create((GRect) { .origin = { 0, 12 }, .size = { bounds.size.w, 40 } });
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text(text_layer, "");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  desc_layer = text_layer_create((GRect) { .origin = { 5, 72 }, .size = { bounds.size.w-5, 60 } });
  text_layer_set_text(desc_layer, "");
  text_layer_set_overflow_mode(desc_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(desc_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(desc_layer));

  show_word ();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  gbitmap_destroy(bitmap);
  bitmap_layer_destroy(bitmap_layer);
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
