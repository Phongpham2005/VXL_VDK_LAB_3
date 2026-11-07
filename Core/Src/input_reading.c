#include "input_reading.h"

#define DURATION_FOR_AUTO_INCREASING 100 // 100 * 10ms = 1 giây
#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

static uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};

void button_reading(void) {
    GPIO_PinState pinStates[NO_OF_BUTTONS];
    pinStates[0] = HAL_GPIO_ReadPin(btn_1_GPIO_Port, btn_1_Pin);
    pinStates[1] = HAL_GPIO_ReadPin(btn_2_GPIO_Port, btn_2_Pin);
    pinStates[2] = HAL_GPIO_ReadPin(btn_3_GPIO_Port, btn_3_Pin);

    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = pinStates[i];

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            buttonBuffer[i] = debounceButtonBuffer1[i];
            if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    flagForButtonPress1s[i] = 1;
                }
            } else {
                counterForButtonPress1s[i] = 0;
                flagForButtonPress1s[i] = 0;
            }
        }
    }
}

unsigned char is_button_pressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0;
    static uint8_t button_flag[NO_OF_BUTTONS] = {1, 1, 1};

    if (buttonBuffer[index] == BUTTON_IS_PRESSED) {
        if (button_flag[index] == 1) {
            button_flag[index] = 0;
            return 1;
        }
    } else {
        button_flag[index] = 1;
    }
    return 0;
}

unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (flagForButtonPress1s[index] == 1);
}
