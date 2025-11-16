/**
 * @author zjm54321
 * @brief 调节电位器RB2，使数码管显示的数值在0至255范围内相应变化。
 */

#include <utility.h>
#include <stc51.h>
#include <stdint.h>

#include <i2c.h>
#include <nixie.h>

uint8_t ADC_Read(uint8_t ADC_Add);

int8_t main(void) {
    uint8_t adc_value = 0;

    for (;;) {
        // 读取ADC值
        adc_value = ADC_Read(0x03); // 读取Rb2

        // 显示ADC值
        nixie_display_number(adc_value);
    }
}

/**
 * @brief 读取ADC值
 * @param ADC_Add ADC通道地址 (0-3)
 * @return 8位ADC转换结果 (0-255)
 */
uint8_t ADC_Read(uint8_t ADC_Add) {
    uint8_t ADC_Read_Data = 0;

    // 主设备给从设备发送/写入数据
    i2c_start();
    i2c_sendbyte(0x90); // 写地址
    i2c_waitack();
    i2c_sendbyte(0x40 | ADC_Add); // 控制字节
    i2c_waitack();

    // 主设备给从设备接收/读取数据
    i2c_start();
    i2c_sendbyte(0x91); // 读地址
    i2c_waitack();
    ADC_Read_Data = i2c_receivebyte();
    i2c_sendack(1);
    i2c_stop();

    return ADC_Read_Data;
}
