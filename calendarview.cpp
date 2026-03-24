#include "calendarview.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QEasingCurve>
#include <QDebug>
#include "todomanager.h"

CalendarView::CalendarView(QWidget *parent)
    : QWidget(parent)
    , m_gridLayout(nullptr)
    , m_opacityEffect(nullptr)
    , m_transitionAnimation(nullptr)
    , m_year(QDate::currentDate().year())
    , m_month(QDate::currentDate().month())
    , m_targetYear(m_year)
    , m_targetMonth(m_month)
    , m_isFadingOut(false)
    , m_firstDayOfWeek(QLocale::system().firstDayOfWeek())
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setContentsMargins(5, 5, 5, 5);
    m_gridLayout->setSpacing(2);

    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);

    m_transitionAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    connect(m_transitionAnimation, &QPropertyAnimation::finished,
            this, &CalendarView::onTransitionFinished);

    setupWeekdayHeaders();
    updateCalendarGrid();
}

void CalendarView::setupWeekdayHeaders() {
    QStringList weekdaysNames;
    weekdaysNames << "" << "一" << "二" << "三" << "四" << "五" << "六" << "日";

    for (int i = 0; i < s_daysInWeek; ++i) {
        const int dayOfWeek = ((static_cast<int>(m_firstDayOfWeek) - 1 + i) % s_daysInWeek) + 1;
        QLabel *headerLabel = new QLabel(weekdaysNames[i], this);
        headerLabel->setText(weekdaysNames.at(dayOfWeek));
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
            connect(cell, &CalendarCell::todosEdited,
                    this, &CalendarView::onCellTodosEdited);
            connect(cell, &CalendarCell::todoCompletionChanged,
                    this, &CalendarView::onTodoCompletionChanged);
            m_gridLayout->addWidget(cell, row, col);
        }
    }

    setCurrentMonth(m_year, m_month, false);
}

void CalendarView::setCurrentMonth(int year, int month, bool animated) {
    if (animated && year == m_year && month == m_month && !m_isFadingOut) {
        return;
    }

    if (!animated || !isVisible()) {
        m_transitionAnimation->stop();
        m_opacityEffect->setOpacity(1.0);
        m_targetYear = year;
        m_targetMonth = month;
        m_isFadingOut = false;
        applyMonth(year, month);
        return;
    }

    m_targetYear = year;
    m_targetMonth = month;
    m_isFadingOut = true;

    m_transitionAnimation->stop();
    m_transitionAnimation->setDuration(140);
    m_transitionAnimation->setStartValue(m_opacityEffect->opacity());
    m_transitionAnimation->setEndValue(0.15);
    m_transitionAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    m_transitionAnimation->start();
}

void CalendarView::applyMonth(int year, int month) {
    m_year = year;
    m_month = month;

    QDate firstDayOfMonth(year, month, 1);
    int firstDayWeekday = firstDayOfMonth.dayOfWeek();
    int startOffset = (7 + firstDayWeekday - static_cast<int>(m_firstDayOfWeek)) % 7;

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
                cell->setTodos(todosByDate.value(currentDate));

                m_cells[currentDate] = cell;
            }

            currentDate = currentDate.addDays(1);
        }
    }
}

void CalendarView::refreshCells() {
    setCurrentMonth(m_year, m_month, false);
}

void CalendarView::editDate(const QDate &date) {
    CalendarCell *cell = m_cells.value(date, nullptr);
    if (cell) {
        cell->beginInlineEdit();
    }
}

void CalendarView::onCellDoubleClicked(const QDate &date) {
    if (date.isValid()) {
        emit dateDoubleClicked(date);
    }
}

void CalendarView::onCellTodosEdited(const QDate &date, const QList<TodoItem> &todos) {
    if (!date.isValid()) {
        return;
    }

    TodoManager::instance().replaceTodosByDate(date, todos);
    refreshCells();
}

void CalendarView::onTodoCompletionChanged(int todoId, bool completed) {
    Q_UNUSED(completed);
    if (todoId == 0) {
        return;
    }

    TodoManager::instance().toggleCompleted(todoId);
    refreshCells();
}

void CalendarView::onTransitionFinished() {
    if (m_isFadingOut) {
        applyMonth(m_targetYear, m_targetMonth);
        m_isFadingOut = false;

        m_transitionAnimation->setDuration(180);
        m_transitionAnimation->setStartValue(m_opacityEffect->opacity());
        m_transitionAnimation->setEndValue(1.0);
        m_transitionAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_transitionAnimation->start();
    }
}
