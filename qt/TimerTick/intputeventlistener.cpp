#include "intputeventlistener.h"

unsigned int IntputEventLisener::mTimeout=0;
std::atomic<unsigned int> IntputEventLisener::mCurrent=0;

IntputEventLisener::IntputEventLisener():
    mThread(nullptr),
    mWorker(nullptr),
    mInputEventLisenerTimer(nullptr),
    mIsPlaying(false),
    mMouseHook(nullptr),
    mKeyboardHook(nullptr)
{
#ifdef _WIN32
    // 安装鼠标钩子
    mMouseHook = SetWindowsHookEx(
        WH_MOUSE_LL,
        &IntputEventLisener::MouseHookCallback,
        GetModuleHandle(nullptr),
        0
        );

    // 安装键盘钩子
    mKeyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        &IntputEventLisener::KeyboardHookCallback,
        GetModuleHandle(nullptr),
        0
        );

    mThread=new QThread(this);
    mWorker=new Worker();
    mWorker->moveToThread(mThread);

    QObject::connect(mThread, &QThread::started, mWorker, &Worker::doWork);
    QObject::connect(mThread, &QThread::finished,mThread, &QThread::deleteLater);

    mInputEventLisenerTimer=new QTimer(this);
    //定时器回调
    connect(mInputEventLisenerTimer, &QTimer::timeout, [this] {
        mCurrent++;
        if (mCurrent > mTimeout && !mIsWorkerRunning) {
            qInfo()<<"start check input state for 30s";
            mCurrent = 0;
            // 清理旧线程
            if (mThread) {
                mThread->quit();
                mThread->wait();
                delete mThread;
                delete mWorker;
            }

            // 创建新线程
            mThread = new QThread(this);
            mWorker = new Worker();
            mWorker->moveToThread(mThread);

            // 连接信号槽
            connect(mThread, &QThread::started, mWorker, &Worker::doWork);
            connect(mWorker, &Worker::workerFinished, this, [this](bool isPlaying) {
                if (!isPlaying) {
                    if(mInputEventLisenerTimer->isActive()){
                        qInfo() << "stop mInputEventLisenerTimer";
                        mInputEventLisenerTimer->stop();//关闭定时器
                    }
                    QMetaObject::invokeMethod(this, &IntputEventLisener::inputTimeoutEvent, Qt::QueuedConnection);
                }else{
                    qInfo() << "User activity detected. Ignore timeout.";
                }
                mThread->quit();
            });
            mThread->start();
        }
    });
    mInputEventLisenerTimer->start(1000);

    connect(this, &IntputEventLisener::inputEvent, [this]{
        if (!mInputEventLisenerTimer->isActive()) {
            qInfo() << "restart mInputEventLisenerTimer";
            mInputEventLisenerTimer->start(1000);
        }
        if(mWorker){
            mWorker->setInterrupte(true);
        }
    });
#else
  qDebug()<<"not surrport "<<__FUNCTION__;
#endif
}

IntputEventLisener::~IntputEventLisener()
{
#ifdef _WIN32
    if (mMouseHook)
        UnhookWindowsHookEx(mMouseHook);    //卸载鼠标钩子
    if (mKeyboardHook)
        UnhookWindowsHookEx(mKeyboardHook); //卸载键盘钩子
#endif
    mInputEventLisenerTimer->stop();
    // 安全停止线程
    if (mThread && mThread->isRunning()) {
        mThread->quit();
        if (!mThread->wait(1000)) {  // 等待最多 1 秒
            mThread->terminate();
            mThread->wait();
        }
    }
}

void IntputEventLisener::setInputEventTimeout(unsigned int timeout)
{
    qInfo()<<"update input listner timeout to "<<timeout<<"s";
    mTimeout=timeout;
}

bool IntputEventLisener::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    return false;//表示未处理事件
}

#ifdef _WIN32
LRESULT IntputEventLisener::MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        mCurrent=0;
        emit IntputEventLisener::getInstance()->inputEvent();
//        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
//        qDebug() << "Mouse Event:"
//                 << "X=" << pMouseStruct->pt.x
//                 << "Y=" << pMouseStruct->pt.y
//                 << "Button=" << wParam;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT IntputEventLisener::KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboardStruct->vkCode;

        // 重置空闲计数器并触发信号
        mCurrent = 0;
        emit IntputEventLisener::getInstance()->inputEvent();

        // 获取当前活动窗口的线程ID以确定键盘布局
        GUITHREADINFO guiInfo = { sizeof(GUITHREADINFO) };
        if (GetGUIThreadInfo(0, &guiInfo)) {
            DWORD threadId = GetWindowThreadProcessId(guiInfo.hwndActive, nullptr);
            HKL keyboardLayout = GetKeyboardLayout(threadId);

            // 获取键盘状态（Shift、Caps Lock等）
            BYTE keyState[256] = {0};
            GetKeyboardState(keyState);

            // 将虚拟键码转换为扫描码
            UINT scanCode = MapVirtualKeyEx(vkCode, MAPVK_VK_TO_VSC, keyboardLayout);

            // 转换为Unicode字符
            wchar_t chars[5] = {0};
            int result = ToUnicodeEx(vkCode, scanCode, keyState, chars, 4, 0, keyboardLayout);

            if (result > 0) {
                // 输出实际按下的字符
                QString keyText = QString::fromWCharArray(chars);
                //qDebug() << "Key Pressed:" << keyText;
            } else {
                // 输出虚拟键码（无法转换为字符的特殊键）
                //qDebug() << "Key Code (No Char): 0x" << Qt::hex << vkCode;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
#endif

//https://www.52pojie.cn/thread-1840351-1-1.html
Worker::Worker():
    mWorkTime(30),//循环检测30s
    mMiniPeakVolumeThreshold(0.0001f),
    mCurrentPlayState(false),
    mInputInterrupt(false)
{

}

bool Worker::getCurrentPlayState()
{
    return mCurrentPlayState;
}

void Worker::setInterrupte(bool state)
{
    mInputInterrupt=state;
}

bool Worker::isPlayingVideoOrAudioByPeak()
{
#ifdef _WIN32
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioMeterInformation* pMeterInfo = nullptr;
    float* peakValues = nullptr; // 提前声明并初始化为nullptr
    bool isPlaying = false;
    // 初始化COM
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        _com_error err(hr);
        qFatal() << "CoInitializeEx failed! Error:"
                 << err.ErrorMessage() << " (0x" << hr << ")";
        return false;
    }
    // 创建设备枚举器
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                          CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)){
        qWarning()<<"CoCreateInstance failed!";
        goto Cleanup;
    }

    // 获取默认音频输出设备
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)){
        qWarning()<<"GetDefaultAudioEndpoint failed!";
        goto Cleanup;
    }

    // 激活音频计量接口
    hr = pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL,
                           NULL, (void**)&pMeterInfo);
    if (FAILED(hr)){
        qWarning()<<"Activate failed!";
        goto Cleanup;
    }

    // 获取声道数量
    UINT channelCount;
    hr = pMeterInfo->GetMeteringChannelCount(&channelCount);
    if (FAILED(hr)){
        qWarning()<<"get channelCount failed!";
        goto Cleanup;
    }

    // 动态分配内存（此时可以确保channelCount有效）
    peakValues = new (std::nothrow) float[channelCount];
    if (!peakValues) {
        qWarning()<<"new peakValues failed!";
        goto Cleanup; // 内存分配失败处理
    }

    // 获取所有声道的峰值
    hr = pMeterInfo->GetChannelsPeakValues(channelCount, peakValues);
    if (SUCCEEDED(hr)) {
        for (UINT i = 0; i < channelCount; ++i) {
            //qDebug()<<"peakValues(id:"<<i<<"):"<<peakValues[i];
            if (peakValues[i] > mMiniPeakVolumeThreshold) {
                isPlaying = true;
                break;
            }
        }
    }else{
        qWarning()<<"GetChannelsPeakValues failed!";
    }

Cleanup:
    // 安全释放内存（delete[] nullptr是安全的）
    delete[] peakValues;

    // 释放COM接口
    if (pMeterInfo) pMeterInfo->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();

    return isPlaying;
#else
    qDebug()<<"not surrport "<<__FUNCTION__;
    return false;
#endif
}

void Worker::setmMiniPeakVolumeThreshold(float value)
{
    auto temp=mMiniPeakVolumeThreshold;
    mMiniPeakVolumeThreshold=value;
    qInfo()<<"setmMiniPeakVolumeThreshold from "<<temp<<" to "<<mMiniPeakVolumeThreshold;
}

void Worker::doWork()
{
    mCurrentPlayState=false;
    for(int i=0;i<mWorkTime;i++){
        if(mCurrentPlayState){
            qInfo()<<"user is playing,ignore this event";
            return;
        }
        if(mInputInterrupt){
            qInfo()<<"user input interrupt";
            mInputInterrupt=false;
            return;
        }
        mCurrentPlayState =isPlayingVideoOrAudioByPeak()||mCurrentPlayState;
        QThread::sleep(1);
    }
    emit workerFinished(mCurrentPlayState);
}
