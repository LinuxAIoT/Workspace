#ifndef TIMEPICKER_H
#define TIMEPICKER_H

#include <QTime>
#include <QWidget>
#include <QListWidgetItem>
#include <QEvent>

class QListWidget;

class TimePicker : public QWidget
{
    Q_OBJECT
public:
    explicit TimePicker(QWidget *parent = nullptr);

    QTime time() const;
    void setTime(const QTime &time);

signals:
    void timeChanged(const QTime &time);
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void onHourClicked(QListWidgetItem *item);
    void onMinuteClicked(QListWidgetItem *item);
    void onSecondClicked(QListWidgetItem *item);

private:
    QListWidget *m_hourList;
    QListWidget *m_minuteList;
    QListWidget *m_secondList;

    int m_hour;
    int m_minute;
    int m_second;

    void setupList(QListWidget *list, int min, int max);
    void updateList(QListWidget *list, int current, int min, int max);
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // TIMEPICKER_H
