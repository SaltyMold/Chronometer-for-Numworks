#include "eadk.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "StopWatch";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define BUTTON_COUNT 3

bool running = false; 
uint64_t start_time = 0;
uint64_t elapsed_time = 0;
int milliseconds = 0; // Declare milliseconds here

typedef struct {
  const char* label;
  eadk_rect_t rect;
} Button;

Button buttons[BUTTON_COUNT] = {
  {"START", {115, 200, 90, 30}},
  {"PAUSE", {10, 200, 90, 30}},
  {"STOP", {220, 200, 90, 30}}
};

void draw_buttons(int selected, bool showStart) {
  if (showStart) {
    eadk_color_t border_color = eadk_color_orange;
    eadk_display_push_rect_uniform((eadk_rect_t){buttons[0].rect.x - 2, buttons[0].rect.y - 2, buttons[0].rect.width + 4, buttons[0].rect.height + 4}, border_color);
    eadk_display_push_rect_uniform(buttons[0].rect, eadk_color_white);
    eadk_display_draw_string(buttons[0].label, (eadk_point_t){buttons[0].rect.x + 25, buttons[0].rect.y + 10}, false, eadk_color_black, eadk_color_white);
  } else {
    for (int i = 1; i < BUTTON_COUNT; i++) {
      eadk_color_t border_color = (i == selected) ? eadk_color_orange : eadk_color_black;
      eadk_display_push_rect_uniform((eadk_rect_t){buttons[i].rect.x - 2, buttons[i].rect.y - 2, buttons[i].rect.width + 4, buttons[i].rect.height + 4}, border_color);
      eadk_display_push_rect_uniform(buttons[i].rect, eadk_color_white);
      eadk_display_draw_string(buttons[i].label, (eadk_point_t){buttons[i].rect.x + 25, buttons[i].rect.y + 10}, false, eadk_color_black, eadk_color_white);
    }
  }
}

void clear_buttons() {
  for (int i = 1; i < BUTTON_COUNT; i++) {
    eadk_display_push_rect_uniform((eadk_rect_t){buttons[i].rect.x - 2, buttons[i].rect.y - 2, buttons[i].rect.width + 4, buttons[i].rect.height + 4}, eadk_color_white);
  }
}

void drawTime() {
    uint64_t current_time = eadk_timing_millis();
    uint64_t total_time = elapsed_time + (running ? (current_time - start_time) : 0);
    int milliseconds = total_time % 1000;
    int seconds = (total_time / 1000) % 60;
    int minutes = (total_time / 60000) % 60;
    int hours = (total_time / 3600000);

    char timeStr[20];
    snprintf(timeStr, 20, "%02d:%02d:%02d:%03d", hours, minutes, seconds, milliseconds);
  
    eadk_display_draw_string(timeStr, (eadk_point_t){118, 90}, false, eadk_color_black, eadk_color_white);
}

int main() {

    eadk_timing_msleep(300);
    drawTime();

    eadk_rect_t fond = {0, 0, 320, 240};
    eadk_display_push_rect_uniform(fond, eadk_color_white);

    eadk_rect_t titre = {0, 0, 320, 18};
    eadk_display_push_rect_uniform(titre, eadk_color_orange);

    eadk_display_draw_string("STOPWATCH", (eadk_point_t){129, 3}, false, eadk_color_white, eadk_color_orange);

    int selected_button = 1; // Start with "PAUSE" selected when "START" is not shown
    bool showStart = true;
    draw_buttons(selected_button, showStart);

    while (true) {
        if (running) {
            drawTime();
        }

        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_home)) {
            break;
        }
        if (!showStart) {
            if (eadk_keyboard_key_down(state, eadk_key_right)) {
                selected_button = (selected_button == 1) ? 2 : 1;
                draw_buttons(selected_button, showStart);
                eadk_timing_msleep(200);
                milliseconds += 200;
            }
            if (eadk_keyboard_key_down(state, eadk_key_left)) {
                selected_button = (selected_button == 2) ? 1 : 2;
                draw_buttons(selected_button, showStart);
                eadk_timing_msleep(200);
                milliseconds += 200;
            }
        }
        if (eadk_keyboard_key_down(state, eadk_key_ok)) {
            // Handle button click
            if (showStart) {
                running = true;
                start_time = eadk_timing_millis();
                showStart = false;
                eadk_display_push_rect_uniform((eadk_rect_t){buttons[0].rect.x - 2, buttons[0].rect.y - 2, buttons[0].rect.width + 4, buttons[0].rect.height + 4}, eadk_color_white);
                draw_buttons(selected_button, showStart);
            } else {
                if (selected_button == 1) {
                    running = !running;
                    if (running) {
                        start_time = eadk_timing_millis();
                    } else {
                        elapsed_time += eadk_timing_millis() - start_time;
                    }
                    eadk_timing_msleep(200);
                    milliseconds += 200;
                } else if (selected_button == 2) {
                    running = false;
                    elapsed_time = 0;
                    eadk_display_push_rect_uniform((eadk_rect_t){0, 190, 320, 50}, eadk_color_white); // Clear the time display area
                    drawTime();
                    showStart = true;
                    clear_buttons(); // Clear PAUSE and STOP buttons
                    draw_buttons(selected_button, showStart);
                    eadk_timing_msleep(200);
                }
            }
            eadk_timing_msleep(200);
        }
    }

    return 0;
}