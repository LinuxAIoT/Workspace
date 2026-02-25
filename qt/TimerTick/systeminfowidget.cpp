#include "systeminfowidget.h"
#include "ui_systeminfowidget.h"
#include "userinfomanager.h"
#include <QCryptographicHash>
#include <QMessageBox>
SystemInfoWidget::SystemInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemInfoWidget)
{
    ui->setupUi(this);
    QString info=R"(
    ​Title:​​ Software License Expiring Soon
    ​Content:​​
    Dear User,
    Your software license will expire on [%1].
    To ensure uninterrupted service,
    please complete QR code activation on the left;
    )";
    auto size=QSize(104,104);
    ui->toolButton->setFixedSize(size);
    ui->toolButton->setIconSize(size);
    ui->toolButton->setIcon(QIcon(":/icon/icon/contact.jpg"));
    auto userInfoManager= UserInfoManager::getInterface();
    auto date=userInfoManager->getInfo(UserInfoManager::SYSTEM,OS_INFO_VALID_TIME);
    ui->label->setWordWrap(true);
    ui->label->setText(info.arg(date));
    ui->lineEdit->setPlaceholderText("Please enter a verification code");
    setWindowTitle("Software License Info");
    resize(435,175);
    setWindowIcon(QIcon(":/icon/icon/APP_64.png"));
}

SystemInfoWidget::~SystemInfoWidget()
{
    delete ui;
}

bool SystemInfoWidget::verify(QString &code)
{
    qInfo()<<__FUNCTION__<<" "<<__LINE__;
    if(code.length()!=28){//10 位时间戳+2位有效时间+16位hash
        qFatal()<<"verify code error";
        return false;
    }
    // 分离时间戳和哈希部分
    quint64 storedTimestamp =code.left(10).toULongLong();
    quint64 timeout= code.left(12).right(2).toULongLong();
    mDefaultTimeout= timeout*60;//s
    qDebug()<<"timeout:"<<mDefaultTimeout;
    QString storedHash = code.right(mDefaultHashNum);
    quint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    if (currentTimestamp - storedTimestamp > mDefaultTimeout){
        qInfo()<<"verify out of time,"
                 <<" current timestamp: "<<QString::number(currentTimestamp)
                 <<" stored timestamp: "<<QString::number(storedTimestamp);
        return false;
    }
    // 重新计算哈希
    QString hashInput =QString::number(storedTimestamp) + mSecretKey;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(hashInput.toStdString().c_str(),hashInput.size());
    QString generatedHash = hash.result().toHex().right(mDefaultHashNum);
    return storedHash == generatedHash;
}

void SystemInfoWidget::resizeEvent(QResizeEvent *event)
{
    //qDebug()<<"size:"<<this->size();
}


void SystemInfoWidget::on_BtnOK_clicked()
{
    QString code = ui->lineEdit->text().trimmed();
    auto ret=verify(code);
    if((code.size()<28)||!(ret)){
        // 显示阻塞式错误对话框
        QMessageBox::critical(this, "Code Error", "Entering a verification code is invalid", QMessageBox::Ok);
        return;
    }
    //更新有效时间
    if(UserInfoManager::getInterface()->updateInfo(UserInfoManager::SYSTEM,OS_INFO_VALID_TIME,"2500-01-01")){
        QMessageBox::information(this,"Active Success","The verification code has been successfully activated, thank you for your support",QMessageBox::Ok);
    }
}

