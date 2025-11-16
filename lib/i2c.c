#include <stc51.h>
#define DELAY_TIME 5

/** 定义I2C总线时钟线和数据线 */
#define scl P20
#define sda P21

/**I2C总线中一些必要的延时*/
void i2c_delay(unsigned char i)
{
    do
    {
        __asm__("nop");
    }
    while(i--);        
}

/** @brief 产生I2C总线启动条件.*/
void i2c_start(void)
{
    sda = 1;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 0;
    i2c_delay(DELAY_TIME);
    scl = 0;    
}

/**@brief 产生I2C总线停止条件*/
void i2c_stop(void)
{
    sda = 0;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 1;
    i2c_delay(DELAY_TIME);       
}

/**I2C发送一个字节的数据*/
void i2c_sendbyte(unsigned char byt)
{
    unsigned char i;
	EA = 0;
    for(i=0; i<8; i++){
        scl = 0;
        i2c_delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
        i2c_delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
        i2c_delay(DELAY_TIME);
    }
	EA = 1;
    scl = 0;  
}

/**等待应答*/
unsigned char i2c_waitack(void)
{
	unsigned char ackbit;
	
    scl = 1;
    i2c_delay(DELAY_TIME);
    ackbit = sda; //while(sda);  //wait ack
    scl = 0;
    i2c_delay(DELAY_TIME);
	
	return ackbit;
}

/**I2C接收一个字节数据*/
unsigned char i2c_receivebyte(void)
{
	unsigned char da;
	unsigned char i;
	EA = 0;	
	for(i=0;i<8;i++){   
		scl = 1;
		i2c_delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		i2c_delay(DELAY_TIME);
	}
	EA = 1;
//
	return da;    
}

/**发送应答*/
void i2c_sendack(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit;  //0：应答；1：非应答
    i2c_delay(DELAY_TIME);
    scl = 1;
    i2c_delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
    i2c_delay(DELAY_TIME);
}



