/**
 * @author zjm54321
 * @brief 设计报警器系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <stdbool.h>
#include <softdelay.h>

#include <nixie.h>
#include <timer.h>
#include <exit.h>

#define BUZZER P06

static void timer0_init_500us(void);

static volatile bool running = false;

int32_t main(void) {
    // 初始化蜂鸣器端口
    P2 = (P2 & 0x1F) | 0xA0; // 选择P0口
    BUZZER = 0;

    timer0_init_500us();
    exti_int0_init();
    exti_int1_init();
    EA = 1; // 全局中断使能

    for (;;)
        ;
}

void timer0_isr(void) interrupt(1) {
    static bool buzzer_state = false;
    static uint8_t buzzer_phase = 0;   // 0:1kHz段  1:500Hz段
    static uint16_t phase_counter = 0; // 当前频段的半周期计数
    static uint8_t tick_counter = 0;   // 500us计数器

    if (!running) {
        BUZZER = 0; // 关闭蜂鸣器
        buzzer_state = false;
        buzzer_phase = 0;
        phase_counter = 0;
        tick_counter = 0;
        return;
    }

    // 根据当前频段生成方波
    if (buzzer_phase == 0) {
        // === 1kHz段: 半周期0.5ms (1个tick) ===
        buzzer_state = !buzzer_state;
        BUZZER = buzzer_state ? 0 : 1; // 低电平响

        phase_counter++;
        if (phase_counter >= 200) { // 200个半周期 = 100ms
            phase_counter = 0;
            buzzer_phase = 1; // 切换到500Hz段
        }
    } else {
        // === 500Hz段: 半周期1ms (2个tick) ===
        tick_counter++;
        if (tick_counter >= 2) { // 每2个tick (1ms) 翻转一次
            tick_counter = 0;
            buzzer_state = !buzzer_state;
            BUZZER = buzzer_state ? 0 : 1;

            phase_counter++;
            if (phase_counter >= 200) { // 200个半周期 = 200ms
                phase_counter = 0;
                buzzer_phase = 0; // 切换回1kHz段
            }
        }
    }
}

void exti0_isr(void) interrupt(0) {
    delay5ms();
    if (P32 == 0)
        running = false;
}

void exti1_isr(void) interrupt(2) {
    delay5ms();
    if (P33 == 0)
        running = true;
}

static inline void timer0_init_500us(void) // 500微秒@11.0592MHz
{
    AUXR |= 0x80; // 定时器时钟1T模式
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0x66;   // 设置定时初始值
    TH0 = 0xEA;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
    ET0 = 1;      // 使能定时器0中断
}
