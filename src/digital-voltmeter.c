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

uint8_t ADC_Read(uint8_t ADC_Add);
uint8_t *format_adc(float number, uint8_t *buffer);
void send_serial(uint8_t *data);

uint8_t *serial_ptr = 0;

int8_t main(void) {
    uint8_t adc_value = 0;
    float adc_value_f = 0.0;

    serial_init();

    for (;;) {
        // 读取ADC值
        adc_value = ADC_Read(0x03); // 读取Rb2
        adc_value_f =
            (float)adc_value / 255.0f * 5.0f; // 转换为0-5V范围内的浮点数
        uint8_t buffer[9];
        format_adc(adc_value_f, buffer);
        send_serial(buffer);
        //  显示ADC值
        nixie_display_fpu8(adc_value_f);
    }
}

void serial_interrupt_handler(void) interrupt(4) {
    if (TI)
        TI = 0;
    if (*serial_ptr != '\0') {
        // 缓冲区还有数据
        SBUF = *serial_ptr; // 发送当前字节
        serial_ptr++;       // 指针移到下一个字节
    } else {
        serial_ptr = 0;
    }

    if (RI)
        RI = 0;
}

void send_serial(uint8_t *data_ptr) {
    while (serial_ptr != 0)
        ;
    serial_ptr = data_ptr;
    TI = 1;
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

uint8_t *format_adc(float number, uint8_t *buffer) {
    int32_t int_number =
        (int32_t)(number * 10000000); // 放大100000倍以保留小数部分
    uint8_t index = 0;

    // 整数部分
    uint32_t integer_part = int_number / 10000000;
    buffer[index++] = (integer_part % 10) + '0';

    buffer[index++] = '.'; // 小数点

    // 小数部分
    uint32_t fractional_part = int_number % 10000000;
    uint8_t frac_buf[7];
    for (uint8_t i = 0; i < 7; i++) {
        frac_buf[6 - i] = (fractional_part % 10) + '0';
        fractional_part /= 10;
    }
    for (uint8_t i = 0; i < 7; i++) {
        buffer[index++] = frac_buf[i];
    }
    buffer[index++] = '\n';
    buffer[index] = '\0'; // 字符串结束符
    return buffer;
}
