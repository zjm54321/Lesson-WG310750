/**
 * @author zjm54321
 * @brief 串行通信系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <nixie.h>
#include <timer.h>
#include <serial.h>

void send_serial(uint8_t *data_ptr);
void serial_rx_handler(uint8_t received_data);

static volatile uint8_t seconds = 0;
static volatile uint8_t minutes = 0;
static volatile bool running = false;

static volatile uint8_t *serial_ptr = NULL;
static volatile bool send_flag = false;

int32_t main(void) {

    timer0_init_10ms();
    timer1_init_10ms();
    serial1_init_timer2();
    EA = 1; // 全局中断使能

    for (;;) {
        if (send_flag) {
            uint8_t buffer[7];
            sprintf(buffer, "%02d:%02d\n", minutes, seconds);
            send_serial(buffer);
            send_flag = false;
        }
    }
}

void timer0_isr(void) interrupt(1) {
    nixie_display_stopwatch(minutes, seconds);
}

void timer1_isr(void) interrupt(3) {
    static uint8_t count = 0;
    if (!running)
        return;

    if (++count >= 100) {
        count = 0;
        send_flag = true;
        if (++seconds >= 60) {
            seconds = 0;
            if (++minutes >= 60) {
                minutes = 0;
            }
        }
    }
}

void serial_interrupt_handler(void) interrupt(4) {
    if (TI) {
        TI = 0;
        if (*serial_ptr != '\0') {
            // 缓冲区还有数据
            SBUF = *serial_ptr; // 发送当前字节
            serial_ptr++;       // 指针移到下一个字节
        } else {
            serial_ptr = NULL;
        }
    }

    if (RI) {
        RI = 0;
        uint8_t received_data = SBUF;
        serial_rx_handler(received_data);
    }
}

inline void send_serial(uint8_t *data_ptr) {
    while (serial_ptr != NULL)
        ;
    serial_ptr = data_ptr;
    TI = 1;
}

inline void serial_rx_handler(uint8_t received_data) {
    // 处理接收到的数据
    switch (received_data) {
    case '1':
        running = false;
        break;
    case '2':
        running = true;
        break;
    case '3':
        running = false;
        seconds = 0;
        minutes = 0;
        break;
    default:
        break;
    }
}