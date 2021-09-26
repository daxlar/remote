#ifndef JOYSTICK_H
#define JOYSTICK_H

void joystick_init(void);
void joystick_read_x_y_values(int16_t* x_value, int16_t* y_value);
void enable_joystick_button(void);

#endif