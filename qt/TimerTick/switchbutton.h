#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPainter>

class SwitchButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(QColor checkedColor READ checkedColor WRITE setCheckedColor)
    Q_PROPERTY(QColor uncheckedColor READ uncheckedColor WRITE setUncheckedColor)
public:
    explicit SwitchButton(QWidget *parent = nullptr);
    void setChecked(bool checked);
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
signals:
    void toggled(bool checked);
    void offsetChanged(int);
private:
    int mOffset;
    bool mChecked;
    QColor mCheckedColor;
    QColor mUncheckedColor;
    int mDragStart;
    int mOffsetStart;

    int offset() const ;
    void setOffset(int o);
    QColor checkedColor() const;
    QColor uncheckedColor() const ;
    void setCheckedColor(const QColor &color);
    void setUncheckedColor(const QColor &color);
    void animate(bool checked);
};

#endif // SWITCHBUTTON_H
