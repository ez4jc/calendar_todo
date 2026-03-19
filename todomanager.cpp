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
