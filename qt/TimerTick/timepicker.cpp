#include "timepicker.h"
#include <QHBoxLayout>
#include <QListWidget>
#include <QWheelEvent>
#include <QDebug>
#include <QPushButton>
TimePicker::TimePicker(QWidget *parent)
    : QWidget(parent),
    m_hour(0),
    m_minute(0),
    m_second(0)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    QHBoxLayout *hlayout_button = new QHBoxLayout();
    QPushButton *btn_ok=new QPushButton();
    btn_ok->setText("ok");
    QPushButton *btn_cancle=new QPushButton();
    btn_cancle->setText("cancle");
    hlayout_button->addWidget(btn_ok);
    hlayout_button->addWidget(btn_cancle);
    hlayout->setSpacing(2);
    hlayout->setContentsMargins(2, 2, 2, 2);
    m_hourList = new QListWidget(this);
    m_minuteList = new QListWidget(this);
    m_secondList = new QListWidget(this);
    setupList(m_hourList, 0, 23);
    setupList(m_minuteList, 0, 59);
    setupList(m_secondList, 0, 59);

    hlayout->addWidget(m_hourList);
    hlayout->addWidget(m_minuteList);
    hlayout->addWidget(m_secondList);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout_button);
    connect(m_hourList, &QListWidget::itemClicked, this, &TimePicker::onHourClicked);
    connect(m_minuteList, &QListWidget::itemClicked, this, &TimePicker::onMinuteClicked);
    connect(m_secondList, &QListWidget::itemClicked, this, &TimePicker::onSecondClicked);

    //设置固定大小
//    setFixedSize(QSize(390,210));
    setTime(QTime::currentTime());
}

QTime TimePicker::time() const
{
    return QTime(m_hour, m_minute, m_second);
}

void TimePicker::setTime(const QTime &time)
{
    if (!time.isValid())
        return;

    m_hour = time.hour();
    m_minute = time.minute();
    m_second = time.second();

    updateList(m_hourList, m_hour, 0, 23);
    updateList(m_minuteList, m_minute, 0, 59);
    updateList(m_secondList, m_second, 0, 59);
}

void TimePicker::resizeEvent(QResizeEvent *event)
{
    updateList(m_hourList, m_hour, 0, 23);
    updateList(m_minuteList, m_minute, 0, 59);
    updateList(m_secondList, m_second, 0, 59);
}

void TimePicker::onHourClicked(QListWidgetItem *item)
{
    int newHour = item->text().toInt();
    if (newHour != m_hour) {
        m_hour = newHour;
        updateList(m_hourList, m_hour, 0, 23);
        emit timeChanged(time());
    }
}

void TimePicker::onMinuteClicked(QListWidgetItem *item)
{
    int newMinute = item->text().toInt();
    if (newMinute != m_minute) {
        m_minute = newMinute;
        updateList(m_minuteList, m_minute, 0, 59);
        emit timeChanged(time());
    }
}

void TimePicker::onSecondClicked(QListWidgetItem *item)
{
    int newSecond = item->text().toInt();
    if (newSecond != m_second) {
        m_second = newSecond;
        updateList(m_secondList, m_second, 0, 59);
        emit timeChanged(time());
    }
}

void TimePicker::setupList(QListWidget *list, int min, int max)
{
    list->setFixedWidth(120);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setFrameShape(QListWidget::NoFrame);
    list->setSelectionMode(QListWidget::SingleSelection);
    list->viewport()->installEventFilter(this);
    updateList(list, min, min, max);
}

void TimePicker::updateList(QListWidget *list, int current, int min, int max)
{
    list->clear();

    int range = max - min + 1;
    for (int i = -2; i <= 2; ++i) {
        int offset = (current - min) + i;
        int value = min + (offset % range + range) % range;
        QListWidgetItem *item = new QListWidgetItem(QString("%1").arg(value, 2, 10, QLatin1Char('0')));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        list->addItem(item);
    }

    list->setCurrentRow(2);
    list->scrollToItem(list->item(2), QAbstractItemView::PositionAtCenter);
}

bool TimePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        QListWidget *list = nullptr;
        int *current = nullptr;
        int min = 0, max = 0;

        if (watched == m_hourList->viewport()) {
            list = m_hourList;
            current = &m_hour;
            min = 0;
            max = 23;
        } else if (watched == m_minuteList->viewport()) {
            list = m_minuteList;
            current = &m_minute;
            min = 0;
            max = 59;
        } else if (watched == m_secondList->viewport()) {
            list = m_secondList;
            current = &m_second;
            min = 0;
            max = 59;
        } else {
            return QWidget::eventFilter(watched, event);
        }

        int delta = wheelEvent->angleDelta().y();
        if (delta == 0) {
            return QWidget::eventFilter(watched, event);
        }

        int steps = delta > 0 ? 1 : -1;
        *current += steps;

        // 处理边界
        if (*current > max) {
            *current = min;
        } else if (*current < min) {
            *current = max;
        }

        updateList(list, *current, min, max);
        emit timeChanged(time());
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
