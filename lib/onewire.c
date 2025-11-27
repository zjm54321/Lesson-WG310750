#include <stc51.h>
#include <compiler.h>
#include <stdint.h>
#include <stdbool.h>

// 单总线引脚：P1.4
#define DQ  P14

// 单总线延时：使用T0一次性计时，保持T1配置不变
static void Delay_OneWire(uint16_t t) {
    TR0 = 0;
    TF0 = 0;
    TMOD = (TMOD & 0xF0) | 0x01;           // 仅设置T0为方式1(16位)
    TH0  = (uint8_t)((65536u - t) >> 8);
    TL0  = (uint8_t)((65536u - t) & 0xFF);
    TR0 = 1;
    while (TF0 == 0) {
        // wait
    }
    TR0 = 0;
    TF0 = 0;
}

// DS18B20 初始化（复位与存在脉冲检测）
// 返回: true=设备存在(成功)；false=失败
bool init_ds18b20(void) {
    bool present;

    DQ = 0;
    Delay_OneWire(500);   // 复位脉冲 > 480us
    DQ = 1;
    Delay_OneWire(100);   // 等待从机存在脉冲窗口

    // 存在脉冲为低电平；读到1表示失败，0表示成功
    present = (DQ == 0);
    return present;
}

// 向DS18B20写一个字节
void Write_DS18B20(uint8_t dat) {
    EA = 0;
    for (uint8_t i = 0; i < 8; i++) {
        DQ = 0;
        NOP(); NOP();
        if (dat & 0x01) {
            DQ = 1;
        } else {
            DQ = 0;
        }
        Delay_OneWire(20);   // 写时隙保持
        DQ = 1;
        dat >>= 1;
        Delay_OneWire(2);    // 恢复间隔
    }
    EA = 1;
}

// 从DS18B20读一个字节
uint8_t Read_DS18B20(void) {
    uint8_t dat = 0;

    EA = 0;
    for (uint8_t i = 0; i < 8; i++) {
        DQ = 0;
        NOP(); NOP();
        DQ = 1;
        NOP(); NOP();
        if (DQ) {
            dat |= (uint8_t)(0x01u << i);
        }
        Delay_OneWire(60);   // 读时隙结束
    }
    EA = 1;

    return dat;
}

// 读取温度（浮点数，分辨率0.0625℃）
float rd_temperature_f(void) {
    uint16_t temp;
    float temperature;
    uint8_t low, high;

    // 启动转换
    init_ds18b20();
    Write_DS18B20(0xCC);     // SKIP ROM
    Write_DS18B20(0x44);     // CONVERT T
    Delay_OneWire(50000);    // 转换等待（简化：阻塞等待）

    // 读取温度寄存器
    init_ds18b20();
    Write_DS18B20(0xCC);     // SKIP ROM
    Write_DS18B20(0xBE);     // READ SCRATCHPAD

    low  = Read_DS18B20();   // 低字节
    high = Read_DS18B20();   // 高字节

    // 仅取低12位（0.0625℃/LSB）
    temp  = ((uint16_t)(high & 0x0F) << 8) | low;
    temperature = (float)temp * 0.0625f;

    return temperature;
}

