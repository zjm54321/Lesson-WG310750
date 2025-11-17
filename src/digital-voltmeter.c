/**
 * @author zjm54321
 * @brief 设计数字电压表系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <i2c.h>
#include <nixie.h>
#include <serial.h>
#include <timer.h>

void format_adc(float number, uint8_t *buffer);
void send_serial(uint8_t *data_ptr);

static volatile uint8_t *serial_ptr = NULL;
static volatile uint8_t adc_value = 0;
static uint16_t adc_fpu8 = 0;

int32_t main(void) {
    uint8_t adc_value_pre = 0;

    timer0_init_10ms();
    timer1_init_10ms();
    serial1_init_timer2();
    EA = 1; // 全局中断使能

    for (;;) {
        if (adc_value != adc_value_pre) {
            adc_value_pre = adc_value;
            float adc_value_f =
                (float)adc_value / 255.0f * 5.0f; // 转换为0-5V范围内的浮点数
            adc_fpu8 = (uint16_t)(adc_value_f * 100) % 1000;
            uint8_t buffer[16];
            format_adc(adc_value_f, buffer);
            send_serial(buffer);
        }
    }
}

void timer0_isr(void) interrupt(1) { nixie_display_fpu8(adc_fpu8); }

void timer1_isr(void) interrupt(3) {
    static uint8_t count = 0;
    if (++count >= 5) {
        count = 0;
        adc_value = adc_pcf8591(0x03); // 读取ADC值（Rb2）
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

    if (RI)
        RI = 0;
}

inline void send_serial(uint8_t *data_ptr) {
    while (serial_ptr != NULL)
        ;
    serial_ptr = data_ptr;
    TI = 1;
}

inline void format_adc(float number, uint8_t *buffer) {
    uint8_t integer_part = (uint8_t)number;
    uint8_t decimal_part = (uint8_t)((number - integer_part) * 100);
    //  使用sprintf格式化字符串
    sprintf(buffer, "Voltage: %d.%02dV\n", integer_part, decimal_part);
}