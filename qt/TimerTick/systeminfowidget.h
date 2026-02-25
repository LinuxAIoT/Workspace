#ifndef SYSTEMINFOWIDGET_H
#define SYSTEMINFOWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
namespace Ui {
class SystemInfoWidget;
}

class SystemInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInfoWidget(QWidget *parent = nullptr);
    ~SystemInfoWidget();
public:
    bool verify(QString &code);
protected:
     void resizeEvent(QResizeEvent *event) override;
private slots:
    void on_BtnOK_clicked();

private:
    Ui::SystemInfoWidget *ui;
    const QString   mSecretKey      = "TickTimer";//用于生成Hash
    unsigned int    mDefaultTimeout = 600;//默认验证码有效时间10分钟
    unsigned int    mDefaultHashNum = 16; //默认使用hash的后x位作为校验
};

#endif // SYSTEMINFOWIDGET_H
