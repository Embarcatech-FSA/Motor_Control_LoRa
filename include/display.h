#ifndef DISPLAY_H
#define DISPLAY_H

#include "lib/ssd1306/ssd1306.h"
#include "lib/ssd1306/font.h"

void display_init(ssd1306_t *ssd);
void display_startup_screen(ssd1306_t *ssd);
void display_update_gate_status(ssd1306_t *ssd, const char* status_line, const char* angle_line);

#endif // DISPLAY_H