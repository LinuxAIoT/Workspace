#ifndef __ESP_DEVICE_H__
#define __ESP_DEVICE_H__
#include <cmath>
#include <memory>               // 添加智能指针支持

#include <Wire.h>               // 添加 Wire(IIC)支持
#include <WiFi.h>               // 添加 WiFi 库支持
#include <Arduino.h>            // 包含 Arduino 核心库
#include <SimpleFOC.h>          // 包含 SimpleFOC 库
#include <HardwareSerial.h>     // 显式包含 HardwareSerial
#include "config.h"
// AS5600磁编码器设备类定义
class AS5600Device {
public:
    AS5600Device(HardwareSerial& serial);
    ~AS5600Device();
    void  init();
    bool  checkDeviceStatus();
    float readAngleValue();
    uint16_t readRawAngleValue();
    uint16_t readAGCValue();

private:
    bool                mIsDeviceDetected;
    TwoWire&            mWire;
    HardwareSerial&     mSerial;
    bool     mCurrentStatus;
    uint16_t mAGCValue;
    uint16_t mRawAngleValue;
    uint16_t mAngleValue;
    uint8_t writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t value);
    byte readRegister(uint8_t deviceAddr, uint8_t regAddr);
    void readMultipleBytes(byte deviceAddr, byte regAddr, byte *buffer, byte length);
};

// ESP32FOC设备类定义
class ESP32FOCDevice {
public:
    ESP32FOCDevice();
    ~ESP32FOCDevice();
    float getManEncoderAngle();
    bool isWifiConnected();
    void run();
    void init();
    void setLEDColor(uint8_t r, uint8_t g, uint8_t b);
    void connectWifi(const char* ssid, const char* password);


private:
    HardwareSerial& mSerial;                            // 串口引用
    WiFiClass&      mWifi;                              // WiFi对象
    std::unique_ptr<AS5600Device>   mAS5600Device;      // AS5600设备智能指针
    std::unique_ptr<BLDCMotor>      mMotor;             // 电机对象
    std::unique_ptr<BLDCDriver3PWM> mDriver;            // 电机驱动对象
    std::unique_ptr<Sensor>         mEncoder;           // 编码器对象,simplefoc库中的Sensor类支持AS5600等多种编码器类型
    std::unique_ptr<Commander>      mCommander;         // Commander对象
};
#endif