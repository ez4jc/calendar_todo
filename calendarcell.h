#ifndef CALENDARCELL_H
#define CALENDARCELL_H

#include <QDate>
#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include "todoitem.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class CalendarCell : public QWidget {
    Q_OBJECT

public:
    explicit CalendarCell(QWidget *parent = nullptr);
    void setDate(const QDate &date);
    QDate date() const { return m_date; }
    void setTodos(const QList<TodoItem> &todos);
    void addTodo(const TodoItem &todo);
    void clearTodos();
    bool isCurrentMonth() const { return m_isCurrentMonth; }
    void setIsCurrentMonth(bool value);
    void beginInlineEdit();
    void cancelInlineEdit();
    bool isEditing() const { return m_isEditing; }

signals:
    void doubleClicked(const QDate &date);
    void todosEdited(const QDate &date, const QList<TodoItem> &todos);
    void todoCompletionChanged(int todoId, bool completed);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void addEditorRow(const TodoItem &todo = TodoItem(), bool blankRow = false);
    void clearLayout(QLayout *layout);
    QList<TodoItem> collectEditedTodos() const;
    void finishInlineEdit(bool accepted);
    void updateDisplay();
    QString formatTodoText(const TodoItem &todo);

    QDate m_date;
    bool m_isCurrentMonth;
    bool m_isEditing;
    QList<TodoItem> m_todos;
    QVBoxLayout *m_layout;
    QLabel *m_dateLabel;
    QWidget *m_todosContainer;
    QWidget *m_editorContainer;
    QVBoxLayout *m_editorLayout;
    QWidget *m_editorActions;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QList<QCheckBox*> m_editorChecks;
    QList<QLineEdit*> m_editorEdits;
};

#endif // CALENDARCELL_H
