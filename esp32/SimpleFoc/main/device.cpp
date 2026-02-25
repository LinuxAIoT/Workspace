#include "device.h"
#include "config.h"

#define DEBUG_ENABLE false //! 调试开关，设置为true启用调试输出，false禁用调试输出
#define ELOGD(...) if(DEBUG_ENABLE) Serial.printf(__VA_ARGS__)
// AS5600Device实现
AS5600Device::AS5600Device(HardwareSerial& serial) : 
    mSerial(serial),
    mWire(Wire),
    mIsDeviceDetected(false){
    ELOGD("AS5600Device constructor called\n");
}

AS5600Device::~AS5600Device() {
    ELOGD("AS5600Device constructor called\n");
}

void AS5600Device::init() {
    mSerial.println("initializing AS5600 device");
    // 初始化I2C通信
    mWire.begin(AS5600_I2C_SDA, AS5600_I2C_SCL, AS5600_I2C_FREQUENCY);
    mWire.beginTransmission(AS5600_I2C_ADDRESS);
    byte error = mWire.endTransmission();
    if (error == 0) {
        mIsDeviceDetected = true;
        mSerial.println("AS5600 device detected successfully");
    } else {
        mSerial.println("AS5600 device detection failed");
    }

    // 读取设备状态以确认设备是否正常工作
    mCurrentStatus = checkDeviceStatus();
    // 获取自动增益控制值
    readAGCValue();
    readRawAngleValue();
    readAngleValue();
}

bool AS5600Device::checkDeviceStatus() {
    mSerial.println("checking AS5600 device status");
    if (mIsDeviceDetected){
        byte status = readRegister(AS5600_I2C_ADDRESS, AS5600_STATUS);  // 读取状态寄存器以检查设备状态
        bool magnetDetected  = (status & 0x20) != 0;    // 位5: MD
        bool magnetToostrong = (status & 0x10) != 0;    // 位4: ML
        bool magnetTooWeak = (status & 0x08) != 0;      // 位3: MH
    
        mSerial.print("STATUS: 0x");
        mSerial.print(status, HEX);
        mSerial.print(" | MD: ");
        mSerial.print(magnetDetected);
        mSerial.print(" | ML: ");
        mSerial.print(magnetToostrong);
        mSerial.print(" | MH: ");
        mSerial.println(magnetTooWeak);
        if(!magnetDetected || !magnetToostrong || magnetTooWeak) {
            mSerial.println("Magnet not detected or too strong or too weak");
            return false;
        }
    }
    return true;
}

uint16_t AS5600Device::readRawAngleValue() {
    mSerial.println("reading AS5600 raw angle data");
    mRawAngleValue = 0xFFFF;
    if (mIsDeviceDetected) {
        byte buffer[2];
        readMultipleBytes(AS5600_I2C_ADDRESS, AS5600_RAW_ANGLE, buffer, 2);
        mRawAngleValue = (buffer[0] << 8) | buffer[1];
    }
    return mRawAngleValue;
}

float AS5600Device::readAngleValue() {
    ELOGD("reading AS5600 processed angle data");
    if (mIsDeviceDetected) {
        uint16_t rawAngle = readRawAngleValue();
        float    angle = rawAngle / pow(2,12) * 360.0; //12 bits resolution, 0-4095对应0-360度
        return angle;
    }
    return 0;  // 临时返回0
}

uint16_t AS5600Device::readAGCValue() {
    ELOGD("reading AS5600 AGC data");
    mAGCValue = readRegister(AS5600_I2C_ADDRESS, AS5600_AGC);
    return mAGCValue;
}

uint8_t AS5600Device::writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t value) {
    ELOGD("writing to AS5600 register at address 0x%02X, register 0x%02X: 0x%02X\n", deviceAddr, regAddr, value);
    mWire.beginTransmission(deviceAddr);
    mWire.write(regAddr);               // 寄存器地址
    mWire.write(value);             // 要写入的数据
    mWire.endTransmission();
    return 0;
}

byte AS5600Device::readRegister(uint8_t deviceAddr, uint8_t regAddr) {
    ELOGD("reading from AS5600 register at address 0x%02X, register 0x%02X\n", deviceAddr, regAddr);
    mWire.beginTransmission(deviceAddr);
    mWire.write(regAddr);
    mWire.endTransmission(false);
    
    mWire.requestFrom(deviceAddr, 1);
    if (mWire.available()) {
        return mWire.read();
    }
    return 0xFF;  // 临时返回0
}

void AS5600Device::readMultipleBytes(byte deviceAddr, byte regAddr, byte *buffer, byte length) {
    mWire.beginTransmission(deviceAddr);
    mWire.write(regAddr);
    mWire.endTransmission(false);
    
    mWire.requestFrom(deviceAddr, length);
    for (byte i = 0; i < length && mWire.available(); i++) {
        buffer[i] = mWire.read();
    }
}

//--------------------------------------------------------------------------------------------
// ESP32FOCDevice实现
ESP32FOCDevice::ESP32FOCDevice() : 
    mSerial(Serial), 
    mWifi(WiFi),
    mAS5600Device(nullptr),
    mMotor(nullptr), 
    mDriver(nullptr){
    // 初始化串口
    mSerial.begin(SERIAL_BAUD_RATE);
    mSerial.println("Serial initialized at baud rate " + String(SERIAL_BAUD_RATE));  
}
ESP32FOCDevice::~ESP32FOCDevice() {
    // Cleanup if necessary
}

void ESP32FOCDevice::init() {
    mSerial.println("Start initialization host device");
    // 关闭LED
    setLEDColor(0, 0, 0);
    // 连接WiFi
    mSerial.printf("Connecting to WiFi %s ...\n", WIFI_SSID);
    connectWifi(WIFI_SSID, WIFI_PASSWORD);
    
    // 初始化成功后点亮绿灯
    setLEDColor(0, 255, 0);

    // 初始化AS5600设备
    if(mAS5600Device) {
        mAS5600Device->init();
    }

    // 初始化AS5600设备，传入同一个Serial引用
    mAS5600Device = std::make_unique<AS5600Device>(mSerial);

    // 配置电机参数
    mMotor = std::make_unique<BLDCMotor>(MOTOR_POLE_PAIRS);

    // 配置电机驱动参数
    mDriver = std::make_unique<BLDCDriver3PWM>(MOTOR_PWM_CHANNEL_A, MOTOR_PWM_CHANNEL_B, MOTOR_PWM_CHANNEL_C, MOTOR_PWM_ENABLE);

    // 配置编码器参数，使用AS5600作为位置传感器
    mEncoder = std::make_unique<MagneticSensorI2C>(AS5600_I2C);

    // 配置Commander参数，使用Serial作为通信接口
    mCommander = std::make_unique<Commander>(mSerial);

    // 启用SimpleFOC调试输出
    SimpleFOCDebug::enable(&mSerial); 

    // 设置电源电压[V]
    mDriver->voltage_power_supply   = MOTOR_DRIVE_VOLTAGE_SUPPLY;
    // 限制驱动器可以设置的最大直流电压，这是对低电阻电机的保护措施，该值在启动时固定
    mDriver->voltage_limit          = MOTOR_DRIVE_VOLTAGE_LIMIT;
    // 初始化驱动器
    if(!mDriver->init()){
        mSerial.println("Driver init failed!");
    } else {
        mSerial.println("Driver initialized successfully");
    }

    // 链接电机和驱动
    mMotor->linkDriver(mDriver.get());
    // 限制电机运动，限制设置到电机的电压，对于高电阻电机来说，电流=电压/电阻，所以尝试保持在1安培以下 [V]
    mMotor->voltage_limit = MOTOR_VOLTAGE_LIMIT;
    // 设置电流限制 [A]
    mMotor->current_limit = MOTOR_CURRENT_LIMIT; 
    // 开环速度控制
    mMotor->controller = MotionControlType::velocity_openloop;
    // 初始化电机硬件
    if(!mMotor->init()){
      mSerial.println("Motor init failed!");
      return;
    }
}
void ESP32FOCDevice::setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    // Set LED color using PWM
    rgbLedWrite(RGB_BUILTIN,r, g, b);
}
void ESP32FOCDevice::connectWifi(const char* ssid, const char* password) {
    // Connect to WiFi
    if (ssid == nullptr || password == nullptr) {
        mSerial.println("WiFi SSID or Password is null!");
        return;
    }
    // Attempt to connect to WiFi
    mWifi.begin(ssid, password);

    while (mWifi.status() != WL_CONNECTED) {
        delay(500);
        mSerial.print(".");
    }
    mSerial.println("");
    mSerial.printf("Connected to WiFi SSID %s successfully!\n", ssid);
}

bool ESP32FOCDevice::isWifiConnected() {
    return mWifi.status() == WL_CONNECTED;
}

float ESP32FOCDevice::getManEncoderAngle() {
    if(mAS5600Device) {
        return mAS5600Device->readAngleValue();
    }
    return 0;
}

void ESP32FOCDevice::run() {
    // 运行主循环
    if(mMotor) {
        mMotor->loopFOC();
        mMotor->move(10); // 设置目标速度为10rad/s
    }
}
