#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QDate>
#include <QList>
#include <QString>
#include <QSqlDatabase>
#include "todoitem.h"

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initDatabase();
    bool addTodo(TodoItem &todo);
    bool updateTodo(const TodoItem &todo);
    bool deleteTodo(int id);
    TodoItem getTodoById(int id) const;
    QList<TodoItem> getTodosByDate(const QDate &date) const;
    QList<TodoItem> getTodosByMonth(int year, int month) const;

    QString getSetting(const QString &key, const QString &defaultValue = QString()) const;
    bool setSetting(const QString &key, const QString &value);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    QString getDatabasePath() const;

    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
