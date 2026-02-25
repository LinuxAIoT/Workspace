#ifndef __ESP_CONFIG_H__
#define __ESP_CONFIG_H__
// Serial configuration
#define SERIAL_BAUD_RATE                    115200  // 串口通信波特率
// WiFi configuration                   
#define WIFI_SSID                           "Xiaomi_5DF1"
#define WIFI_PASSWORD                       "Wt980920..@"

// AS5600磁编码器寄存器地址定义
#define AS5600_I2C_SDA                      15          // I2C SDA引脚（根据实际与MCU之间的引脚配置）
#define AS5600_I2C_SCL                      16          // I2C SCL引脚（根据实际与MCU之间的引脚配置）
#define AS5600_I2C_FREQUENCY                400000      // 400kHz高速模式
#define AS5600_I2C_ADDRESS                  0x36        // AS5600 I2C设备地址
#define AS5600_RAW_ANGLE                    0x0C        // 原始角度数据寄存器(12位)
#define AS5600_ANGLE                        0x0E        // 处理后角度数据寄存器(12位)
#define AS5600_STATUS                       0x0B        // 状态寄存器
#define AS5600_AGC                          0x1A        // 自动增益控制寄存器
#define AS5600_MAGNITUDE                    0x1B        // 磁场强度寄存器
#define AS5600_BURN                         0xFF    //! 熔断寄存器(用于永久编程)

//motor config
#define MOTOR_PWM_CHANNEL_A                 11          // 电机PWM通道A
#define MOTOR_PWM_CHANNEL_B                 12          // 电机PWM通道B
#define MOTOR_PWM_CHANNEL_C                 13          // 电机PWM通道C
#define MOTOR_PWM_ENABLE                    10          // 电机使能引脚       
#define MOTOR_POLE_PAIRS                    7           // 电机极对数，影响电机的电气角度计算和控制精度
#define MOTOR_DRIVE_VOLTAGE_SUPPLY          12
#define MOTOR_DRIVE_VOLTAGE_LIMIT           6           // 限制驱动器可以设置的最大直流电压
#define MOTOR_VOLTAGE_LIMIT                 3           // 限制电机可以设置的最大直流电压，这是对低电阻电机的保护措施       
#define MOTOR_CURRENT_LIMIT                 10          
#define MOTOR_CURRENT_PID_P                 0.5         // PID控制器的比例增益（P参数），决定响应速度
#define MOTOR_CURRENT_PID_I                 0.01        // PID控制器的积分增益（I参数），用于消除稳态误差
#define MOTOR_CURRENT_PID_D                 0.01        // PID控制器的微分增益（D参数），用于抑制超调和振荡
#define MOTOR_CURRENT_PID_I_DEAD_TIME       0.01        // 积分死区时间（秒），防止积分项在小误差时过度累积
#define MOTOR_CURRENT_PID_I_SATURATION      10          // 积分饱和限幅值（安培），避免积分项过大导致系统不稳定
#define MOTOR_CURRENT_PID_D_TIMEOUT         0.01        // 微分超时时间（秒），用于处理噪声或快速变化的信号
#define MOTOR_CURRENT_PID_D_DEAD_TIME       0.01        // 微分死区时间（秒），减少高频噪声对微分项的影响


#endif

