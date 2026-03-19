#include "calendarview.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include "todomanager.h"

CalendarView::CalendarView(QWidget *parent)
    : QWidget(parent)
    , m_gridLayout(nullptr)
    , m_year(QDate::currentDate().year())
    , m_month(QDate::currentDate().month())
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setContentsMargins(5, 5, 5, 5);
    m_gridLayout->setSpacing(2);

    setupWeekdayHeaders();
    updateCalendarGrid();
}

void CalendarView::setupWeekdayHeaders() {
    QStringList weekdaysNames = {"一", "二", "三", "四", "五", "六", "日"};

    for (int i = 0; i < s_daysInWeek; ++i) {
        QLabel *headerLabel = new QLabel(weekdaysNames[i], this);
        headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        headerLabel->setStyleSheet(
            "QLabel { "
            "   color: rgba(255, 255, 255, 0.7); "
            "   font-weight: bold; "
            "   font-size: 12px; "
            "   background-color: transparent; "
            "}"
        );
        m_gridLayout->addWidget(headerLabel, 0, i);
    }
}

void CalendarView::updateCalendarGrid() {
    for (auto cell : m_cells.values()) {
        cell->clearTodos();
        delete cell;
    }
    m_cells.clear();

    for (int row = 1; row <= s_weeksInMonth; ++row) {
        for (int col = 0; col < s_daysInWeek; ++col) {
            CalendarCell *cell = new CalendarCell(this);
            connect(cell, &CalendarCell::doubleClicked,
                    this, &CalendarView::onCellDoubleClicked);
            m_gridLayout->addWidget(cell, row, col);
        }
    }

    setCurrentMonth(m_year, m_month);
}

void CalendarView::setCurrentMonth(int year, int month) {
    m_year = year;
    m_month = month;

    QDate firstDayOfMonth(year, month, 1);
    int firstDayWeekday = firstDayOfMonth.dayOfWeek();
    int startOffset = (firstDayWeekday - 1) % 7;

    QDate prevMonthDate = firstDayOfMonth.addDays(-startOffset);

    QList<TodoItem> monthTodos = TodoManager::instance().getTodosByMonth(year, month);
    QMap<QDate, QList<TodoItem>> todosByDate;
    for (const TodoItem &todo : monthTodos) {
        todosByDate[todo.date].append(todo);
    }

    QDate currentDate = prevMonthDate;

    for (int week = 1; week <= s_weeksInMonth; ++week) {
        for (int day = 0; day < s_daysInWeek; ++day) {
            CalendarCell *cell = qobject_cast<CalendarCell*>(
                m_gridLayout->itemAtPosition(week, day)->widget()
            );

            if (cell) {
                cell->setDate(currentDate);
                cell->setIsCurrentMonth(currentDate.month() == month);

                if (todosByDate.contains(currentDate)) {
                    for (const TodoItem &todo : todosByDate[currentDate]) {
                        cell->addTodo(todo);
                    }
                }

                m_cells[currentDate] = cell;
            }

            currentDate = currentDate.addDays(1);
        }
    }
}

void CalendarView::refreshCells() {
    setCurrentMonth(m_year, m_month);
}

void CalendarView::onCellDoubleClicked(const QDate &date) {
    if (date.isValid()) {
        emit dateDoubleClicked(date);
    }
}
