#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public:
    QString generateVerificationCode();
    bool verify(QString &code);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    const QString   mSecretKey      = "TickTimer";//用于生成Hash
    unsigned int    mDefaultTimeout = 600;//默认验证码有效时间10分钟
    unsigned int    mDefaultHashNum = 16; //默认使用hash的后x位作为校验
};
#endif // WIDGET_H
