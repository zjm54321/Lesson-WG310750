/**
 * @author zjm54321
 * @brief 设计数字电压表系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>

#include <i2c.h>
#include <nixie.h>
#include <serial.h>

void send_serial(uint8_t *data);

uint8_t *serial_ptr = 0;
uint8_t msg[] = "Speed:1,Mode:1\n";
bit flag = 0;

int32_t main(void) {
    uint8_t adc_value = 0;
    float adc_value_f = 0.0;

    serial_init();

    for (;;) {
        while (!flag)
            ;
        send_serial(msg);
        flag = 0;
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
            serial_ptr = 0;
        }
    }

    if (RI) {
        RI = 0;
        uint8_t num = SBUF;
        num = num - '0';
        msg[6] = num / 10 + '0';
        msg[13] = num % 10 + '0';
        flag = 1;
    }
}

void send_serial(uint8_t *data_ptr) {
    while (serial_ptr != 0)
        ;
    serial_ptr = data_ptr;
    TI = 1;
}
