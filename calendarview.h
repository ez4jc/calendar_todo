#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QDate>
#include <QMap>
#include <QLocale>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "calendarcell.h"

class CalendarView : public QWidget {
    Q_OBJECT

public:
    explicit CalendarView(QWidget *parent = nullptr);

    void setCurrentMonth(int year, int month, bool animated = true);
    void refreshCells();
    void editDate(const QDate &date);
    int currentYear() const { return m_isFadingOut ? m_targetYear : m_year; }
    int currentMonth() const { return m_isFadingOut ? m_targetMonth : m_month; }

signals:
    void dateDoubleClicked(const QDate &date);

private slots:
    void onCellDoubleClicked(const QDate &date);
    void onCellTodosEdited(const QDate &date, const QList<TodoItem> &todos);
    void onTodoCompletionChanged(int todoId, bool completed);
    void onTransitionFinished();

private:
    void applyMonth(int year, int month);
    void setupWeekdayHeaders();
    void updateCalendarGrid();

    QGridLayout *m_gridLayout;
    QMap<QDate, CalendarCell*> m_cells;
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_transitionAnimation;
    int m_year;
    int m_month;
    int m_targetYear;
    int m_targetMonth;
    bool m_isFadingOut;
    Qt::DayOfWeek m_firstDayOfWeek;

    static const int s_daysInWeek = 7;
    static const int s_weeksInMonth = 6;
};

#endif // CALENDARVIEW_H
