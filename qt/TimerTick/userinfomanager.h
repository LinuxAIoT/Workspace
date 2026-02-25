#ifndef USERINFOMANAGER_H
#define USERINFOMANAGER_H

#include <mutex>

#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

#include <QSysInfo>
#include <QStorageInfo>
#include <QNetworkInterface>

#define OS_INIT_FLAG                        "os_init_flag"

#define OS_INFO_NAME                        "os_name"
#define OS_INFO_TYPE                        "os_type"
#define OS_INFO_VERSION                     "os_version"
#define OS_INFO_KERNEL_TYPE                 "os_kernel_type"
#define OS_INFO_KERNEL_VERSION              "os_kernel_version"
#define OS_INFO_PRODUCT_NAME                "os_product_name"
#define OS_INFO_ARCH                        "os_arch"
#define OS_INFO_UNIQUE_ID                   "os_unique_id"
#define OS_INFO_BOOT_UNIQUE_ID              "os_boot_unique_id"
#define OS_INFO_BUILD_API                   "os_build_api"
#define OS_INFO_SETUP_TIME                  "user_setup_time"
#define OS_INFO_VALID_TIME                  "user_valid_time"
//TimerTickWidget
#define UI_BACKGROUD_LINE_WIDTH             "ui_background_line_width"
#define UI_BACKGROUD_PADDING_SCALE          "ui_background_padding_scale"
#define UI_BACKGROUD_RADIUS                 "ui_background_radius"
#define UI_BACKGROUD_COLOR_RED              "ui_background_color_red"
#define UI_BACKGROUD_COLOR_GREEN            "ui_background_color_green"
#define UI_BACKGROUD_COLOR_BLUE             "ui_background_color_blue"
#define UI_BACKGROUD_COLOR_ALPHA            "ui_background_color_alpha"
#define UI_FONT_COLOR_RED                   "ui_font_color_red"
#define UI_FONT_COLOR_GREEN                 "ui_font_color_green"
#define UI_FONT_COLOR_BLUE                  "ui_font_color_blue"
#define UI_FONT_COLOR_ALPHA                 "ui_font_color_alpha"
#define UI_FONT_FAMILY                      "ui_font_family"

//TimerTickManager
#define UI_TIME_TYPE                        "ui_time_type"      //H_M_S/H_M/M_S
#define UI_TIME_FORMAT                      "ui_time_format"    //12/24
#define UI_SIZE_FULL_RATIO                  "ui_size_full_ratio"
#define UI_SIZE_RATIO                       "ui_size_ratio"
#define UI_CUSTOM_THEME_INDEX               "ui_custom_theme_index"
#define UI_CUSTOM_THEME_ENABLE              "ui_custom_theme_enable"
#define UI_STANDBY_TIME                     "ui_standby_time"

//SettingsWidget
#define UI_PRIMARY_DISPLAY_ONLY             "ui_primary_display_only"
#define UI_DISPLAY_SECOND                   "ui_display_second"
#define UI_DATE_12_HOUR_FORMAT              "ui_date_12_format"
#define UI_SHOW_DATE_INFO                   "ui_show_date_info"
#define UI_DISPLAY_ONLY_ON                  "ui_display_only_on"
#define UI_SELF_STARING                     "ui_self_staring"
class UserInfoManager : public QObject
{
    Q_OBJECT

public:
    enum TABLENAME{
        SYSTEM,
        SETTINGS
    };
    explicit UserInfoManager(QObject *parent = nullptr);
    static UserInfoManager* getInterface();
    unsigned int getUILineWidth();
    unsigned int getUIPaddingScale();
    unsigned int getUIBackgroundRadius();
    unsigned int getUIBackgroundColorRed();
    unsigned int getUIBackgroundColorGreen();
    unsigned int getUIBackgroundColorBlue();
    unsigned int getUIBackgroundColorAlpha();
    unsigned int getUIFontColorRed();
    unsigned int getUIFontColorGreen();
    unsigned int getUIFontColorBlue();
    unsigned int getUIFontColorAlpha();

    unsigned int getUICustomThemeIndex();
    unsigned int getUICustomThemeEnable();
    unsigned int getUIStandbyTime();
    unsigned int getUITimeFormat(); //12/24
    unsigned int getUITimeType();   //H_M_S/H_M/M_S
    unsigned int getUISizeRatio();
    unsigned int getUISizeFullRatio();

    QString getOSName();
    QString getOSVersion();
    QString getOSKernelVersion();
    QString getOSProductName();
    QString getOSArch();
    QString getOSUUID();
    QString getSetupTime();
    QString getValidTime();

    bool isPrimaryDisplayOnly();
    bool isDisplaySecond();
    bool isDate12HourFormat();
    bool isCustomThemeEnable();
    bool isShowDateInfo();
    bool isSelfStaring();
    QString getUIFontFamily();
    QString getInfo(const TABLENAME &tableName,const QString &name, bool *ok = nullptr);
    bool deleteInfo(const TABLENAME &tableName,const QString &name);
    bool insertInfo(const TABLENAME &tableName,const QString &name, const QString &value);
    bool updateInfo(const TABLENAME &tableName,const QString &name, const QString &newValue);
signals:
private:
    void getSystemInfo();
    void initUserInfoManager();
    void createTable(QString tableName);
    QString getTableName(const TABLENAME &tableName);
private:
    qint64 mDefaultMaxValidYearTime;
    qint64 mDefaultMaxValidMonTime;
    qint64 mDefaultMaxValidDayTime;
    std::mutex mDBMutex;
    QSqlDatabase mDB;
    static std::mutex mMutex;
    static UserInfoManager *mUserInfoManager;
};

#endif // USERINFOMANAGER_H
