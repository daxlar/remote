#ifndef DEBOUNCER_H
#define DEBOUNCER_H

// channel 0 corresponds to the joystick button
// channel 1 corresponds to the left button
// channel 2 corresponds to the right button 
// channel 3 corresponds to the down button 
// channel 4 corresponds to the up button 

typedef enum {
  JOYSTICK_BUTTON,
  LEFT_BUTTON,
  RIGHT_BUTTON,
  DOWN_BUTTON,
  UP_BUTTON
}button;

void debouncer_init(void);
void start_debouncing(button button_choice);

#endif