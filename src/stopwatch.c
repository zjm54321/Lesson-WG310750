/**
 * @author zjm54321
 * @brief 马表控制系统
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>
#include <stdbool.h>
#include <softdelay.h>

#include <nixie.h>
#include <timer.h>
#include <exit.h>

static volatile uint8_t seconds = 0;
static volatile uint8_t minutes = 0;
static volatile bool running = false;

int32_t main(void) {

    timer0_init_10ms();
    timer1_init_10ms();
    exti_int0_init();
    exti_int1_init();
    EA = 1; // 全局中断使能

    for (;;)
        ;
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
        if (++seconds >= 60) {
            seconds = 0;
            if (++minutes >= 60) {
                minutes = 0;
            }
        }
    }
}

void exti0_isr(void) interrupt(0) {
    delay5ms();
    if (P32 == 0)
        running = !running;
}

void exti1_isr(void) interrupt(2) {
    delay5ms();
    if (P33 == 0) {
        running = false;
        seconds = 0;
        minutes = 0;
    }
}