#include "led_display.h"
#include "timer.h"

// Bảng giải mã LED 7 đoạn ANODE CHUNG (0: Sáng, 1: Tắt tương ứng a..g)
const uint8_t segmentNumber[10] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

void display7SEG(int num) {
    if (num < 0 || num > 9) return;
    uint8_t value = segmentNumber[num];

    HAL_GPIO_WritePin(GPIOB, a_Pin, (value & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, b_Pin, (value & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, c_Pin, (value & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, d_Pin, (value & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, e_Pin, (value & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, f_Pin, (value & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, g_Pin, (value & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Quét 4 LED 7 đoạn
void scan7SEG(int num1, int num2) {
    static uint8_t led_index = 0;

    HAL_GPIO_WritePin(GPIOA, en0_Pin | en1_Pin | en2_Pin | en3_Pin, GPIO_PIN_SET);

    switch (led_index) {
        case 0:
            display7SEG(num1 / 10);
            HAL_GPIO_WritePin(GPIOA, en0_Pin, GPIO_PIN_RESET); // Kích hoạt en0
            break;
        case 1:
            display7SEG(num1 % 10);
            HAL_GPIO_WritePin(GPIOA, en1_Pin, GPIO_PIN_RESET); // Kích hoạt en1
            break;
        case 2:
            display7SEG(num2 / 10);
            HAL_GPIO_WritePin(GPIOA, en2_Pin, GPIO_PIN_RESET); // Kích hoạt en2
            break;
        case 3:
            display7SEG(num2 % 10);
            HAL_GPIO_WritePin(GPIOA, en3_Pin, GPIO_PIN_RESET); // Kích hoạt en3
            break;
    }
    led_index = (led_index + 1) % 4;
}

void clearAllLeds(void) {
    HAL_GPIO_WritePin(GPIOA, red_1_Pin | yellow_1_Pin | green_1_Pin |
                             red_2_Pin | yellow_2_Pin | green_2_Pin, GPIO_PIN_SET);
}
