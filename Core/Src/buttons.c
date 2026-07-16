#include "buttons.h"

void Buttons_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

ButtonEvent_t Buttons_ReadEvent(void)
{
    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_2) == GPIO_PIN_SET) {
        return BUTTON_EVENT_LEFT;
    }

    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_3) == GPIO_PIN_SET) {
        return BUTTON_EVENT_RIGHT;
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET) {
        return BUTTON_EVENT_DOWN;
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) {
        return BUTTON_EVENT_ROTATE;
    }

    return BUTTON_EVENT_NONE;
}

uint8_t Buttons_HasEvent(ButtonEvent_t event)
{
    return event != BUTTON_EVENT_NONE;
}
