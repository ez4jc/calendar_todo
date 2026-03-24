#include "todomanager.h"
#include "databasemanager.h"

TodoManager::TodoManager() {
}

TodoManager::~TodoManager() {
}

TodoManager& TodoManager::instance() {
    static TodoManager instance;
    return instance;
}

QList<TodoItem> TodoManager::getTodosByDate(const QDate &date) {
    return DatabaseManager::instance().getTodosByDate(date);
}

QList<TodoItem> TodoManager::getTodosByMonth(int year, int month) {
    return DatabaseManager::instance().getTodosByMonth(year, month);
}

bool TodoManager::addTodo(TodoItem &todo) {
    return DatabaseManager::instance().addTodo(todo);
}

bool TodoManager::updateTodo(const TodoItem &todo) {
    return DatabaseManager::instance().updateTodo(todo);
}

bool TodoManager::deleteTodo(int id) {
    return DatabaseManager::instance().deleteTodo(id);
}

bool TodoManager::toggleCompleted(int id) {
    TodoItem todo = DatabaseManager::instance().getTodoById(id);
    if (todo.id == 0) {
        return false;
    }
    todo.completed = !todo.completed;
    return DatabaseManager::instance().updateTodo(todo);
}

bool TodoManager::replaceTodosByDate(const QDate &date, const QList<TodoItem> &todos) {
    QList<TodoItem> existingTodos = DatabaseManager::instance().getTodosByDate(date);
    QList<TodoItem> normalizedTodos;
    for (TodoItem todo : todos) {
        todo.date = date;
        todo.title = todo.title.trimmed();
        if (!todo.title.isEmpty()) {
            normalizedTodos.append(todo);
        }
    }

    const int sharedCount = qMin(existingTodos.size(), normalizedTodos.size());
    for (int index = 0; index < sharedCount; ++index) {
        TodoItem todo = existingTodos.at(index);
        const TodoItem &updated = normalizedTodos.at(index);
        if (todo.title != updated.title || todo.completed != updated.completed) {
            todo.title = updated.title;
            todo.completed = updated.completed;
            if (!DatabaseManager::instance().updateTodo(todo)) {
                return false;
            }
        }
    }

    for (int index = normalizedTodos.size(); index < existingTodos.size(); ++index) {
        if (!DatabaseManager::instance().deleteTodo(existingTodos.at(index).id)) {
            return false;
        }
    }

    for (int index = existingTodos.size(); index < normalizedTodos.size(); ++index) {
        TodoItem todo = normalizedTodos.at(index);
        if (!todo.createdAt.isValid()) {
            todo.createdAt = QDateTime::currentDateTime();
        }
        if (!DatabaseManager::instance().addTodo(todo)) {
            return false;
        }
    }

    return true;
}
