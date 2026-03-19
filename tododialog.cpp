#include "tododialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include "todomanager.h"

TodoDialog::TodoDialog(const QDate &date, QWidget *parent)
    : QDialog(parent)
    , m_date(date)
    , m_mode(AddMode)
    , m_mainLayout(nullptr)
    , m_dateLabel(nullptr)
    , m_titleEdit(nullptr)
    , m_descriptionEdit(nullptr)
    , m_todoListWidget(nullptr)
    , m_saveButton(nullptr)
    , m_deleteButton(nullptr)
    , m_cancelButton(nullptr)
{
    setWindowTitle("待办事项 - " + date.toString(Qt::ISODate));
    setMinimumSize(400, 350);
    setModal(true);

    setupUI();
    loadTodosForDate();
}

void TodoDialog::setupUI() {
    m_mainLayout = new QVBoxLayout(this);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setText(m_date.toString("yyyy年MM月dd日 ddd"));
    m_mainLayout->addWidget(m_dateLabel);

    m_todoListWidget = new QListWidget(this);
    m_mainLayout->addWidget(m_todoListWidget);
    connect(m_todoListWidget, &QListWidget::itemClicked,
            this, &TodoDialog::onItemClicked);

    QLabel *titleLabel = new QLabel("添加新待办:", this);
    m_mainLayout->addWidget(titleLabel);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("待办标题");
    m_mainLayout->addWidget(m_titleEdit);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("描述 (可选)");
    m_descriptionEdit->setMaximumHeight(60);
    m_mainLayout->addWidget(m_descriptionEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_saveButton = new QPushButton("保存", this);
    connect(m_saveButton, &QPushButton::clicked, this, &TodoDialog::onSave);
    buttonLayout->addWidget(m_saveButton);

    m_deleteButton = new QPushButton("删除", this);
    m_deleteButton->setVisible(false);
    connect(m_deleteButton, &QPushButton::clicked, this, &TodoDialog::onDelete);
    buttonLayout->addWidget(m_deleteButton);

    m_cancelButton = new QPushButton("关闭", this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelButton);

    m_mainLayout->addLayout(buttonLayout);

    setStyleSheet(
        "QDialog { "
        "   background-color: rgba(50, 50, 50, 0.95); "
        "   color: white; "
        "}"
        "QLabel { "
        "   color: white; "
        "}"
        "QLineEdit, QTextEdit { "
        "   background-color: rgba(80, 80, 80, 0.9); "
        "   color: white; "
        "   border: 1px solid rgba(100, 100, 100, 0.5); "
        "   border-radius: 4px; "
        "   padding: 4px; "
        "}"
        "QPushButton { "
        "   background-color: rgba(70, 70, 70, 0.9); "
        "   color: white; "
        "   border: 1px solid rgba(100, 100, 100, 0.5); "
        "   border-radius: 4px; "
        "   padding: 6px 16px; "
        "   min-width: 70px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(90, 90, 90, 0.9); "
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(60, 60, 60, 0.9); "
        "}"
        "QListWidget { "
        "   background-color: rgba(60, 60, 60, 0.9); "
        "   color: white; "
        "   border: 1px solid rgba(100, 100, 100, 0.5); "
        "   border-radius: 4px; "
        "}"
        "QListWidget::item { "
        "   padding: 4px; "
        "   border-bottom: 1px solid rgba(100, 100, 100, 0.3); "
        "}"
        "QListWidget::item:selected { "
        "   background-color: rgba(70, 130, 180, 0.5); "
        "}"
    );
}

void TodoDialog::setTodo(const TodoItem &todo) {
    m_currentTodo = todo;
    m_titleEdit->setText(todo.title);
    m_descriptionEdit->setText(todo.description);
    m_deleteButton->setVisible(todo.id != 0);
}

void TodoDialog::setMode(Mode mode) {
    m_mode = mode;
    switch (mode) {
        case AddMode:
            m_titleEdit->setReadOnly(false);
            m_descriptionEdit->setReadOnly(false);
            m_saveButton->setVisible(true);
            m_deleteButton->setVisible(false);
            break;
        case EditMode:
            m_titleEdit->setReadOnly(false);
            m_descriptionEdit->setReadOnly(false);
            m_saveButton->setVisible(true);
            m_deleteButton->setVisible(true);
            break;
        case ViewMode:
            m_titleEdit->setReadOnly(true);
            m_descriptionEdit->setReadOnly(true);
            m_saveButton->setVisible(false);
            m_deleteButton->setVisible(false);
            break;
    }
}

TodoItem TodoDialog::getTodo() const {
    TodoItem todo;
    todo.date = m_date;
    todo.title = m_titleEdit->text().trimmed();
    todo.description = m_descriptionEdit->toPlainText().trimmed();
    todo.completed = m_currentTodo.completed;
    todo.id = m_currentTodo.id;
    todo.createdAt = m_currentTodo.createdAt;
    return todo;
}

TodoItem TodoDialog::getNewTodo(const QDate &date, QWidget *parent) {
    TodoDialog dialog(date, parent);
    dialog.setMode(AddMode);

    TodoItem result;
    if (dialog.exec() == QDialog::Accepted) {
        result = dialog.getTodo();
    }
    return result;
}

bool TodoDialog::editTodo(TodoItem &todo, QWidget *parent) {
    TodoDialog dialog(todo.date, parent);
    dialog.setMode(EditMode);
    dialog.setTodo(todo);

    if (dialog.exec() == QDialog::Accepted) {
        todo = dialog.getTodo();
        return true;
    }
    return false;
}

void TodoDialog::loadTodosForDate() {
    updateTodoList();
}

void TodoDialog::updateTodoList() {
    m_todoListWidget->clear();

    QList<TodoItem> todos = TodoManager::instance().getTodosByDate(m_date);
    for (const TodoItem &todo : todos) {
        QString displayText = todo.completed
            ? QString("☑ %1").arg(todo.title)
            : QString("☐ %1").arg(todo.title);

        QListWidgetItem *item = new QListWidgetItem(displayText, m_todoListWidget);
        item->setData(Qt::UserRole, todo.id);

        if (todo.completed) {
            QFont font = item->font();
            font.setStrikeOut(true);
            item->setFont(font);
            item->setForeground(Qt::gray);
        }

        m_todoListWidget->addItem(item);
    }
}

void TodoDialog::onSave() {
    QString title = m_titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入待办标题");
        return;
    }

    TodoItem todo = getTodo();
    todo.createdAt = QDateTime::currentDateTime();

    if (todo.id == 0) {
        if (TodoManager::instance().addTodo(todo)) {
            emit todoSaved(todo);
            m_titleEdit->clear();
            m_descriptionEdit->clear();
            updateTodoList();
        }
    } else {
        if (TodoManager::instance().updateTodo(todo)) {
            emit todoSaved(todo);
            accept();
        }
    }
}

void TodoDialog::onDelete() {
    if (m_currentTodo.id == 0) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        "确定要删除这个待办事项吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (TodoManager::instance().deleteTodo(m_currentTodo.id)) {
            emit todoDeleted(m_currentTodo.id);
            accept();
        }
    }
}

void TodoDialog::onItemClicked(QListWidgetItem *item) {
    int todoId = item->data(Qt::UserRole).toInt();
    if (todoId == 0) {
        return;
    }

    QList<TodoItem> todos = TodoManager::instance().getTodosByDate(m_date);
    for (const TodoItem &todo : todos) {
        if (todo.id == todoId) {
            TodoItem editedTodo = todo;

            QStringList options;
            options << "切换完成状态" << "编辑" << "删除" << "取消";
            QString choice = QInputDialog::getItem(this, "选择操作", "", options, 0, false);

            if (choice == "切换完成状态") {
                TodoManager::instance().toggleCompleted(todoId);
                updateTodoList();
            } else if (choice == "编辑") {
                if (editTodo(editedTodo, this)) {
                    TodoManager::instance().updateTodo(editedTodo);
                    updateTodoList();
                }
            } else if (choice == "删除") {
                TodoManager::instance().deleteTodo(todoId);
                updateTodoList();
            }
            break;
        }
    }
}
