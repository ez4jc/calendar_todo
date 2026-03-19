#ifndef CALENDARCELL_H
#define CALENDARCELL_H

#include <QDate>
#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include "todoitem.h"

class CalendarCell : public QWidget {
    Q_OBJECT

public:
    explicit CalendarCell(QWidget *parent = nullptr);
    void setDate(const QDate &date);
    QDate date() const { return m_date; }
    void addTodo(const TodoItem &todo);
    void clearTodos();
    bool isCurrentMonth() const { return m_isCurrentMonth; }
    void setIsCurrentMonth(bool value) { m_isCurrentMonth = value; }

signals:
    void doubleClicked(const QDate &date);
    void todoClicked(int todoId);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateDisplay();
    QString formatTodoText(const TodoItem &todo);

    QDate m_date;
    bool m_isCurrentMonth;
    QList<TodoItem> m_todos;
    QVBoxLayout *m_layout;
    QLabel *m_dateLabel;
    QWidget *m_todosContainer;
};

#endif // CALENDARCELL_H
