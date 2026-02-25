#ifndef INTPUTEVENTLISTENER_H
#define INTPUTEVENTLISTENER_H


#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QAbstractNativeEventFilter>

#include <atomic>

#ifdef _WIN32
#include <comdef.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#endif

class Worker:public QObject{
    Q_OBJECT
public:
    Worker();
    bool  getCurrentPlayState();
    void  setInterrupte(bool state);
private:
    unsigned int mWorkTime;
    float mMiniPeakVolumeThreshold;
    bool  mCurrentPlayState;
    bool  mInputInterrupt;
    bool  isPlayingVideoOrAudioByPeak();
    void  setmMiniPeakVolumeThreshold(float value);
public slots:
    void doWork();
signals:
    void workerFinished(bool isPlaying);
};

class IntputEventLisener : public QObject,public QAbstractNativeEventFilter
{
    Q_OBJECT
private:
    QThread * mThread;
    Worker  * mWorker;
    QTimer  * mInputEventLisenerTimer;
    bool      mIsPlaying;
    bool      mIsWorkerRunning = false;
private:
    static unsigned int mTimeout;
    static std::atomic<unsigned int> mCurrent;
#ifdef _WIN32 //windows only
    HHOOK mMouseHook;
    HHOOK mKeyboardHook;
    static LRESULT CALLBACK MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
#endif
public:
    IntputEventLisener();
    ~IntputEventLisener();
    static IntputEventLisener* getInstance() {
        static IntputEventLisener instance;
        return &instance;
    }
    static void setInputEventTimeout(unsigned int timeout);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
signals:
    void inputTimeoutEvent();
    void inputEvent();
};

#endif // INTPUTEVENTLISTENER_H
