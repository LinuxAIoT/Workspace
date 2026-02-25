#include "switchbutton.h"

SwitchButton::SwitchButton(QWidget *parent)
    : QWidget(parent),
    mOffset(2),
    mChecked(false),
    mCheckedColor(0, 120, 215),
    mUncheckedColor(Qt::gray)
{
    setFixedSize(32, 16);
}

void SwitchButton::paintEvent(QPaintEvent *)
{

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    QRect rect(0, 0, width(), height());
    p.setPen(Qt::NoPen);
    p.setBrush(mChecked ? mCheckedColor : mUncheckedColor);
    p.drawRoundedRect(rect, 8, 8);

    // 绘制滑块
    p.setBrush(Qt::white);
    p.drawEllipse(mOffset, 2, height() - 4, height() - 4);

}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
        mDragStart = event->pos().x();
        mOffsetStart = mOffset;
    }
    QWidget::mousePressEvent(event);

}

void SwitchButton::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton) {
        int delta = event->pos().x() - mDragStart;
        int newOffset = qBound(2, mOffsetStart + delta, width() - height() + 2);
        setOffset(newOffset);
    }

}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        bool click = qAbs(event->pos().x() - mDragStart) < 5;
        bool checked = click ? !mChecked : mOffset > width()/2;
        setChecked(checked);
    }
}

void SwitchButton::setChecked(bool checked)
{
    if (mChecked != checked) {
        mChecked = checked;
        animate(checked);
        emit toggled(mChecked);
    }
}

int SwitchButton::offset() const
{
    return mOffset;
}

void SwitchButton::setOffset(int o)
{
    mOffset = o;
    update();
}

QColor SwitchButton::checkedColor() const
{
    return mCheckedColor;
}

void SwitchButton::setCheckedColor(const QColor &color)
{
    mCheckedColor = color;
    update();
}

QColor SwitchButton::uncheckedColor() const
{
    return mUncheckedColor;
}

void SwitchButton::setUncheckedColor(const QColor &color)
{
    mUncheckedColor = color;
    update();
}

void SwitchButton::animate(bool checked)
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "offset");
    anim->setDuration(120);
    anim->setStartValue(mOffset);
    anim->setEndValue(checked ? width() - height() + 2 : 2);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
