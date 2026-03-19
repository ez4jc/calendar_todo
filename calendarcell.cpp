#include "calendarcell.h"

#include <QMouseEvent>
#include <QDebug>

CalendarCell::CalendarCell(QWidget *parent)
    : QWidget(parent)
    , m_isCurrentMonth(false)
    , m_layout(nullptr)
    , m_dateLabel(nullptr)
    , m_todosContainer(nullptr)
{
    setMinimumSize(80, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(1);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_layout->addWidget(m_dateLabel);

    m_todosContainer = new QWidget(this);
    QVBoxLayout *todosLayout = new QVBoxLayout(m_todosContainer);
    todosLayout->setContentsMargins(0, 0, 0, 0);
    todosLayout->setSpacing(0);
    m_layout->addWidget(m_todosContainer);

    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.1); "
        "   border: 1px solid rgba(200, 200, 200, 0.2); "
        "   border-radius: 4px; "
        "}"
        "CalendarCell:hover { "
        "   background-color: rgba(255, 255, 255, 0.2); "
        "}"
    );
}

void CalendarCell::setDate(const QDate &date) {
    m_date = date;
    updateDisplay();
}

void CalendarCell::addTodo(const TodoItem &todo) {
    m_todos.append(todo);
    updateDisplay();
}

void CalendarCell::clearTodos() {
    m_todos.clear();
    updateDisplay();
}

void CalendarCell::updateDisplay() {
    if (!m_date.isValid()) {
        m_dateLabel->setText("");
        QLayoutItem *child;
        while ((child = m_todosContainer->layout()->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        return;
    }

    if (m_date.day() == 1 || m_dateLabel->text().isEmpty()) {
        m_dateLabel->setText(QString::number(m_date.day()));
    }

    if (m_date == QDate::currentDate()) {
        m_dateLabel->setStyleSheet("color: #4CAF50; font-weight: bold;");
    } else if (!m_isCurrentMonth) {
        m_dateLabel->setStyleSheet("color: rgba(150, 150, 150, 0.5);");
    } else {
        m_dateLabel->setStyleSheet("color: white;");
    }

    QLayoutItem *child;
    while ((child = m_todosContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QVBoxLayout *todosLayout = qobject_cast<QVBoxLayout*>(m_todosContainer->layout());
    for (const TodoItem &todo : m_todos) {
        QLabel *todoLabel = new QLabel(m_todosContainer);
        todoLabel->setText(formatTodoText(todo));
        todoLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        todoLabel->setWordWrap(true);
        todoLabel->setStyleSheet(
            "QLabel { "
            "   color: white; "
            "   font-size: 11px; "
            "   padding: 1px 2px; "
            "   background-color: transparent; "
            "}"
        );
        todoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        todosLayout->addWidget(todoLabel);
    }

    todosLayout->addStretch();
}

QString CalendarCell::formatTodoText(const TodoItem &todo) {
    QString checkbox = todo.completed ? "☑ " : "☐ ";
    if (todo.completed) {
        return QString("<s>%1%2</s>").arg(checkbox).arg(todo.title);
    }
    return checkbox + todo.title;
}

void CalendarCell::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_date.isValid()) {
        emit doubleClicked(m_date);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CalendarCell::enterEvent(QEvent *event) {
    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.25); "
        "   border: 1px solid rgba(200, 200, 200, 0.4); "
        "   border-radius: 4px; "
        "}"
    );
    QWidget::enterEvent(event);
}

void CalendarCell::leaveEvent(QEvent *event) {
    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.1); "
        "   border: 1px solid rgba(200, 200, 200, 0.2); "
        "   border-radius: 4px; "
        "}"
    );
    QWidget::leaveEvent(event);
}
