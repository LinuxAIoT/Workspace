#include "timertickmanager.h"
#include "./ui_timertickmanager.h"
#include <QFile>
#include <QGuiApplication>
#include "intputeventlistener.h"
TimerTickManager::TimerTickManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerTickManager),
    mTimeFormat(DISPLAY_TIME_FORMAT::HOURS_24),
    mTimeType(DISPLAY_TIME_TYPE::ALL),
    mHour(nullptr),
    mMin(nullptr),
    mSec(nullptr),
    mUiSizeFullRatio(0),
    mUiSizeRatio(0),
    mStanbyTime(0),
    mCustomThemeIndex(0),
    mCustomThemeEnable(true),
    mShowSettingButton(false),
    mDateInfo(nullptr),
    mEnableTrayMenu(false),
    mIsNotified(false)
{
    ui->setupUi(this);

    //加载配置文件
    loadConfig();

    //初始化Widgets
    initWidget();

    //初始化Theme
    initWidgetTheme();
}

void TimerTickManager::initWidget()
{
    if(mHour!=nullptr){
        delete mHour;
        mHour=nullptr;
    }
    if(mMin!=nullptr){
        delete mMin;
        mMin=nullptr;
    }
    if(mSec!=nullptr){
        delete mSec;
        mSec=nullptr;
    }
    switch (mTimeType) {
    case ALL://H_M_S
        mHour =new TimerTickWidget(this);
        mMin =new TimerTickWidget(this);
        mSec =new TimerTickWidget(this);
        ui->horizontalLayout->addWidget(mHour);
        ui->horizontalLayout->addWidget(mMin);
        ui->horizontalLayout->addWidget(mSec);
        mHour->setCurrentAPM(true);
        break;
    case HOUR_MIN:
        mHour =new TimerTickWidget(this);
        mMin =new TimerTickWidget(this);
        ui->horizontalLayout->addWidget(mHour);
        ui->horizontalLayout->addWidget(mMin);
        mHour->setCurrentAPM(true);
        break;
    case MIN_SEC:
        mMin =new TimerTickWidget(this);
        mSec =new TimerTickWidget(this);
        ui->horizontalLayout->addWidget(mMin);
        ui->horizontalLayout->addWidget(mSec);
        mMin->setCurrentAPM(true);
        break;
    default:
        break;
    }

    initUiSize();

    mTimer=new QTimer(this);
    connect(mTimer,&QTimer::timeout,[&](){
        if(!isVisible()){//不可见时，停止
            return;
        }

        QTime t = QTime::currentTime();
        auto hour=t.hour();
        if(mUserInfoManager->isDate12HourFormat()){
            if(hour>12){
               hour=hour-12;
            }
        }

        switch (mTimeType) {
        case ALL:
            mHour->updateFrame(hour);
            mMin->updateFrame(t.minute());
            mSec->updateFrame(t.second());
            break;
        case HOUR_MIN:
            mHour->updateFrame(hour);
            mMin->updateFrame(t.minute());
            break;
        case MIN_SEC:
            mMin->updateFrame(t.minute());
            mSec->updateFrame(t.second());
            break;
        default:
            break;
        }
        //更新dateInfo
        if(mCurrentHour!=t.hour()){
            updateDateInfo();
            mCurrentHour=t.hour();
        }
    });
    mSetting=new QToolButton(this);
    mSetting->setFixedSize(QSize(32,32));
    mSetting->setObjectName("settings");
    mSetting->setIconSize(QSize(32,32));
    mSetting->setIcon(QIcon(":/icon/icon/setting.png"));
    mSetting->move(QPoint(width()-mSetting->width()-BOARD_OFFSET,height()-mSetting->height()-BOARD_OFFSET));
    mSetting->hide();
    connect(mSetting,&QToolButton::clicked,[this](){
       emit sendSettingClickedEvent();
    });

    showDateInfo(mUserInfoManager->isShowDateInfo());
}
void TimerTickManager::loadConfig()
{
    //加载数据库
    mUserInfoManager = UserInfoManager::getInterface();
    mCustomThemeIndex=mUserInfoManager->getUICustomThemeIndex();
    mCustomThemeEnable=mUserInfoManager->getUICustomThemeEnable();
    mStanbyTime=mUserInfoManager->getUIStandbyTime();
    IntputEventLisener::getInstance()->setInputEventTimeout(UMINUTE(mStanbyTime));
    auto timeformat=mUserInfoManager->getUITimeFormat();
    switch (timeformat) {
    case DISPLAY_TIME_FORMAT::HOURS_24:
        mTimeFormat=DISPLAY_TIME_FORMAT::HOURS_24;
        break;
    case DISPLAY_TIME_FORMAT::HOURS_12:
        mTimeFormat=DISPLAY_TIME_FORMAT::HOURS_12;
        break;
    default:
        break;
    }

    auto timeType=mUserInfoManager->getUITimeType();
    switch (timeType) {
    case DISPLAY_TIME_TYPE::ALL:
        mTimeType=DISPLAY_TIME_TYPE::ALL;
        break;
    case DISPLAY_TIME_TYPE::HOUR_MIN:
        mTimeType=DISPLAY_TIME_TYPE::HOUR_MIN;
        break;
    case DISPLAY_TIME_TYPE::MIN_SEC:
        mTimeType=DISPLAY_TIME_TYPE::MIN_SEC;
        break;
    default:
        break;
    }
    mUiSizeFullRatio=mUserInfoManager->getUISizeFullRatio();
    mUiSizeRatio=mUserInfoManager->getUISizeRatio();
    setWindowIcon(QIcon(":/icon/icon/APP_64.png"));
}

void TimerTickManager::initWidgetTheme()
{
    if(mCustomThemeEnable){
        if(mCustomThemeIndex){
            QString style=R"(
            QWidget#horizontalWidget
            {
                border-image: url(%1);
            }
        )";
            QString styleSheet=style.arg(":/wallpaper/wallpaper/%0.jpg").arg(mCustomThemeIndex);
            TimerTickWidget::isCustomTheme(true);
            this->setStyleSheet(styleSheet);
        }else{
            QColor BackgroudColor=QColor(Qt::black);
            setStyleSheet(QString("background-color: %1;").arg(BackgroudColor.name()));
        }
    }else{
        QColor BackgroudColor=QColor(Qt::black);
        setStyleSheet(QString("background-color: %1;").arg(BackgroudColor.name()));
    }
}

void TimerTickManager::initUiSize()
{
    auto w=width()/10;
    auto h=width()/10;
    switch (mTimeType) {
    case ALL:
        w=w*mUiSizeFullRatio;
        h=h*mUiSizeFullRatio;
        mHour->setMaximumSize(w,h);
        mSec->setMaximumSize(w,h);
        break;
    case HOUR_MIN:
        w=w*mUiSizeRatio;
        h=h*mUiSizeRatio;
        mHour->setMaximumSize(w,h);
        break;
    case MIN_SEC:
        w=w*mUiSizeRatio;
        h=h*mUiSizeRatio;
        mSec->setMaximumSize(w,h);
        break;
    default:
        break;
    }
    mMin->setMaximumSize(w,h);
}

void TimerTickManager::onIconTriggered(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger: //单击显示
        start();
        raise();
        showNormal();
        break;
    case QSystemTrayIcon::DoubleClick: //双击退出
        QCoreApplication::quit();
        qInfo()<<"close all windows";
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}

QString TimerTickManager::dayInWeekString(int day)
{
    const QStringList weekDays = {
        "Invalid",  // 占位符（索引0）
        "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"
    };
    return (day >= 1 && day <= 7) ? weekDays[day] : "Invalid";
}

QString TimerTickManager::dayInMonthString(int month)
{
    const QStringList monthAbbr = {
        "Invalid",  // 占位符（索引0）
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    };
    return (month >= 1 && month <= 12) ? monthAbbr[month] : "Invalid";
}

void TimerTickManager::updateWidgetBackgroudImageIndex(unsigned int index)
{
    auto enable=UserInfoManager::getInterface()->getUICustomThemeEnable();
    if(enable){
        QString style=R"(
            QWidget#horizontalWidget
            {
                border-image: url(%1);
            }
        )";
        QString styleSheet=style.arg(":/wallpaper/wallpaper/%0.jpg").arg(index);
        this->setStyleSheet(styleSheet);
    }
    if(index==0||!enable){
        QColor BackgroudColor=QColor(Qt::black);
        setStyleSheet(QString("background-color: %1;").arg(BackgroudColor.name()));
        TimerTickWidget::setLineColor(BackgroudColor);
    }
}

void TimerTickManager::showDateInfo(bool value)
{
    if(value){
        if(mDateInfo==nullptr){
            mDateInfo=new QLabel(this);
            mDateInfo->setObjectName("dateInfo");
            QFont font;
            font.setPixelSize(this->height()/25);
            mDateInfo->resize(this->width(),this->height()/10);
            mDateInfo->setFont(font);
            //获取时间
            QDateTime now = QDateTime::currentDateTime();
            mDateInfo->setText(QString("%1,%2 %3,%4")
                                   .arg(dayInWeekString(now.date().dayOfWeek()))
                                   .arg(dayInMonthString(now.toString("MM").toInt()))
                                   .arg(now.date().day())
                                   .arg(now.toString("yyyy")));
            mDateInfo->setAlignment(Qt::AlignHCenter); //水平居中
            mDateInfo->show();
        }else{
            mDateInfo->show();
        }
    }else{
        if(mDateInfo!=nullptr){
            mDateInfo->hide();
        }
    }
}

void TimerTickManager::updateDateInfo()
{
    if(mDateInfo){
        if(mDateInfo->isVisible()){
            QDateTime now = QDateTime::currentDateTime();
            mDateInfo->setText(QString("%1,%2 %3,%4")
                                   .arg(dayInWeekString(now.date().dayOfWeek()))
                                   .arg(dayInMonthString(now.toString("MM").toInt()))
                                   .arg(now.date().day())
                                   .arg(now.toString("yyyy")));
        }
    }
}

void TimerTickManager::updateFrame()
{
    auto t=QTime::currentTime();
    if(mHour){
        mHour->updateFrameStyle();
        mHour->update();
    }
    if(mMin){
        mMin->updateFrameStyle();
        mMin->update();
    }

    if(mSec){
        mSec->updateFrameStyle();
        mSec->update();
    }
}

TimerTickManager::~TimerTickManager()
{
    delete ui;
    if(mHour){
        delete mHour;
    }
    if(mMin){
        delete mMin;
    }
    if(mSec){
        delete mSec;
    }

    if(mTimer){
        mTimer->stop();
        delete mTimer;
    }

    if(mUserInfoManager){
        delete mUserInfoManager;
    }
}

void TimerTickManager::setBgColorRed(unsigned int value)
{
    qDebug()<<"setBgColorRed:"<<value;
    if(mHour){
        mHour->setBgColorRed(value);
    }
    if(mMin){
        mMin->setBgColorRed(value);
    }
    if(mSec){
        mSec->setBgColorRed(value);
    }
}

void TimerTickManager::setBgColorGreen(unsigned int value)
{
    qDebug()<<"setBgColorGreen:"<<value;
    if(mHour){
        mHour->setBgColorGreen(value);
    }
    if(mMin){
        mMin->setBgColorGreen(value);
    }
    if(mSec){
        mSec->setBgColorGreen(value);
    }
}

void TimerTickManager::setBgColorBlue(unsigned int value)
{
    qDebug()<<"setBgColorBlue:"<<value;
    if(mHour){
        mHour->setBgColorBlue(value);
    }
    if(mMin){
        mMin->setBgColorBlue(value);
    }
    if(mSec){
        mSec->setBgColorBlue(value);
    }
}

void TimerTickManager::setBgColorAlpha(unsigned int value)
{
    qDebug()<<"setBgColorAlpha:"<<value;
    if(mHour){
        mHour->setBgColorAlpha(value);
    }
    if(mMin){
        mMin->setBgColorAlpha(value);
    }
    if(mSec){
        mSec->setBgColorAlpha(value);
    }
}


void TimerTickManager::setFontColorRed(unsigned int value)
{
    qDebug()<<"setFontColorRed:"<<value;
    if(mHour){
        mHour->setFontColorRed(value);
    }
    if(mMin){
        mMin->setFontColorRed(value);
    }
    if(mSec){
        mSec->setFontColorRed(value);
    }
}

void TimerTickManager::setFontColorGreen(unsigned int value)
{
    qDebug()<<"setFontColorGreen:"<<value;
    if(mHour){
        mHour->setFontColorGreen(value);
    }
    if(mMin){
        mMin->setFontColorGreen(value);
    }
    if(mSec){
        mSec->setFontColorGreen(value);
    }
}

void TimerTickManager::setFontColorBlue(unsigned int value)
{
    qDebug()<<"setFontColorBlue:"<<value;
    if(mHour){
        mHour->setFontColorBlue(value);
    }
    if(mMin){
        mMin->setFontColorBlue(value);
    }
    if(mSec){
        mSec->setFontColorBlue(value);
    }
}

void TimerTickManager::setFontColorAlpha(unsigned int value)
{
    qDebug()<<"setFontColorAlpha:"<<value;
    if(mHour){
        mHour->setFontColorAlpha(value);
    }
    if(mMin){
        mMin->setFontColorAlpha(value);
    }
    if(mSec){
        mSec->setFontColorAlpha(value);
    }
}

void TimerTickManager::setFontFamily(QString family)
{
    qDebug()<<"setFontFamily:"<<family;
    if(mHour){
        mHour->setFontFamily(family);
    }
    if(mMin){
        mMin->setFontFamily(family);
    }
    if(mSec){
        mSec->setFontFamily(family);
    }
}

void TimerTickManager::setTrayIconMenu(bool enable)
{
    qDebug()<<"setTrayIconMenu:"<<enable;
    mEnableTrayMenu=enable;
    //托盘
    restoreAction = new QAction(QIcon(":/icon/icon/windows.png"),tr("show"), this);
    connect(restoreAction, &QAction::triggered, this, &TimerTickManager::showNormal);

    quitAction = new QAction(QIcon(":/icon/icon/close.png"),tr("quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu = new QMenu(this);
    trayIconMenu->setObjectName("trayIconMenu");
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icon/icon/APP_64.png"));
    trayIcon->show();

    //点击托盘图标信号：
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TimerTickManager::onIconTriggered);
}

void TimerTickManager::start()
{
    qDebug()<<"TimerTickManager::start";
    if(mTimer){
        if(!mTimer->isActive()){
            mTimer->start(250);
        }
    }
}

void TimerTickManager::stop()
{
    qDebug()<<"TimerTickManager::stop";
    if(mTimer){
        if(mTimer->isActive()){
            mTimer->stop();
        }
    }
}

bool TimerTickManager::isTimerActived()
{
    return mTimer->isActive();
}

void TimerTickManager::showSettingButton(bool state)
{
    qDebug()<<"showSettingButton:"<<state;
    mShowSettingButton=state;
    if(mShowSettingButton){
        mSetting->show();
    }else{
        mSetting->hide();
    }
}

void TimerTickManager::showBubbleWindowInfo()
{
    qDebug()<<"showBubbleWindowInfo";
    if(trayIcon->supportsMessages())
    {
        QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::MessageIcon::Information);
        const QString titleInfo="Information";
        const QString bodyStr="The program is displayed in the tray window.";
        //trayIcon->setVisible(true);
        trayIcon->showMessage(tr(titleInfo.toStdString().c_str()),bodyStr,msgIcon,3000);
        qApp->processEvents();
    }
}

void TimerTickManager::resizeEvent(QResizeEvent *event)
{
    initUiSize();
    if(mSetting)
        mSetting->move(QPoint(width()-mSetting->width()-BOARD_OFFSET,height()-mSetting->height()-BOARD_OFFSET));
    if(mDateInfo){
        QFont font;
        font.setPixelSize(this->height()/25);
        mDateInfo->setFont(font);
        mDateInfo->resize(this->width(),this->height()/10);
    }
}

void TimerTickManager::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
}

void TimerTickManager::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
}

void TimerTickManager::closeEvent(QCloseEvent *event)
{
    qDebug()<<"TimerTickManager::closeEvent";
    if(trayIcon->isVisible())
    {
        event->ignore();
        stop();
        hide();
        if(!mIsNotified){ //没通知过
            mIsNotified=true;
            showBubbleWindowInfo();
        }
    }
}

