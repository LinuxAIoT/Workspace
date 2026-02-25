#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QSettings>
#include <QCoreApplication>
#include "userinfomanager.h"
#include "switchbutton.h"

#define EXIT_CODE_REBOOT    0xfefe

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_HSliderBackgroundColorAlpha_valueChanged(int value);
    void on_HSliderBackgroundColorBlue_valueChanged(int value);
    void on_HSliderBackgroundColorRed_valueChanged(int value);
    void on_HSliderBackgroundColorGreen_valueChanged(int value);
    void on_HSliderFontColorRed_valueChanged(int value);
    void on_HSliderFontColorGreen_valueChanged(int value);
    void on_HSliderFontColorBlue_valueChanged(int value);
    void on_HSliderFontColorAlpha_valueChanged(int value);
    void on_buttonBox_accept_accepted();
    void on_buttonBox_accept_rejected();
    void on_spinBoxStandbyTime_valueChanged(int arg1);
    void on_buttonBoxBaseControl_accepted();
    void on_buttonBoxBaseControl_rejected();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_toolButtonIndex1_clicked();
    void on_toolButtonIndex2_clicked();
    void on_toolButtonIndex3_clicked();
    void on_toolButtonIndex10_clicked();
    void on_toolButtonIndex4_clicked();
    void on_toolButtonIndex5_clicked();
    void on_toolButtonIndex6_clicked();
    void on_toolButtonIndex7_clicked();
    void on_toolButtonIndex8_clicked();
    void on_toolButtonIndex9_clicked();

    void on_comboBoxScreen_currentIndexChanged(int index);

signals:
    void sendBgColorRedChanged(int value);
    void sendBgColorGreenChanged(int value);
    void sendBgColorBlueChanged(int value);
    void sendBgColorAlphaChanged(int value);

    void sendFontColorRedChanged(int value);
    void sendFontColorGreenChanged(int value);
    void sendFontColorBlueChanged(int value);
    void sendFontColorAlphaChanged(int value);
    void sendShowSecond(bool value);
    void sendFormatChanged(bool value);
    //need restart widget
    void sendPrimaryDisplayOnly(bool value);

    void sendBackgroundImageIndexChanged(unsigned int index);
    void sendShowDateInfo(bool state);
    void sendFontFamilyChanged(QString family);
private:
    void loadConfig();
    void initWidget();
    void loadBaseInfo();
    void loadThemeInfo();
    void loadSwitchButton();
    void updatePreViewBackgroundColor();
    void updatePreViewFontColor();
    void updateThemeIconState();
    void setupToolButtons();
    void updateLastState();
    void setSelfStaring(bool enable);
    unsigned int mStandyTime;
    bool mPrimaryDisplayOnly;
    bool mTimeFormat12;
    bool mCustomThemeEnable;
    bool mShowSecond;
    bool mShowDateInfo;
    bool mSelfStaring;
    QFont mCurrentUIFont;
    QMap<int,QScreen*>mScreens;
    QLayout* addSwitchControlWidget(QString text,bool checked,std::function<void(bool)> slot);
    QIcon createRoundedIcon(const QIcon& icon, const QSize& size, int radius);
    Ui::SettingsWidget *ui;
    UserInfoManager *mUserInfoManager;
    QColor mPreViewBackgroud;
    QColor mPreViewFontColor;

    void updateWidgetBackgroudImageIndex(unsigned int index);
};

#endif // SETTINGSWIDGET_H
