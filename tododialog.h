#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include <QDate>
#include "todoitem.h"

class QLineEdit;
class QTextEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QVBoxLayout;
class QLabel;

class TodoDialog : public QDialog {
    Q_OBJECT

public:
    enum Mode { AddMode, EditMode, ViewMode };

    explicit TodoDialog(const QDate &date, QWidget *parent = nullptr);
    void setTodo(const TodoItem &todo);
    void setMode(Mode mode);
    TodoItem getTodo() const;

    static TodoItem getNewTodo(const QDate &date, QWidget *parent = nullptr);
    static bool editTodo(TodoItem &todo, QWidget *parent = nullptr);

signals:
    void todoSaved(const TodoItem &todo);
    void todoDeleted(int id);

private slots:
    void onSave();
    void onDelete();
    void onItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void loadTodosForDate();
    void updateTodoList();

    QDate m_date;
    Mode m_mode;
    TodoItem m_currentTodo;

    QVBoxLayout *m_mainLayout;
    QLabel *m_dateLabel;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QListWidget *m_todoListWidget;
    QPushButton *m_saveButton;
    QPushButton *m_deleteButton;
    QPushButton *m_cancelButton;
};

#endif // TODODIALOG_H
