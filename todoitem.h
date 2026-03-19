#ifndef TODOITEM_H
#define TODOITEM_H

#include <QDate>
#include <QDateTime>
#include <QString>

struct TodoItem {
    int id;
    QDate date;
    QString title;
    QString description;
    bool completed;
    QDateTime createdAt;

    TodoItem()
        : id(0)
        , completed(false)
    {}

    TodoItem(const QDate &d, const QString &t, const QString &desc = QString())
        : id(0)
        , date(d)
        , title(t)
        , description(desc)
        , completed(false)
        , createdAt(QDateTime::currentDateTime())
    {}
};

#endif // TODOITEM_H
