#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
    BUTTON_EVENT_NONE = 'O',
    BUTTON_EVENT_LEFT = 'L',
    BUTTON_EVENT_RIGHT = 'R',
    BUTTON_EVENT_DOWN = 'D',
    BUTTON_EVENT_ROTATE = 'C'
} ButtonEvent_t;

void Buttons_Init(void);
ButtonEvent_t Buttons_ReadEvent(void);
uint8_t Buttons_HasEvent(ButtonEvent_t event);

#endif /* __BUTTONS_H__ */
