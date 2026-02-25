#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "timepicker.h"
SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget),
    mPrimaryDisplayOnly(true),
    mTimeFormat12(true),
    mShowSecond(true),
    mShowDateInfo(false),
    mSelfStaring(false)
{
    ui->setupUi(this);
    resize(915,515);//16:9
    mUserInfoManager=UserInfoManager::getInterface();
    loadConfig();
    initWidget();
    loadThemeInfo();
    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/icon/icon/APP_64.png"));
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::resizeEvent(QResizeEvent *event)
{
}

void SettingsWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void SettingsWidget::loadConfig()
{
    ui->label_red->setFixedWidth(25);
    ui->label_green->setFixedWidth(25);
    ui->label_blue->setFixedWidth(25);
    ui->label_alpha->setFixedWidth(25);

    mStandyTime=mUserInfoManager->getUIStandbyTime();
    ui->spinBoxStandbyTime->setValue(mStandyTime);

    const auto screens = QGuiApplication::screens();
    for(auto &screen:screens){
        if(screen==QGuiApplication::primaryScreen()){
            qInfo()<<"Primary Screen Name:"<<screen->name();
            ui->comboBoxScreen->insertItem(0,screen->name()+"(primary)");
            mScreens.insert(0,screen);
        }
    }

    for(auto &screen:screens){
        int index=1;
        if(screen==QGuiApplication::primaryScreen()){
            continue;
        }
        auto str=screen->name().trimmed();
        int pos = 0;
        while (pos < str.size() && !str[pos].isLetterOrNumber()) {
            pos++;
        }
        str = str.mid(pos);
        if(str.isEmpty()){
            str="UnKnown";
        }
        ui->comboBoxScreen->insertItem(index,str);
        index++;
    }
}

void SettingsWidget::initWidget()
{
    ui->HSliderBackgroundColorRed->setRange(0,255);
    ui->HSliderBackgroundColorGreen->setRange(0,255);
    ui->HSliderBackgroundColorBlue->setRange(0,255);
    ui->HSliderBackgroundColorAlpha->setRange(0,255);

    ui->HSliderFontColorRed->setRange(0,255);
    ui->HSliderFontColorGreen->setRange(0,255);
    ui->HSliderFontColorBlue->setRange(0,255);
    ui->HSliderFontColorAlpha->setRange(0,255);

    auto fontRed=mUserInfoManager->getUIFontColorRed();
    auto fontGreen=mUserInfoManager->getUIFontColorGreen();
    auto fontBlue=mUserInfoManager->getUIFontColorBlue();
    auto fontAlpha=mUserInfoManager->getUIFontColorAlpha();

    if((fontRed==0)&&(fontGreen==0)&&(fontBlue==0)&&(fontAlpha==0)){
        ui->HSliderFontColorRed->setValue(197);
        ui->HSliderFontColorGreen->setValue(197);
        ui->HSliderFontColorBlue->setValue(197);
        ui->HSliderFontColorAlpha->setValue(255);
        mPreViewFontColor=QColor(197,197,197,255);
    }else{
        ui->HSliderFontColorRed->setValue(fontRed);
        ui->HSliderFontColorGreen->setValue(fontGreen);
        ui->HSliderFontColorBlue->setValue(fontBlue);
        ui->HSliderFontColorAlpha->setValue(fontAlpha);
        mPreViewFontColor=QColor(fontRed,fontGreen,fontBlue,fontAlpha);
    }

    auto bgRed=mUserInfoManager->getUIBackgroundColorRed();
    auto bgGreen=mUserInfoManager->getUIBackgroundColorGreen();
    auto bgBlue=mUserInfoManager->getUIBackgroundColorBlue();
    auto bgAlpha=mUserInfoManager->getUIBackgroundColorAlpha();

    if((bgRed==0)&&(bgGreen==0)&&(bgBlue==0)&&(bgAlpha==0)){
        ui->HSliderBackgroundColorRed->setValue(25);
        ui->HSliderBackgroundColorGreen->setValue(25);
        ui->HSliderBackgroundColorBlue->setValue(25);
        ui->HSliderBackgroundColorAlpha->setValue(200);
        mPreViewBackgroud=QColor(25,25,25,200);
    }else{
        ui->HSliderBackgroundColorRed->setValue(bgRed);
        ui->HSliderBackgroundColorGreen->setValue(bgGreen);
        ui->HSliderBackgroundColorBlue->setValue(bgBlue);
        ui->HSliderBackgroundColorAlpha->setValue(bgAlpha);
        mPreViewBackgroud=QColor(bgRed,bgGreen,bgBlue,bgAlpha);
    }

    updatePreViewBackgroundColor();
    updatePreViewFontColor();
    ui->tabWidget->setCurrentIndex(0);//tab_1

    ui->fontComboBox->setMaximumWidth(250);
    ui->fontComboBox->setCurrentText("Arial Black");

    //获取计算机信息
    loadBaseInfo();
    //加载SWButton
    loadSwitchButton();
    //更新最终的UI状态
    updateLastState();
}

void SettingsWidget::loadBaseInfo()
{
    if(mUserInfoManager){
        ui->label_os_name->setText(mUserInfoManager->getOSName());
        ui->label_os_version->setText(mUserInfoManager->getOSProductName());
        ui->label_kernel_version->setText(mUserInfoManager->getOSKernelVersion());
        ui->label_arch->setText(mUserInfoManager->getOSArch());
        ui->label_uuid->setText(mUserInfoManager->getOSUUID());
        ui->label_setup_date->setText(mUserInfoManager->getSetupTime());
        ui->label_expiration_date->setText(mUserInfoManager->getValidTime());
    }
}

void SettingsWidget::loadThemeInfo()
{
    setupToolButtons();
    updateThemeIconState();
}

void SettingsWidget::loadSwitchButton()
{
    if(mUserInfoManager){
        mPrimaryDisplayOnly=mUserInfoManager->isPrimaryDisplayOnly();
        mShowSecond=mUserInfoManager->isDisplaySecond();
        mTimeFormat12=mUserInfoManager->isDate12HourFormat();
        mCustomThemeEnable=mUserInfoManager->isCustomThemeEnable();
        mShowDateInfo=mUserInfoManager->isShowDateInfo();
        mSelfStaring=mUserInfoManager->isSelfStaring();
    }
    // 第一行开关
    auto primarySwitch = addSwitchControlWidget(
        "Display on primary screen only:",
        mPrimaryDisplayOnly,
        [this](bool checked) {
            //插入失败则，更新数据
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_PRIMARY_DISPLAY_ONLY,QString::number(checked));
            mPrimaryDisplayOnly=checked;
            qInfo()<<"Display on primary screen only set to "<<mPrimaryDisplayOnly;
            updateLastState();
            emit sendPrimaryDisplayOnly(checked);
        }
        );
    ui->verticalLayoutSButton->addLayout(primarySwitch);

    auto ShowDateInfo = addSwitchControlWidget(
        "Display date info:",
        mShowDateInfo,  // 初始状态应从数据库读取
        [this](bool checked) {
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_SHOW_DATE_INFO,QString::number(checked));
            mShowDateInfo=checked;
            qInfo()<<"Display date info set to "<<mShowDateInfo;
            emit sendShowDateInfo(checked);
        }
        );
    ui->verticalLayoutSButton->addLayout(ShowDateInfo);

    auto secondSwitch = addSwitchControlWidget(
        "Show second:",
        mShowSecond,  // 初始状态应从数据库读取
        [this](bool checked) {
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_DISPLAY_SECOND,QString::number(checked));
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_TIME_TYPE,QString::number(checked));
            mShowSecond=checked;
            qInfo()<<"Show second set to "<<mShowSecond;
            emit sendShowSecond(checked);
        }  // 修正信号
        );
    ui->verticalLayoutSButton->addLayout(secondSwitch);

    auto Time24FormatSwitch = addSwitchControlWidget(
        "12-hour clock:",
        mTimeFormat12,  // 初始状态应从数据库读取
        [this](bool checked) {
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_DATE_12_HOUR_FORMAT,QString::number(checked));
            mTimeFormat12=checked;
            qInfo()<<"12-hour clock set to "<<mTimeFormat12;
            emit sendFormatChanged(checked);
        }  // 修正信号
    );
    ui->verticalLayoutSButton->addLayout(Time24FormatSwitch);

    auto AutoBoot = addSwitchControlWidget(
        "Self staring:",
        mSelfStaring,  // 初始状态应从数据库读取
        [this](bool checked) {
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_SELF_STARING,QString::number(checked));
            mSelfStaring=checked;
            qInfo()<<"Self staring set to "<<mSelfStaring;
            setSelfStaring(mSelfStaring);
        }  // 修正信号
    );
    ui->verticalLayoutSButton->addLayout(AutoBoot);

    auto ThemeEnable = addSwitchControlWidget(
        "Custom theme:",
        mCustomThemeEnable,  // 初始状态应从数据库读取
        [this](bool checked) {
            mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_ENABLE,QString::number(checked));
            mCustomThemeEnable=checked;
            qInfo()<<"custom theme set to "<<mCustomThemeEnable;
            updateThemeIconState();
            if(!checked){
                updateWidgetBackgroudImageIndex(0);
            }
        }
    );
    ui->customThemeEnable->addLayout(ThemeEnable);
}

void SettingsWidget::updatePreViewFontColor()
{

}

void SettingsWidget::updateThemeIconState()
{
    if(mCustomThemeEnable){
        ui->toolButtonIndex1->setDisabled(false);
        ui->toolButtonIndex2->setDisabled(false);
        ui->toolButtonIndex3->setDisabled(false);
        ui->toolButtonIndex4->setDisabled(false);
        ui->toolButtonIndex5->setDisabled(false);
        ui->toolButtonIndex6->setDisabled(false);
        ui->toolButtonIndex7->setDisabled(false);
        ui->toolButtonIndex8->setDisabled(false);
        ui->toolButtonIndex9->setDisabled(false);
        ui->toolButtonIndex10->setDisabled(false);
    }else{
        ui->toolButtonIndex1->setDisabled(true);
        ui->toolButtonIndex2->setDisabled(true);
        ui->toolButtonIndex3->setDisabled(true);
        ui->toolButtonIndex4->setDisabled(true);
        ui->toolButtonIndex5->setDisabled(true);
        ui->toolButtonIndex6->setDisabled(true);
        ui->toolButtonIndex7->setDisabled(true);
        ui->toolButtonIndex8->setDisabled(true);
        ui->toolButtonIndex9->setDisabled(true);
        ui->toolButtonIndex10->setDisabled(true);
    }
}

void SettingsWidget::setupToolButtons()
{
    // 参数集中管理
    const int buttonCount = 10;    // 按钮总数
    const QSize buttonSize(160,90);

    const int radius = 20;         // 圆角半径

    // 统一设置所有按钮
    for(int i = 1; i <= buttonCount; ++i)
    {
        // 动态获取按钮对象
        QToolButton* btn = findChild<QToolButton*>(QString("toolButtonIndex%1").arg(i));
        if(!btn) continue;

        // 设置按钮尺寸属性
        btn->setFixedSize(buttonSize);
        btn->setIconSize(buttonSize);

        QString imagePath = QString(":/icon/icon/%1.jpg").arg(i);
        btn->setIcon(QIcon(imagePath));
    }
}

void SettingsWidget::updateLastState()
{
    if(mPrimaryDisplayOnly){
        ui->comboBoxScreen->setDisabled(true);
    }else{
        ui->comboBoxScreen->setDisabled(false);
    }
}

void SettingsWidget::setSelfStaring(bool enable)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath(); // 路径格式转换
    QString command =QDir::toNativeSeparators(appPath);
    command+=" --background";
    qDebug()<<command;
    if (enable) {
        settings.setValue(appName, command); // 添加注册表项
    } else {
        settings.remove(appName); // 删除注册表项
    }
}

QLayout* SettingsWidget::addSwitchControlWidget(QString text,bool checked,std::function<void(bool)> slot)
{
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *label = new QLabel(text);
    SwitchButton *switchBtn = new SwitchButton();

    // 连接信号
    connect(switchBtn, &SwitchButton::toggled, slot);
    switchBtn->setChecked(checked);

    // 布局管理
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(switchBtn);

    return layout;
}

QIcon SettingsWidget::createRoundedIcon(const QIcon &icon, const QSize &size, int radius)
{
    QPixmap original = icon.pixmap(size);
    if(original.isNull()) return QIcon();

    QPixmap rounded(size);
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 绘制圆角遮罩
    QPainterPath path;
    path.addRoundedRect(rounded.rect(), radius, radius);
    painter.setClipPath(path);

    // 缩放并绘制原始图片
    painter.drawPixmap(0, 0, original.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    return QIcon(rounded);
}

void SettingsWidget::updateWidgetBackgroudImageIndex(unsigned int index)
{
    qInfo()<<__FUNCTION__<<" "<<__LINE__<<" index:"<<index;
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_INDEX,QString::number(index));
    emit sendBackgroundImageIndexChanged(index);
}

void SettingsWidget::updatePreViewBackgroundColor()
{

}

void SettingsWidget::on_HSliderBackgroundColorAlpha_valueChanged(int value)
{
    emit sendBgColorAlphaChanged(value);
    ui->label_alpha->setText(QString("%1").arg(value));
    mPreViewBackgroud.setAlpha(value);
    updatePreViewBackgroundColor();
    updatePreViewFontColor();
}


void SettingsWidget::on_HSliderBackgroundColorBlue_valueChanged(int value)
{
    emit sendBgColorBlueChanged(value);
    ui->label_blue->setText(QString("%1").arg(value));
    mPreViewBackgroud.setBlue(value);
    updatePreViewBackgroundColor();
    updatePreViewFontColor();
}


void SettingsWidget::on_HSliderBackgroundColorRed_valueChanged(int value)
{
    emit sendBgColorRedChanged(value);
    ui->label_red->setText(QString("%1").arg(value));
    mPreViewBackgroud.setRed(value);
    updatePreViewBackgroundColor();
    updatePreViewFontColor();
}


void SettingsWidget::on_HSliderBackgroundColorGreen_valueChanged(int value)
{
    emit sendBgColorGreenChanged(value);
    ui->label_green->setText(QString("%1").arg(value));
    mPreViewBackgroud.setGreen(value);
    updatePreViewBackgroundColor();
    updatePreViewFontColor();
}


void SettingsWidget::on_HSliderFontColorRed_valueChanged(int value)
{
    emit sendFontColorRedChanged(value);
    mPreViewFontColor.setRed(value);
    updatePreViewFontColor();
    ui->label_font_red->setText(QString("%1").arg(value));
}


void SettingsWidget::on_HSliderFontColorGreen_valueChanged(int value)
{
    emit sendFontColorGreenChanged(value);
    mPreViewFontColor.setGreen(value);
    updatePreViewFontColor();
    ui->label_font_green->setText(QString("%1").arg(value));
}


void SettingsWidget::on_HSliderFontColorBlue_valueChanged(int value)
{
    emit sendFontColorBlueChanged(value);
    mPreViewFontColor.setBlue(value);
    updatePreViewFontColor();
    ui->label_font_blue->setText(QString("%1").arg(value));
}


void SettingsWidget::on_HSliderFontColorAlpha_valueChanged(int value)
{
    emit sendFontColorAlphaChanged(value);
    mPreViewFontColor.setAlpha(value);
    updatePreViewFontColor();
    ui->label_font_alpha->setText(QString("%1").arg(value));
}


void SettingsWidget::on_buttonBox_accept_accepted()
{
    qInfo()<<__FUNCTION__<<" "<<__LINE__;
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_RED,QString::number(ui->HSliderBackgroundColorRed->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_GREEN,QString::number(ui->HSliderBackgroundColorGreen->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_BLUE,QString::number(ui->HSliderBackgroundColorBlue->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_BACKGROUD_COLOR_ALPHA,QString::number(ui->HSliderBackgroundColorAlpha->value()));

    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_RED,QString::number(ui->HSliderFontColorRed->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_GREEN,QString::number(ui->HSliderFontColorGreen->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_BLUE,QString::number(ui->HSliderFontColorBlue->value()));
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_FONT_COLOR_ALPHA,QString::number(ui->HSliderFontColorAlpha->value()));

    ui->HSliderFontColorRed->value();
    ui->HSliderFontColorGreen->value();
    ui->HSliderFontColorBlue->value();
    ui->HSliderFontColorAlpha->value();
}


void SettingsWidget::on_buttonBox_accept_rejected()
{

}


void SettingsWidget::on_spinBoxStandbyTime_valueChanged(int arg1)
{
    mStandyTime=arg1;
}


void SettingsWidget::on_buttonBoxBaseControl_accepted()
{
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_STANDBY_TIME,QString::number(mStandyTime));
}


void SettingsWidget::on_buttonBoxBaseControl_rejected()
{

}


void SettingsWidget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    mCurrentUIFont=f;
    mCurrentUIFont.setPixelSize(72);
    //更新UI
    emit sendFontFamilyChanged(f.family());
    mUserInfoManager->updateInfo(UserInfoManager::SETTINGS,UI_FONT_FAMILY,f.family());
}


void SettingsWidget::on_toolButtonIndex1_clicked()
{
    updateWidgetBackgroudImageIndex(1);
}


void SettingsWidget::on_toolButtonIndex2_clicked()
{
    updateWidgetBackgroudImageIndex(2);
}


void SettingsWidget::on_toolButtonIndex3_clicked()
{
    updateWidgetBackgroudImageIndex(3);
}


void SettingsWidget::on_toolButtonIndex4_clicked()
{
    updateWidgetBackgroudImageIndex(4);
}


void SettingsWidget::on_toolButtonIndex5_clicked()
{
    updateWidgetBackgroudImageIndex(5);
}


void SettingsWidget::on_toolButtonIndex6_clicked()
{
    updateWidgetBackgroudImageIndex(6);
}


void SettingsWidget::on_toolButtonIndex7_clicked()
{
    updateWidgetBackgroudImageIndex(7);
}


void SettingsWidget::on_toolButtonIndex8_clicked()
{
    updateWidgetBackgroudImageIndex(8);
}


void SettingsWidget::on_toolButtonIndex9_clicked()
{
    updateWidgetBackgroudImageIndex(9);
}

void SettingsWidget::on_toolButtonIndex10_clicked()
{
    updateWidgetBackgroudImageIndex(10);
}


void SettingsWidget::on_comboBoxScreen_currentIndexChanged(int index)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<"index:"<<index;
    qDebug()<<ui->comboBoxScreen->itemText(ui->comboBoxScreen->currentIndex());
}

