#include "main.h"
#include "input_reading.h"
#include "led_display.h"
#include "timer.h"

enum SystemMode { MODE_1 = 1, MODE_2, MODE_3, MODE_4 };
enum SystemMode current_mode = MODE_1;

// Khởi tạo mặc định thỏa mãn: Red (3) = Green (1) + Yellow (1) + 1
int red_time = 6;
int yellow_time = 2;
int green_time = 3;
int temp_value = 0;

enum TrafficState { AUTO_RED_GREEN, AUTO_RED_YELLOW, AUTO_GREEN_RED, AUTO_YELLOW_RED };
enum TrafficState traffic_state = AUTO_RED_GREEN;

int counter_road1 = 0;
int counter_road2 = 0;
static int blink_state = 0;

// Hàm cập nhật đèn giao thông Mode 1 (Active-Low: RESET = Sáng, SET = Tắt)
void updateTrafficLights(void) {
    clearAllLeds();
    switch (traffic_state) {
        case AUTO_RED_GREEN:
            HAL_GPIO_WritePin(GPIOA, red_1_Pin | green_2_Pin, GPIO_PIN_RESET);
            break;
        case AUTO_RED_YELLOW:
            HAL_GPIO_WritePin(GPIOA, red_1_Pin | yellow_2_Pin, GPIO_PIN_RESET);
            break;
        case AUTO_GREEN_RED:
            HAL_GPIO_WritePin(GPIOA, green_1_Pin | red_2_Pin, GPIO_PIN_RESET);
            break;
        case AUTO_YELLOW_RED:
            HAL_GPIO_WritePin(GPIOA, yellow_1_Pin | red_2_Pin, GPIO_PIN_RESET);
            break;
    }
}

void initTrafficSystem(void) {
    traffic_state = AUTO_RED_GREEN;
    counter_road1 = red_time;
    counter_road2 = green_time;
    updateTrafficLights();
}

void fsm_for_input_processing(void) {
    // 1. Heartbeat LED nhấp nháy mỗi 1s (Timer index 9)
    if (isTimerExpired(9)) {
        setTimer(1000, 9);
        blink_state = !blink_state;
        HAL_GPIO_WritePin(GPIOA, blinking_led_Pin, blink_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }

    // 2. Quét LED 7 đoạn mỗi 100ms (Timer index 0)
    if (isTimerExpired(0)) {
        setTimer(100, 0);
        if (current_mode == MODE_1) {
            scan7SEG(counter_road1, counter_road2);
        } else {
            scan7SEG(temp_value, current_mode);
        }
    }

    // 3. Xử lý nút bấm chuyển Mode (Nút 1 - PB0)
    if (is_button_pressed(0)) {
        current_mode++;
        if (current_mode > MODE_4) current_mode = MODE_1;
        clearAllLeds();

        switch (current_mode) {
            case MODE_1:
                traffic_state = AUTO_RED_GREEN;
                counter_road1 = red_time;
                counter_road2 = green_time;
                updateTrafficLights();
                setTimer(1000, 1);
                break;
            case MODE_2:
                temp_value = red_time;
                setTimer(250, 1); // Nhấp nháy 2Hz
                break;
            case MODE_3:
                temp_value = yellow_time;
                setTimer(250, 1);
                break;
            case MODE_4:
                temp_value = green_time;
                setTimer(250, 1);
                break;
        }
    }

    // 4. Logic FSM cho từng chế độ
    switch (current_mode) {
        case MODE_1: // Chế độ chạy tự động bình thường
            if (isTimerExpired(1)) {
                setTimer(1000, 1);
                counter_road1--;
                counter_road2--;

                if (counter_road1 < 0 || counter_road2 < 0) {
                    switch (traffic_state) {
                        case AUTO_RED_GREEN:
                            traffic_state = AUTO_RED_YELLOW;
                            counter_road2 = yellow_time;
                            updateTrafficLights();
                            break;
                        case AUTO_RED_YELLOW:
                            traffic_state = AUTO_GREEN_RED;
                            counter_road1 = green_time;
                            counter_road2 = red_time;
                            updateTrafficLights();
                            break;
                        case AUTO_GREEN_RED:
                            traffic_state = AUTO_YELLOW_RED;
                            counter_road1 = yellow_time;
                            updateTrafficLights();
                            break;
                        case AUTO_YELLOW_RED:
                            traffic_state = AUTO_RED_GREEN;
                            counter_road1 = red_time;
                            counter_road2 = green_time;
                            updateTrafficLights();
                            break;
                    }
                }
            }
            break;

        case MODE_2: // Mode 2: Chỉnh Đỏ (Red) từ 3 đến 99
            if (isTimerExpired(1)) {
                setTimer(250, 1);
                blink_state = !blink_state;
                HAL_GPIO_WritePin(GPIOA, red_1_Pin | red_2_Pin, blink_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            if (is_button_pressed(1)) { // Nút 2: Tăng giá trị (Từ 3 đến 99)
                temp_value++;
                if (temp_value > 99) temp_value = 3;
            }
            if (is_button_pressed(2)) { // Nút 3: Lưu và tự động đồng bộ theo chuẩn Red = Green + Yellow + 1
                red_time = temp_value;
                // Nếu tổng hiện tại không khớp công thức, tự động cân đối lại Xanh và Vàng
                if (green_time + yellow_time + 1 != red_time) {
                    green_time = red_time - yellow_time - 1;
                    if (green_time < 1) {
                        green_time = 1;
                        yellow_time = red_time - green_time - 1; // Đảm bảo tối thiểu mỗi cái là 1
                    }
                }
            }
            break;

        case MODE_3: // Mode 3: Chỉnh Vàng (Yellow)
            if (isTimerExpired(1)) {
                setTimer(250, 1);
                blink_state = !blink_state;
                HAL_GPIO_WritePin(GPIOA, yellow_1_Pin | yellow_2_Pin, blink_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            if (is_button_pressed(1)) { // Nút 2: Tăng giá trị Vàng (Vàng tối đa phải nhỏ hơn Đỏ - 1 để Xanh ít nhất là 1)
                temp_value++;
                if (temp_value >= red_time - 1) temp_value = 1;
            }
            if (is_button_pressed(2)) { // Nút 3: Lưu và cập nhật lại Xanh = Đỏ - Vàng - 1
                yellow_time = temp_value;
                green_time = red_time - yellow_time - 1;
            }
            break;

        case MODE_4: // Mode 4: Chỉnh Xanh (Green)
            if (isTimerExpired(1)) {
                setTimer(250, 1);
                blink_state = !blink_state;
                HAL_GPIO_WritePin(GPIOA, green_1_Pin | green_2_Pin, blink_state ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
            if (is_button_pressed(1)) { // Nút 2: Tăng giá trị Xanh (Xanh tối đa phải nhỏ hơn Đỏ - 1 để Vàng ít nhất là 1)
                temp_value++;
                if (temp_value >= red_time - 1) temp_value = 1;
            }
            if (is_button_pressed(2)) { // Nút 3: Lưu và cập nhật lại Vàng = Đỏ - Xanh - 1
                green_time = temp_value;
                yellow_time = red_time - green_time - 1;
            }
            break;
    }
}
