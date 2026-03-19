#include "databasemanager.h"

#include <QApplication>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

DatabaseManager::DatabaseManager()
    : m_database(QSqlDatabase::addDatabase("QSQLITE"))
{
}

DatabaseManager::~DatabaseManager() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

QString DatabaseManager::getDatabasePath() const {
    QString configPath = QDir::homePath() + "/.config/calendar_todo";
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(configPath);
    }
    return configPath + "/data.db";
}

bool DatabaseManager::initDatabase() {
    m_database.setDatabaseName(getDatabasePath());

    if (!m_database.open()) {
        qWarning("Cannot open database: %s", qPrintable(m_database.lastError().text()));
        return false;
    }

    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query;

    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS todos ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    date TEXT NOT NULL,"
        "    title TEXT NOT NULL,"
        "    description TEXT,"
        "    completed INTEGER DEFAULT 0,"
        "    created_at TEXT NOT NULL"
        ")"
    );

    if (!success) {
        qWarning("Failed to create todos table: %s", qPrintable(query.lastError().text()));
        return false;
    }

    success = query.exec(
        "CREATE TABLE IF NOT EXISTS settings ("
        "    key TEXT PRIMARY KEY,"
        "    value TEXT NOT NULL"
        ")"
    );

    if (!success) {
        qWarning("Failed to create settings table: %s", qPrintable(query.lastError().text()));
        return false;
    }

    return true;
}

bool DatabaseManager::addTodo(TodoItem &todo) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO todos (date, title, description, completed, created_at) "
        "VALUES (:date, :title, :description, :completed, :created_at)"
    );
    query.bindValue(":date", todo.date.toString(Qt::ISODate));
    query.bindValue(":title", todo.title);
    query.bindValue(":description", todo.description);
    query.bindValue(":completed", todo.completed ? 1 : 0);
    query.bindValue(":created_at", todo.createdAt.toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning("Failed to add todo: %s", qPrintable(query.lastError().text()));
        return false;
    }

    todo.id = query.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::updateTodo(const TodoItem &todo) {
    QSqlQuery query;
    query.prepare(
        "UPDATE todos SET date = :date, title = :title, "
        "description = :description, completed = :completed "
        "WHERE id = :id"
    );
    query.bindValue(":id", todo.id);
    query.bindValue(":date", todo.date.toString(Qt::ISODate));
    query.bindValue(":title", todo.title);
    query.bindValue(":description", todo.description);
    query.bindValue(":completed", todo.completed ? 1 : 0);

    if (!query.exec()) {
        qWarning("Failed to update todo: %s", qPrintable(query.lastError().text()));
        return false;
    }

    return true;
}

bool DatabaseManager::deleteTodo(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM todos WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning("Failed to delete todo: %s", qPrintable(query.lastError().text()));
        return false;
    }

    return true;
}

TodoItem DatabaseManager::getTodoById(int id) const {
    QSqlQuery query;
    query.prepare("SELECT id, date, title, description, completed, created_at FROM todos WHERE id = :id");
    query.bindValue(":id", id);

    TodoItem todo;
    if (query.exec() && query.next()) {
        todo.id = query.value(0).toInt();
        todo.date = QDate::fromString(query.value(1).toString(), Qt::ISODate);
        todo.title = query.value(2).toString();
        todo.description = query.value(3).toString();
        todo.completed = query.value(4).toInt() == 1;
        todo.createdAt = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
    }

    return todo;
}

QList<TodoItem> DatabaseManager::getTodosByDate(const QDate &date) const {
    QSqlQuery query;
    query.prepare("SELECT id, date, title, description, completed, created_at FROM todos WHERE date = :date ORDER BY created_at");
    query.bindValue(":date", date.toString(Qt::ISODate));

    QList<TodoItem> todos;
    if (query.exec()) {
        while (query.next()) {
            TodoItem todo;
            todo.id = query.value(0).toInt();
            todo.date = QDate::fromString(query.value(1).toString(), Qt::ISODate);
            todo.title = query.value(2).toString();
            todo.description = query.value(3).toString();
            todo.completed = query.value(4).toInt() == 1;
            todo.createdAt = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
            todos.append(todo);
        }
    }

    return todos;
}

QList<TodoItem> DatabaseManager::getTodosByMonth(int year, int month) const {
    QSqlQuery query;
    QDate startDate(year, month, 1);
    QDate endDate = startDate.addMonths(1).addDays(-1);
    QString startDateStr = startDate.toString(Qt::ISODate);
    QString endDateStr = endDate.toString(Qt::ISODate);

    query.prepare(
        "SELECT id, date, title, description, completed, created_at "
        "FROM todos "
        "WHERE date >= :start_date AND date <= :end_date "
        "ORDER BY date, created_at"
    );
    query.bindValue(":start_date", startDateStr);
    query.bindValue(":end_date", endDateStr);

    QList<TodoItem> todos;
    if (query.exec()) {
        while (query.next()) {
            TodoItem todo;
            todo.id = query.value(0).toInt();
            todo.date = QDate::fromString(query.value(1).toString(), Qt::ISODate);
            todo.title = query.value(2).toString();
            todo.description = query.value(3).toString();
            todo.completed = query.value(4).toInt() == 1;
            todo.createdAt = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
            todos.append(todo);
        }
    }

    return todos;
}

QString DatabaseManager::getSetting(const QString &key, const QString &defaultValue) const {
    QSqlQuery query;
    query.prepare("SELECT value FROM settings WHERE key = :key");
    query.bindValue(":key", key);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return defaultValue;
}

bool DatabaseManager::setSetting(const QString &key, const QString &value) {
    QSqlQuery query;
    query.prepare(
        "INSERT OR REPLACE INTO settings (key, value) VALUES (:key, :value)"
    );
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    if (!query.exec()) {
        qWarning("Failed to save setting: %s", qPrintable(query.lastError().text()));
        return false;
    }

    return true;
}
