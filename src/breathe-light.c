/**
 * @author zjm54321
 * @brief 编写一个程序,实现呼吸灯的动态显示效果
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>

void Timer0_Init(void);
void Delay_10us(void);

int main(void) {
    uint8_t pwm_duty = 0;
    uint8_t pwm_count = 0;
    uint16_t breath_counter = 0;
    bit direction = 1;

    P2 = ((P2 & 0x1f) | 0x80);
    P0 = 0x00;

    Timer0_Init();

    for (;;) {
        // PWM周期控制
        Delay_10us();
        pwm_count++;

        // 根据占空比控制LED亮度
        P0 = (pwm_count < pwm_duty) ? 0xFF : 0xFE;

        // 呼吸效果控制
        if (++breath_counter >= 200) {
            breath_counter = 0;

            if (direction) {
                if (++pwm_duty >= 250) {
                    direction = 0; // 切换到递减
                }
            } else {
                if (pwm_duty-- == 0) {
                    direction = 1; // 切换到递增
                }
            }
        }
    }
}

void Timer0_Init(void) {
    TMOD = 0x01; // 定时器0工作在模式1(16位)
    TH0 = (65536 - 10) >> 8;
    TL0 = (65536 - 10) & 0xFF;
    TR0 = 1; // 启动定时器
}

void Delay_10us(void) {
    while (!TF0)
        ;    // 等待定时器溢出
    TF0 = 0; // 清除溢出标志
    // 重新加载计数值
    TH0 = (65536 - 10) >> 8;
    TL0 = (65536 - 10) & 0xFF;
}