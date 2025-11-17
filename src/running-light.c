/**
 * @author zjm54321
 * @brief 依次点亮LED灯，每个灯点亮后保持0.5秒，然后熄灭并点亮下一个灯。
 */

#include <utility.h>
#include <stc51.h>
#include <softdelay.h>
#include <stdint.h>

int32_t main(void) {
    P2 = ((P2 & 0x1f) | 0x80);
    // 从P00到P07依次点亮LED灯
    for (;;) {
        for (uint8_t i = 0; i <= 7; i++) {
            P0 = ~((1 << i));
            delay500ms();
        }
        // 从P07到P00依次点亮LED灯
        /*
        for (uint8_t i = 7; i >= 0; i--) {
            P0 = ~((1 << i));
            delay500ms();
        }
        */
    }
}
