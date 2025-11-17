/**
 * @author zjm54321
 * @brief 调节电位器RB2，使数码管显示的数值在0至255范围内相应变化。
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <stdbool.h>

#include <i2c.h>
#include <nixie.h>
#include <timer.h>

static volatile uint8_t adc_value = 0;

int32_t main(void) {

    timer0_init_10ms();
    timer1_init_10ms();
    EA = 1; // 全局中断使能

    for (;;)
        ;
}

void timer0_isr(void) interrupt(1) { nixie_display_u8(adc_value); }

void timer1_isr(void) interrupt(3) {
    static uint8_t count = 0;
    if (++count >= 5) {
        count = 0;
        adc_value = adc_pcf8591(0x03); // 读取ADC值（Rb2）
    }
}