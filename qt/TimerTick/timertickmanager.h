#ifndef TIMERTICKMANAGER_H
#define TIMERTICKMANAGER_H

#include <QTime>
#include <QMenu>
#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QResizeEvent>
#include <QToolButton>
#include <QSystemTrayIcon>
#include <memory>

#include "settingswidget.h"
#include "timertickwidget.h"
#include "userinfomanager.h"

#define UHOUR(x)            (x*60*60)
#define UMINUTE(x)          (x*60)
#define USECOND(x)          (x)
#define BOARD_OFFSET        24

enum DISPLAY_TIME_FORMAT{
    HOURS_24,
    HOURS_12,
};

enum DISPLAY_TIME_TYPE{
    ALL         =1,
    HOUR_MIN    =0,
    MIN_SEC     =2,
};

QT_BEGIN_NAMESPACE
namespace Ui { class TimerTickManager; }
QT_END_NAMESPACE

class TimerTickManager : public QWidget
{
    Q_OBJECT

public:
    TimerTickManager(QWidget *parent = nullptr);
    ~TimerTickManager();
    void setBgColorRed(unsigned int value);
    void setBgColorGreen(unsigned int value);
    void setBgColorBlue(unsigned int value);
    void setBgColorAlpha(unsigned int value);

    void setFontColorRed(unsigned int value);
    void setFontColorGreen(unsigned int value);
    void setFontColorBlue(unsigned int value);
    void setFontColorAlpha(unsigned int value);
    void setFontFamily(QString family);
    void setTrayIconMenu(bool enable);
    void updateWidgetBackgroudImageIndex(unsigned int index);
    void showDateInfo(bool value);
    void updateDateInfo();
    void updateFrame();
    //开启与关闭定时器
    void start();
    void stop();
    bool isTimerActived();
    void showSettingButton(bool state);
    void showBubbleWindowInfo();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
signals:
    void sendSettingClickedEvent();
private:
    void initWidget();
    void loadConfig();
    void initWidgetTheme();
    void initUiSize();
    void onIconTriggered(QSystemTrayIcon::ActivationReason reason);
    QString dayInWeekString(int day);
    QString dayInMonthString(int month);

private:
    Ui::TimerTickManager *ui;
    DISPLAY_TIME_FORMAT mTimeFormat;        //时间制式,12/24(default)
    DISPLAY_TIME_TYPE   mTimeType;          //显示制式，H_M_S/H_M/M_S

    TimerTickWidget *mHour;                 //时
    TimerTickWidget *mMin;                  //分
    TimerTickWidget *mSec;                  //秒
    unsigned int    mCurrentHour;           //用于降低资源
    unsigned int    mCurrentMin;            //用于降低资源
    unsigned int    mUiSizeFullRatio;       //UI大小系数（ALL）
    unsigned int    mUiSizeRatio;           //UI大小系数（HOUR_MIN/MIN_SEC）
    unsigned int    mStanbyTime;            //默认开启input检查时间
    QTimer *mTimer;                         //定时器(s)
    unsigned int mCustomThemeIndex;         //自定义主题index
    bool mCustomThemeEnable;                //自定义主题使能标记
    UserInfoManager* mUserInfoManager;      //用户数据
    bool mShowSettingButton;                //是否显示设置按键
    QToolButton *mSetting;
    QLabel *mDateInfo;                      //显示dateInfo
    QAction *restoreAction;                 //恢复
    QAction *quitAction;                    //退出
    QSystemTrayIcon *trayIcon;              //托盘
    QMenu *trayIconMenu;                    //托盘菜单
    bool mEnableTrayMenu;                   //使能
    bool mIsNotified;                       //托盘菜单仅通知一次
};
#endif // TIMERTICKMANAGER_H
