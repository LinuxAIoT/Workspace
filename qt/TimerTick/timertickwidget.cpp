#include "timertickwidget.h"
#include <QPainterPath>
//标识是否设置了自定主题
bool TimerTickWidget::mIsCustomTheme=false;
QColor TimerTickWidget::mLineColor=Qt::black;
TimerTickWidget::TimerTickWidget(QWidget *parent)
    : QWidget{parent},
    mUserInfoManager(nullptr),
    mCurrentNumber(0),
    mPaddingScale(0),
    mRadius(0),
    mLineWidth(0),
    mBgColorAlpha(0),
    mBackgroundColor(Qt::black),
    mCurrentAPM(QString())
{
    this->setWindowTitle("TimerTicker");
    resize(640,480);
    initConfige();
    connect(&mTimerLine, SIGNAL(frameChanged(int)), SLOT(update()));
    mTimerLine.setFrameRange(0, 100);
    mTimerLine.setDuration(600);
#ifdef DEBUG
    mTimer=new QTimer(this);
    connect(mTimer,&QTimer::timeout,[this]{
        auto t=QTime::currentTime();
        this->updateFrame(t.second());
    });
    mTimer->start(1000);
#endif
}

TimerTickWidget::~TimerTickWidget()
{
#ifdef DEBUG
    if(mTimer){
        mTimer->stop();
    }
#endif
}

void TimerTickWidget::initConfige()
{
    mUserInfoManager=UserInfoManager::getInterface();
    mRadius=mUserInfoManager->getUIBackgroundRadius();

    mPaddingScale=mUserInfoManager->getUIPaddingScale();
    mLineWidth=mUserInfoManager->getUILineWidth();
    QString fontFamily=mUserInfoManager->getUIFontFamily();
    mFont.setFamily(fontFamily);
    auto bgRed=mUserInfoManager->getUIBackgroundColorRed();
    auto bgGreen=mUserInfoManager->getUIBackgroundColorGreen();
    auto bgBlue=mUserInfoManager->getUIBackgroundColorBlue();
    auto bgAlpha=mUserInfoManager->getUIBackgroundColorAlpha();
    mBackgroundColor=QColor(bgRed,bgGreen,bgBlue,bgAlpha);

    auto fontRed=mUserInfoManager->getUIFontColorRed();
    auto fontGreen=mUserInfoManager->getUIFontColorGreen();
    auto fontBlue=mUserInfoManager->getUIFontColorBlue();
    auto fontAlpha=mUserInfoManager->getUIFontColorAlpha();
    mFontCorlor=QColor(fontRed,fontGreen,fontBlue);
}

void TimerTickWidget::paintEvent(QPaintEvent *event)
{
    if(mTimerLine.state() == QTimeLine::Running)
    {
        paintFlip();
    } else {
        paintStatic();
    }
}

void TimerTickWidget::resizeEvent(QResizeEvent *event)
{
    preparePixmap();
    update();
}

void TimerTickWidget::keyPressEvent(QKeyEvent *event)
{

}

void TimerTickWidget::drawRoundedRectWithGap(QPainter* painter, const QRect& rect, int radius, int gapWidth)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 计算空白区域位置（居中）
    int gapCenterY = rect.center().y();
    int gapTop = gapCenterY - gapWidth/2;
    int gapBottom = gapCenterY + gapWidth/2;

    // 绘制上部（顶部圆角，底部直角）
    QPainterPath topPath;
    topPath.moveTo(rect.left(), gapTop);  // 左下角（连接处）
    topPath.lineTo(rect.left(), rect.top() + radius);  // 左侧直线
    topPath.arcTo(rect.left(), rect.top(), radius*2, radius*2, 180, -90);  // 左上圆角
    topPath.lineTo(rect.right() - radius, rect.top());  // 顶部直线
    topPath.arcTo(rect.right() - radius*2, rect.top(), radius*2, radius*2, 90, -90);  // 右上圆角
    topPath.lineTo(rect.right(), gapTop);  // 右侧直线
    topPath.lineTo(rect.left(), gapTop);  // 底部直线（连接处）

    // 绘制下部（顶部直角，底部圆角）
    QPainterPath bottomPath;
    bottomPath.moveTo(rect.left(), gapBottom);  // 左上角（连接处）
    bottomPath.lineTo(rect.left(), rect.bottom() - radius);  // 左侧直线
    bottomPath.arcTo(rect.left(), rect.bottom() - radius*2, radius*2, radius*2, 180, 90);  // 左下圆角
    bottomPath.lineTo(rect.right() - radius, rect.bottom());  // 底部直线
    bottomPath.arcTo(rect.right() - radius*2, rect.bottom() - radius*2, radius*2, radius*2, 270, 90);  // 右下圆角
    bottomPath.lineTo(rect.right(), gapBottom);  // 右侧直线
    bottomPath.lineTo(rect.left(), gapBottom);  // 顶部直线（连接处）

    // 绘制两部分
    painter->drawPath(topPath);
    painter->drawPath(bottomPath);

    //绘制时间制式AM/PM
    QPen pen;
    pen.setColor(mFontCorlor);
    QFont font;
    auto fontPixSize=rect.height()*0.16;
    font.setFamily(mFont.family());
    font.setPixelSize(fontPixSize);
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(QPoint(rect.x()+fontPixSize/5,rect.y()+fontPixSize),mCurrentAPM);
    painter->restore();
}

void TimerTickWidget::drawFrame(QPainter *p, const QRect &rect)
{
    p->setPen(Qt::NoPen);
    p->setBrush(mBackgroundColor);
    drawRoundedRectWithGap(p,rect,mRadius,mLineWidth);
}

void TimerTickWidget::paintStatic()
{
    QPainter painter(this);
    // 启用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRectF backgroundRect = rect();
    painter.fillRect(backgroundRect, Qt::transparent);
    if(mPaddingScale==0){
        mPaddingScale = 10;
    }
    int pad = width() / mPaddingScale;
    drawFrame(&painter, rect().adjusted(pad, pad, -pad, -pad));
    mPadding=pad;
    painter.drawPixmap(0,0,mCurrentPixmap);
}

void TimerTickWidget::paintFlip()
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::transparent);

    int hw = width() / 2;
    int hh = height() / 2;

    // behind is the new pixmap
    int pad = width() / 10;
    QRect fr = rect().adjusted(pad, pad, -pad, -pad);
    drawFrame(&p, fr);
    p.drawPixmap(0, 0, mCurrentPixmap);

    int index = mTimerLine.currentFrame();

    if (index <= 50)
    {
        // the top part of the old pixmap is flipping
        int angle = -180 * index / 100;
        QTransform transform;
        transform.translate(hw, hh);
        //Moves the coordinate system to the center of widget

        transform.rotate(angle, Qt::XAxis);
        //Rotates the coordinate system counterclockwise by angle about the X axis

        p.setTransform(transform);
        drawFrame(&p, fr.adjusted(-hw, -hh, -hw, -hh));
        p.drawPixmap(-hw, -hh, mLastPixmap);

        // the bottom part is still the old pixmap
        p.resetTransform();
        p.setClipRect(0, hh, width(), hh);
        //Enables clipping, and sets the clip region to the rectangle beginning at (0, hh) with the given width and height

        drawFrame(&p, fr);
        p.drawPixmap(0, 0, mLastPixmap);
    }
    else
    {
        p.setClipRect(0, hh, width(), hh);

        // the bottom part is still the old pixmap
        drawFrame(&p, fr);
        p.drawPixmap(0, 0, mLastPixmap);

        // the bottom part of the new pixmap is flipping
        int angle = 180 - 180 * mTimerLine.currentFrame() / 100;
        QTransform transform;
        transform.translate(hw, hh);
        transform.rotate(angle, Qt::XAxis);
        p.setTransform(transform);
        drawFrame(&p, fr.adjusted(-hw, -hh, -hw, -hh));
        p.drawPixmap(-hw, -hh, mCurrentPixmap);
    }
}

void TimerTickWidget::preparePixmap()
{
    mCurrentPixmap = QPixmap(size());
    mCurrentPixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&mCurrentPixmap);
    painter.drawPixmap(0, 0, drawDigits(mCurrentNumber, rect()));
    painter.end();
    update();
}

//删除文字中间的像素
void TimerTickWidget::makeLineTransparent(QPixmap& pixmap, const QPoint& startPoint,const QPoint& endPoint)
{
    if (pixmap.isNull()) return;

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);

    // 根据方向创建矩形区域
    QRect lineRect;
    auto len=endPoint.x()-startPoint.x();
    lineRect = QRect(startPoint.x(), startPoint.y(), len, mLineWidth); // 水平线

    painter.fillRect(lineRect, Qt::transparent);
    painter.end();
}


void TimerTickWidget::updateFrame(int numer)
{
    if(numer!=mCurrentNumber){
        mCurrentNumber = qBound(0, numer, 99);
        mLastPixmap = mCurrentPixmap;
        preparePixmap();
        update();
        mTimerLine.stop();
        mTimerLine.start();
    }
}

void TimerTickWidget::setCurrentAPM(bool enable)
{
    if(enable){
        auto t=QTime::currentTime();
        if(t.hour()>12){
            mCurrentAPM="PM";
        }else{
            mCurrentAPM="AM";
        }
    }
}

void TimerTickWidget::setBgColorRed(unsigned int value)
{
    mBackgroundColor.setRed(value);
}

void TimerTickWidget::setBgColorGreen(unsigned int value)
{
    mBackgroundColor.setGreen(value);
}

void TimerTickWidget::setBgColorBlue(unsigned int value)
{
    mBackgroundColor.setBlue(value);
}

void TimerTickWidget::setBgColorAlpha(unsigned int value)
{
    mBgColorAlpha=value;
    mBackgroundColor.setAlpha(value);
}

void TimerTickWidget::setFontColorRed(unsigned int value)
{
    mFontCorlor.setRed(value);
}

void TimerTickWidget::setFontColorGreen(unsigned int value)
{
    mFontCorlor.setGreen(value);
}

void TimerTickWidget::setFontColorBlue(unsigned int value)
{
    mFontCorlor.setBlue(value);
}

void TimerTickWidget::setFontColorAlpha(unsigned int value)
{
    mFontCorlor.setAlpha(value);
}

void TimerTickWidget::setFontFamily(QString family)
{
    mFont.setFamily(family);
}

void TimerTickWidget::setName(QString name)
{
    mName=name;
}

void TimerTickWidget::updateFrameStyle()
{
    preparePixmap();
    update();
}

int TimerTickWidget::getFontPixelSize()
{
    return mFont.pixelSize();
}

void TimerTickWidget::isCustomTheme(bool enable)
{
    mIsCustomTheme=enable;
}

void TimerTickWidget::setLineColor(QColor color)
{
    mLineColor=color;
}

QPixmap TimerTickWidget::drawDigits(int number, const QRect &r)
{
    int scaleFactor = 3;

    // 如果数字只有一位，前置0.
    QString formatted = QString::number(number).rightJustified(2, '0');

    int fontHeight = scaleFactor * 0.50 * r.height();
    mFont.setPixelSize(fontHeight);
    mFont.setBold(true);

    QPixmap pixmap(r.size() * scaleFactor);
    pixmap.fill(Qt::transparent);

    QPainter p;
    p.begin(&pixmap);
    p.setFont(mFont);

    //设置字体颜色
    QPen pen;
    pen.setColor(mFontCorlor);
    p.setPen(pen);
    p.drawText(pixmap.rect(), Qt::AlignCenter, formatted);
    p.end();
    int pad = width() / mPaddingScale;
    int w=rect().adjusted(pad, pad, -pad, -pad).width();
    auto ret=pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
    QPoint start=QPoint((ret.width()-w)/2+mLineWidth/2,ret.height()/2-mLineWidth/2);
    QPoint end=QPoint(ret.width()-(ret.width()-w)/2-mLineWidth/2,ret.height()/2-mLineWidth/2);

    //去除中间像素
    makeLineTransparent(ret,start,end);

    return ret;
}
