#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->spinBox->setRange(1,99);
    ui->spinBox->setSingleStep(1);
    ui->spinBox->setSuffix("min");  //后缀
    ui->spinBox->setValue(10);      //默认10分钟
    mDefaultTimeout=ui->spinBox->value();
}

Widget::~Widget()
{
    delete ui;
}

QString Widget::generateVerificationCode()
{
    ui->plainTextEdit->appendPlainText("generateVerificationCode:");
    // 获取当前时间戳（秒级）
    quint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    QString timestampStr =QString::number(timestamp);
    QString timeout=QString::number(ui->spinBox->value()).rightJustified(2, '0');

    ui->plainTextEdit->appendPlainText("current timestamp:"+timestampStr);
    ui->plainTextEdit->appendPlainText("timeout:"+QString::number(timeout.toInt()*60));
    // 生成哈希输入
    QString hashInput = timestampStr +mSecretKey;
    ui->plainTextEdit->appendPlainText("hashInput:"+hashInput);
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(hashInput.toStdString().c_str(),hashInput.size());
    QString hashStr= hash.result().toHex();
    ui->plainTextEdit->appendPlainText("hashStr:"+hashStr);
    ui->plainTextEdit->appendPlainText("result:"+timestampStr + timeout + hashStr);
    return timestampStr + timeout + hashStr.right(mDefaultHashNum);
}

bool Widget::verify(QString &code)
{
    ui->plainTextEdit->appendPlainText("verify:");
    if(code.length()!=28){//10 位时间戳+2位有效时间+16位hash
        ui->plainTextEdit->appendPlainText("verify code error");
        return false;
    }
    // 分离时间戳和哈希部分
    quint64 storedTimestamp =code.left(10).toULongLong();
    quint64 timeout= code.left(12).right(2).toULongLong();
    mDefaultTimeout= timeout*60;//s
    QString storedHash = code.right(mDefaultHashNum);
    ui->plainTextEdit->appendPlainText("storedHash:"+storedHash);
    ui->plainTextEdit->appendPlainText("timeout:"+QString::number(mDefaultTimeout));

    quint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    ui->plainTextEdit->appendPlainText("currentTimestamp:"+QString::number(currentTimestamp));
    if (currentTimestamp - storedTimestamp > mDefaultTimeout){
        ui->plainTextEdit->appendPlainText("verify out of time");
        return false;
    }

    // 重新计算哈希
    QString hashInput =QString::number(storedTimestamp) + mSecretKey;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(hashInput.toStdString().c_str(),hashInput.size());
    QString generatedHash = hash.result().toHex().right(mDefaultHashNum);
    ui->plainTextEdit->appendPlainText("generatedHash:"+generatedHash);
    ui->plainTextEdit->appendPlainText("result:"+QString::number(storedHash == generatedHash));
    return storedHash == generatedHash;
}


void Widget::on_pushButton_clicked()
{
    ui->lineEdit->clear();
    QString code= generateVerificationCode();
    verify(code);
    ui->lineEdit->setText(code);
}

