#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QDate>
#include <QMap>
#include "calendarcell.h"

class CalendarView : public QWidget {
    Q_OBJECT

public:
    explicit CalendarView(QWidget *parent = nullptr);

    void setCurrentMonth(int year, int month);
    void refreshCells();
    int currentYear() const { return m_year; }
    int currentMonth() const { return m_month; }

signals:
    void dateDoubleClicked(const QDate &date);

private slots:
    void onCellDoubleClicked(const QDate &date);

private:
    void setupWeekdayHeaders();
    void updateCalendarGrid();

    QGridLayout *m_gridLayout;
    QMap<QDate, CalendarCell*> m_cells;
    int m_year;
    int m_month;

    static const int s_daysInWeek = 7;
    static const int s_weeksInMonth = 6;
};

#endif // CALENDARVIEW_H
