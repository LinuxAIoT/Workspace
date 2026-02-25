#include "timertickmanager.h"
#include "intputeventlistener.h"
#include "userinfomanager.h"
#include "systeminfowidget.h"
#include <QApplication>
#include <QToolButton>
#include <QVector>
#include <QMessageBox>
#include <QMutexLocker>
#include <iostream>
#include <QDir>
#define OUT_DAY 0x000000ff
#define ERROR   0xffffffff
#define OK      0x00000000


QMutex logMutex;
void QtlogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    // 1. 创建日志目录
    QDir logDir("logs");
    if (!logDir.exists()) logDir.mkpath(".");

    // 2. 生成秒级时间戳文件名
    static QString lastTime;
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString logFilePath = QString("logs/%1.log").arg(currentTime);

    // 3. 时间变化时切换文件
    static QFile logFile;
    if (currentTime != lastTime) {
        if (logFile.isOpen()) logFile.close();
        logFile.setFileName(logFilePath);
        logFile.open(QIODevice::Append | QIODevice::Text);
        lastTime = currentTime;
    }

    QMutexLocker locker(&logMutex);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logEntry;
    switch (type) {
        case QtDebugMsg:
        logEntry = QString("[%1] Debug: %2").arg(timestamp).arg(msg);
        break;
    case QtInfoMsg:
        logEntry = QString("[%1] Info: %2").arg(timestamp).arg(msg);
        break;
    case QtWarningMsg:
        logEntry = QString("[%1] tWarning: %2").arg(timestamp).arg(msg);
        break;
    case QtFatalMsg:
        logEntry = QString("[%1] Fatal: %2").arg(timestamp).arg(msg);
        break;
    }

    // 5. 写入文件
    if (logFile.isOpen()) {
        QTextStream stream(&logFile);
        stream << logEntry << Qt::endl;
    }
}

int loadDefaultConfig(){
   auto userInfoManager= UserInfoManager::getInterface();
   auto flag=userInfoManager->getInfo(UserInfoManager::SYSTEM,OS_INIT_FLAG).toInt();
   if(!flag){
       qDebug()<<"user setting is empty,using default settings";
       //settingsWidget
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INIT_FLAG,QString::number(1));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_PRIMARY_DISPLAY_ONLY,QString::number(false));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_DISPLAY_SECOND,QString::number(true));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_DATE_12_HOUR_FORMAT,QString::number(false));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_SHOW_DATE_INFO,QString::number(true));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_DISPLAY_ONLY_ON,QString::number(0));//主屏
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_SELF_STARING,QString::number(false));
       //timertickManager
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_ENABLE,QString::number(false));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_INDEX,QString::number(false));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_STANDBY_TIME,QString::number(5));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_TIME_TYPE,QString::number(1));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_SIZE_FULL_RATIO,QString::number(3));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_SIZE_RATIO,QString::number(4));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_TIME_FORMAT,QString::number(0));
       //timertickwidget
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_RADIUS,QString::number(32));
       userInfoManager->insertInfo( UserInfoManager::SETTINGS, UI_BACKGROUD_PADDING_SCALE,QString::number(10));
       userInfoManager->insertInfo( UserInfoManager::SETTINGS,UI_FONT_FAMILY,"Arial Black");
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_LINE_WIDTH,QString::number(8));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_RED,QString::number(25));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_GREEN,QString::number(25));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_BLUE,QString::number(25));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_ALPHA,QString::number(200));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_RED,QString::number(197));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_GREEN,QString::number(197));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_BLUE,QString::number(197));
       userInfoManager->insertInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_ALPHA,QString::number(255));

       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_NAME,QSysInfo::machineHostName());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_TYPE,QSysInfo::productType());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_VERSION,QSysInfo::productVersion());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_KERNEL_TYPE,QSysInfo::kernelType());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_KERNEL_VERSION,QSysInfo::kernelVersion());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_PRODUCT_NAME,QSysInfo::prettyProductName());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_ARCH,QSysInfo::currentCpuArchitecture());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_UNIQUE_ID,QSysInfo::machineUniqueId());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_BOOT_UNIQUE_ID,QSysInfo::bootUniqueId());
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_BUILD_API,QSysInfo::buildAbi());

       //用户首次注册时间
       auto date=QDate::currentDate();
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_SETUP_TIME,date.toString("yyyy-MM-dd"));
       qInfo()<<"User Setup date:"<<date.toString("yyyy-MM-dd");
       auto validDate=date.addDays(7);
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INFO_VALID_TIME,validDate.toString("yyyy-MM-dd"));
       qInfo()<<"User valid date on:"<<date.toString("yyyy-MM-dd");
       userInfoManager->insertInfo(UserInfoManager::SYSTEM,OS_INIT_FLAG,QString::number(1));
       return OK;
   }

   //判断有效性
   auto today=QDate::currentDate();
   QDate targetDate=QDate::fromString(userInfoManager->getInfo(UserInfoManager::SYSTEM,OS_INFO_VALID_TIME),"yyyy-MM-dd");
   auto days=today.daysTo(targetDate);
   if(days<0){
       qInfo()<<"out of days:"<<days <<" of "<<targetDate;
       return OUT_DAY;
   }
   return OK;
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(QtlogOutput);
    QApplication a(argc, argv);
    auto ret=loadDefaultConfig();
    bool isAutoRun=false;
    if(argc>1){
        QString args=QString(argv[1]);
        if(args.trimmed() == "--background"){
            qDebug()<<"isAutoRun=true";
            isAutoRun=true;
        }
    }
    QFile qss(":/style/style.qss");
    qss.open(QFile::ReadOnly);
    QString styleSheet=QString::fromLatin1(qss.readAll());
    qApp->setStyleSheet(styleSheet);
    a.installNativeEventFilter(IntputEventLisener::getInstance());

    //main widget
    TimerTickManager* widgetM = new TimerTickManager();
    //Setting
    SettingsWidget* settingWidget=nullptr;
    //all widgets
    QVector<TimerTickManager*> widgets;

    if(ret!=OK){
       SystemInfoWidget *info=new SystemInfoWidget();
       info->show();
    }else{
       //仅副屏显示
       bool externDisplayOnlay=UserInfoManager::getInterface()->getInfo(UserInfoManager::SETTINGS,UI_PRIMARY_DISPLAY_ONLY).toInt();
       const auto screens = QGuiApplication::screens();
       for (QScreen* screen : screens) {
           if(externDisplayOnlay){
               if(screen == QGuiApplication::primaryScreen()){
                   continue;//skip primary screen
               }
           }
           // 为每个屏幕创建窗口
           TimerTickManager* widget = new TimerTickManager();

           // 设置窗口归属的屏幕和全屏显示
           widget->setGeometry(screen->geometry());
           // 关键：指定窗口所在的屏幕
           widget->setScreen(screen);
           // 全屏显示（无边框）
           // widget->showFullScreen();

           //默认不显示
           widget->hide();
           widgets.push_back(widget);
       }

       //Main Widget
        widgetM->showSettingButton(true);
        widgetM->setTrayIconMenu(true);
        if(isAutoRun){
            widgetM->stop();
            widgetM->close();
        }else{
            widgetM->start();
            widgetM->show();
        }
       qInfo()<<"start main widget";
       QObject::connect(widgetM,&TimerTickManager::sendSettingClickedEvent,[&](){
           if(settingWidget==nullptr){
               //Settings Widget
               settingWidget=new SettingsWidget();
               settingWidget->show();
               qInfo()<<"settingWidget show";
               for(auto widget:widgets){
                   QObject::connect(settingWidget,&SettingsWidget::sendBgColorRedChanged,widget,[widget,widgetM](int value){
                       widgetM->setBgColorRed(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendBgColorGreenChanged,widget,[widget,widgetM](int value){
                       widgetM->setBgColorGreen(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendBgColorBlueChanged,widget,[widget,widgetM](int value){
                       widgetM->setBgColorBlue(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendBgColorAlphaChanged,widget,[widget,widgetM](int value){
                       widgetM->setBgColorAlpha(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });


                   QObject::connect(settingWidget,&SettingsWidget::sendFontColorRedChanged,widget,[widget,widgetM](int value){
                       widgetM->setFontColorRed(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendFontColorGreenChanged,widget,[widget,widgetM](int value){
                       widgetM->setFontColorGreen(value);
                       widget->setBgColorRed(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendFontColorBlueChanged,widget,[widget,widgetM](int value){
                       widgetM->setFontColorBlue(value);
                       widget->setFontColorBlue(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendFontColorAlphaChanged,widget,[widget,widgetM](int value){
                       widgetM->setFontColorAlpha(value);
                       widget->setFontColorAlpha(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendPrimaryDisplayOnly,widget,[widget,widgetM](bool value){
                       qDebug()<<"sendPrimaryDisplayOnly:"<<value;
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendBackgroundImageIndexChanged,widget,[widget,widgetM](int value){
                       widgetM->updateWidgetBackgroudImageIndex(value);
                       widget->updateWidgetBackgroudImageIndex(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendShowDateInfo,widget,[widget,widgetM](bool value){
                       widgetM->showDateInfo(value);
                       widget->showDateInfo(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });

                   QObject::connect(settingWidget,&SettingsWidget::sendFontFamilyChanged,widget,[widget,widgetM](QString value){
                       widgetM->setFontFamily(value);
                       widget->setFontFamily(value);
                       widgetM->updateFrame();
                       widget->updateFrame();
                   });
               }
           }else{
               if(settingWidget->isHidden()){
                   settingWidget->show();
               }
           }
       });

       // 使用lambda表达式连接信号
       QObject::connect(IntputEventLisener::getInstance(),&IntputEventLisener::inputTimeoutEvent,[widgets,widgetM]{
           qInfo()<<"input event listener timout";
           for(auto widget:widgets){
               //全屏显示
               if(widget && widget->isHidden()){
                   widget->start();
                   widget->setWindowFlags(widget->windowFlags()|Qt::WindowStaysOnTopHint);
                   //全屏显示（无边框）
                   widget->showFullScreen();
               }
           }
           if(widgetM->isTimerActived()){
               widgetM->stop();
           }
       });

       QObject::connect(IntputEventLisener::getInstance(), &IntputEventLisener::inputEvent,[widgets,widgetM]{
           for(auto widget:widgets){
               //全屏显示
               if(widget && widget->isVisible()){
                   widget->hide();
                   widget->stop();
               }
           }
           if(!widgetM->isTimerActived()){
               widgetM->start();
           }
       });
    }

    return a.exec();
}
