#ifndef TIMERTICKWIDGET_H
#define TIMERTICKWIDGET_H

#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimeLine>
#include <QKeyEvent>

#include "userinfomanager.h"
//#define DEBUG

class TimerTickWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimerTickWidget(QWidget *parent = nullptr);
    ~TimerTickWidget();
    void initConfige();
    void updateFrame(int numer);
    void setCurrentAPM(bool enable);
    void setBgColorRed(unsigned int value);
    void setBgColorGreen(unsigned int value);
    void setBgColorBlue(unsigned int value);
    void setBgColorAlpha(unsigned int value);
    void setFontColorRed(unsigned int value);
    void setFontColorGreen(unsigned int value);
    void setFontColorBlue(unsigned int value);
    void setFontColorAlpha(unsigned int value);
    void setFontFamily(QString family);
    void setName(QString name);
    void updateFrameStyle();
    int  getFontPixelSize();
    static void isCustomTheme(bool enable);
    static void setLineColor(QColor color);
signals:
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    UserInfoManager *mUserInfoManager;  // 用户数据
    QPixmap mCurrentPixmap;             // 当前数字图像.
    QPixmap mLastPixmap;                // 上一幅数字图像.
    QTimeLine mTimerLine;               // 动画时间线.
    QTimer *mTimer;                     // 定时器.
    int mCurrentNumber;                 // 数字.

    //Themem Param
    unsigned int    mPaddingScale;      //内边距缩放系数
    unsigned int    mPadding;           //内边距
    unsigned int    mRadius;            //圆角
    unsigned int    mLineWidth;         //直线
    unsigned int    mBgColorAlpha;      //默认背景色Alpha;
    static QColor   mLineColor;         //直线颜色
    QColor          mBackgroundColor;   //背景色
    QFont           mFont;              //字体
    QColor          mFontCorlor;        //字体颜色
    static bool     mIsCustomTheme;     //自定主题
    QString         mCurrentAPM;        //AM/PM
    QString         mName;              //部件名称
private:
    void paintStatic();                 //静态
    void paintFlip();                   //切页动画
    void preparePixmap();               //准备背景

    QPixmap drawDigits(int number, const QRect &rect);
    void drawFrame(QPainter *painter, const QRect &rect);
    void makeLineTransparent(QPixmap& pixmap, const QPoint& startPoint,const QPoint& endPoint);
    void drawRoundedRectWithGap(QPainter* painter, const QRect& rect, int radius, int gapWidth);
};

#endif // TIMERTICKWIDGET_H
