#include "userinfomanager.h"
#include <QApplication>
#include <QScreen>
#include <QFileInfo>
//statci成员变量在类中只是声明那么就需要在类外部重新定义或者初始化
std::mutex UserInfoManager::mMutex;

//statci成员变量在类中只是声明那么就需要在类外部重新定义或者初始化
UserInfoManager * UserInfoManager::mUserInfoManager = nullptr;

UserInfoManager::UserInfoManager(QObject *parent)
    : QObject{parent},
    mDefaultMaxValidYearTime(0),
    mDefaultMaxValidMonTime(0),
    mDefaultMaxValidDayTime(7)
{
    QString db = QDir::currentPath()+"/UserInfo.db";
    if(QFileInfo::exists(db)){
        qDebug()<<"db at "<<db<<"exists";
        initUserInfoManager();
    }else{
        qDebug()<<"db at "<<db<<"not exists";
        initUserInfoManager();
    }
}

void UserInfoManager::initUserInfoManager()
{
    //https://blog.csdn.net/bigxiamu/article/details/145635394
    //需要编译SQLITECIPHER，编译保存需要修改sqlitecipher.cpp
    /**修改为：
     * #if (QT_VERSION >= 0x050000)
        Q_DECLARE_OPAQUE_POINTER(sqlite3*)
        Q_DECLARE_OPAQUE_POINTER(sqlite3_stmt*)
    #endif
        Q_DECLARE_METATYPE(sqlite3*)
        Q_DECLARE_METATYPE(sqlite3_stmt*)
    */
    mDB = QSqlDatabase::addDatabase("QSQLITE");//SQLITECIPHER 加密数据库

    // 数据库文件名称
    mDB.setDatabaseName("UserInfo.db");
    //https://www.cnblogs.com/09w09/p/16540716.html
    mDB.setPassword("admin");
    //mDB.setConnectOptions("QSQLITE_CREATE_KEY");
    // 尝试打开数据库
    if (!mDB.open()) {
        qCritical() << "open Unserinfo db error:" << mDB.lastError().text();
        return;
    }

    //system info
    QString systemInfo =R"(
        CREATE TABLE IF NOT EXISTS system (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        value TEXT NOT NULL)
    )";

    // user settings
    QString userSettings =R"(
        CREATE TABLE IF NOT EXISTS settings (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        value TEXT NOT NULL)
    )";

    createTable(systemInfo);
    createTable(userSettings);
}

void UserInfoManager::getSystemInfo()
{
//#define DEBUG
#ifdef DEBUG
    qDebug() << "OS Type:" << QSysInfo::productType()
             << "OS Version:" <<QSysInfo::productVersion();
    qDebug() << "Hostname:" << QSysInfo::machineHostName();
    qDebug() << "Architecture:" << QSysInfo::currentCpuArchitecture();
    qDebug() << "machineUniqueId:" << QSysInfo::machineUniqueId();
    qDebug() << "bootUniqueId:" << QSysInfo::bootUniqueId();
    qDebug() << "bootUniqueId:" << QSysInfo::buildAbi();
    qDebug() << "kernelType: " << QSysInfo::kernelType();
    qDebug() << "kernelVersion: " << QSysInfo::kernelVersion();
    qDebug() << "prettyProductName: " << QSysInfo::prettyProductName();
#endif

}

void UserInfoManager::createTable(QString tableName)
{
    std::lock_guard<std::mutex> lock(mDBMutex);
    // 创建表（如果不存在）
    QSqlQuery query;
    if (!query.exec(tableName)) {
        qFatal() << "create tableName" << query.lastError().text();
    }
}

bool UserInfoManager::insertInfo(const TABLENAME &tableName,const QString &name, const QString &value)
{
    std::lock_guard<std::mutex> lock(mDBMutex);
    QSqlQuery query;
    QString cmd=QString("INSERT INTO %1 (name, value) VALUES (:name, :value)").arg(getTableName(tableName));
    query.prepare(cmd);
    query.bindValue(":name", name);
    query.bindValue(":value", value);

    if (!query.exec()) {
        qWarning() << "Insert error:" << query.lastError();
        return false;
    }
    return true;
}

bool UserInfoManager::deleteInfo(const TABLENAME &tableName, const QString &name)
{
    std::lock_guard<std::mutex> lock(mDBMutex);
    QSqlQuery query;
    QString cmd=QString("DELETE FROM %1 WHERE name = :name").arg(getTableName(tableName));
    query.prepare(cmd);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "Delete by name error:" << query.lastError();
        return false;
    }
    return true;
}

bool UserInfoManager::updateInfo(const TABLENAME &tableName, const QString &name, const QString &newValue)
{
    std::lock_guard<std::mutex> lock(mDBMutex);
    QSqlQuery query;
    QString cmd=QString("UPDATE %1 SET value = :value WHERE name = :name").arg(getTableName(tableName));
    query.prepare(cmd);
    query.bindValue(":value", newValue);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "Update error:" << query.lastError();
        return false;
    }
    return true;
}

QString UserInfoManager::getInfo(const TABLENAME &tableName, const QString &name, bool *ok)
{
    QSqlQuery query;
    QString cmd=QString("SELECT value FROM %1 WHERE name = :name").arg(getTableName(tableName));
    query.prepare(cmd);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "Query by name error:" << query.lastError();
        if (ok) *ok = false;
        return QString();
    }

    if (query.next()) {
        if (ok) *ok = true;
        return query.value("value").toString();
    }

    if (ok) *ok = false;
    return QString();
}

unsigned int UserInfoManager::getUILineWidth()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_LINE_WIDTH).toUInt();
}

unsigned int UserInfoManager::getUIPaddingScale()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_PADDING_SCALE).toUInt();
}

unsigned int UserInfoManager::getUIBackgroundRadius()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_RADIUS).toUInt();
}

unsigned int UserInfoManager::getUIBackgroundColorRed()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_COLOR_RED).toUInt();
}

unsigned int UserInfoManager::getUIBackgroundColorGreen()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_COLOR_GREEN).toUInt();
}

unsigned int UserInfoManager::getUIBackgroundColorBlue()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_COLOR_BLUE).toUInt();
}

unsigned int UserInfoManager::getUIBackgroundColorAlpha()
{
    return getInfo(TABLENAME::SETTINGS,UI_BACKGROUD_COLOR_ALPHA).toUInt();
}

unsigned int UserInfoManager::getUIFontColorRed()
{
    return getInfo(TABLENAME::SETTINGS,UI_FONT_COLOR_RED).toUInt();
}

unsigned int UserInfoManager::getUIFontColorGreen()
{
    return getInfo(TABLENAME::SETTINGS,UI_FONT_COLOR_GREEN).toUInt();
}

unsigned int UserInfoManager::getUIFontColorBlue()
{
    return getInfo(TABLENAME::SETTINGS,UI_FONT_COLOR_BLUE).toUInt();
}

unsigned int UserInfoManager::getUIFontColorAlpha()
{
    return getInfo(TABLENAME::SETTINGS,UI_FONT_COLOR_ALPHA).toUInt();
}

unsigned int UserInfoManager::getUICustomThemeIndex()
{
    return getInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_INDEX).toInt();
}

unsigned int UserInfoManager::getUICustomThemeEnable()
{
    return getInfo(UserInfoManager::SETTINGS,UI_CUSTOM_THEME_ENABLE).toInt();
}

unsigned int UserInfoManager::getUIStandbyTime()
{
    return getInfo(UserInfoManager::SETTINGS,UI_STANDBY_TIME).toInt();
}

unsigned int UserInfoManager::getUITimeFormat()
{
    return getInfo(UserInfoManager::SETTINGS,UI_TIME_FORMAT).toInt();
}

unsigned int UserInfoManager::getUITimeType()
{
    return getInfo(UserInfoManager::SETTINGS,UI_TIME_TYPE).toInt();
}

unsigned int UserInfoManager::getUISizeRatio()
{
    return getInfo(UserInfoManager::SETTINGS,UI_SIZE_RATIO).toInt();
}

unsigned int UserInfoManager::getUISizeFullRatio()
{
    return getInfo(UserInfoManager::SETTINGS,UI_SIZE_FULL_RATIO).toInt();
}

QString UserInfoManager::getOSName()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_NAME);
}

QString UserInfoManager::getOSVersion()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_VERSION);
}

QString UserInfoManager::getOSKernelVersion()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_KERNEL_VERSION);
}

QString UserInfoManager::getOSProductName()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_PRODUCT_NAME);
}

QString UserInfoManager::getOSArch()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_ARCH);
}

QString UserInfoManager::getOSUUID()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_UNIQUE_ID);
}

QString UserInfoManager::getSetupTime()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_SETUP_TIME);
}

QString UserInfoManager::getValidTime()
{
    return getInfo(UserInfoManager::SYSTEM,OS_INFO_VALID_TIME);
}

bool UserInfoManager::isPrimaryDisplayOnly()
{
    return getInfo(TABLENAME::SETTINGS,UI_PRIMARY_DISPLAY_ONLY).toInt();
}

bool UserInfoManager::isDisplaySecond()
{
    return getInfo(TABLENAME::SETTINGS,UI_DISPLAY_SECOND).toInt();
}

bool UserInfoManager::isDate12HourFormat()
{
    return getInfo(TABLENAME::SETTINGS,UI_DATE_12_HOUR_FORMAT).toInt();
}

bool UserInfoManager::isCustomThemeEnable()
{
    return getInfo(TABLENAME::SETTINGS,UI_CUSTOM_THEME_ENABLE).toInt();
}

bool UserInfoManager::isShowDateInfo()
{
    return getInfo(TABLENAME::SETTINGS,UI_SHOW_DATE_INFO).toInt();
}

bool UserInfoManager::isSelfStaring()
{
    return getInfo(TABLENAME::SETTINGS,UI_SELF_STARING).toInt();
}

QString UserInfoManager::getUIFontFamily()
{
    return getInfo(TABLENAME::SETTINGS,UI_FONT_FAMILY);
}

QString UserInfoManager::getTableName(const TABLENAME &tableName)
{
    QString name;
    switch (tableName) {
    case TABLENAME::SETTINGS:
        name="settings";
        break;
    case TABLENAME::SYSTEM:
        name="system";
        break;
    default:
        break;
    }
    return name;
}

UserInfoManager *UserInfoManager::getInterface()
{
    //第一次检查：实例化单例对象后，就不会再进入加锁逻辑
    if (mUserInfoManager == nullptr)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        //第二次检查：可能两个线程同时通过第一次检查，一个线程获得锁时，可能另外一个线程已经实例化单体
        if (mUserInfoManager == nullptr)
        {
            mUserInfoManager = new UserInfoManager();
        }
    }
    return mUserInfoManager;
}
