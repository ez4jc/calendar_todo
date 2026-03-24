#ifndef TODOMANAGER_H
#define TODOMANAGER_H

#include <QDate>
#include <QList>
#include "todoitem.h"

class TodoManager {
public:
    static TodoManager& instance();

    QList<TodoItem> getTodosByDate(const QDate &date);
    QList<TodoItem> getTodosByMonth(int year, int month);
    bool addTodo(TodoItem &todo);
    bool updateTodo(const TodoItem &todo);
    bool deleteTodo(int id);
    bool toggleCompleted(int id);
    bool replaceTodosByDate(const QDate &date, const QList<TodoItem> &todos);

private:
    TodoManager();
    ~TodoManager();
    TodoManager(const TodoManager&) = delete;
    TodoManager& operator=(const TodoManager&) = delete;
};

#endif // TODOMANAGER_H
