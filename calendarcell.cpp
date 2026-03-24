#include "calendarcell.h"

#include <QApplication>
#include <QCheckBox>
#include <QEvent>
#include <QFont>
#include <QKeyEvent>
#include <QLineEdit>
#include <QLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QDebug>

CalendarCell::CalendarCell(QWidget *parent)
    : QWidget(parent)
    , m_isCurrentMonth(false)
    , m_isEditing(false)
    , m_layout(nullptr)
    , m_dateLabel(nullptr)
    , m_todosContainer(nullptr)
    , m_editorContainer(nullptr)
    , m_editorLayout(nullptr)
    , m_editorActions(nullptr)
    , m_saveButton(nullptr)
    , m_cancelButton(nullptr)
{
    setMinimumSize(80, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(1);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_dateLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_layout->addWidget(m_dateLabel);

    m_todosContainer = new QWidget(this);
    QVBoxLayout *todosLayout = new QVBoxLayout(m_todosContainer);
    todosLayout->setContentsMargins(0, 0, 0, 0);
    todosLayout->setSpacing(2);
    m_layout->addWidget(m_todosContainer);

    m_editorContainer = new QWidget(this);
    m_editorContainer->setVisible(false);
    m_editorLayout = new QVBoxLayout(m_editorContainer);
    m_editorLayout->setContentsMargins(0, 0, 0, 0);
    m_editorLayout->setSpacing(2);
    m_layout->addWidget(m_editorContainer);

    m_editorActions = new QWidget(this);
    m_editorActions->setVisible(false);
    QHBoxLayout *editorLayout = new QHBoxLayout(m_editorActions);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(4);
    editorLayout->addStretch();

    m_saveButton = new QPushButton("保存", m_editorActions);
    m_saveButton->setFixedHeight(22);
    connect(m_saveButton, &QPushButton::clicked, this, [this]() { finishInlineEdit(true); });
    editorLayout->addWidget(m_saveButton);

    m_cancelButton = new QPushButton("取消", m_editorActions);
    m_cancelButton->setFixedHeight(22);
    connect(m_cancelButton, &QPushButton::clicked, this, [this]() { finishInlineEdit(false); });
    editorLayout->addWidget(m_cancelButton);

    m_layout->addWidget(m_editorActions);

    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.1); "
        "   border: 1px solid rgba(200, 200, 200, 0.2); "
        "   border-radius: 4px; "
        "}"
        "CalendarCell:hover { "
        "   background-color: rgba(255, 255, 255, 0.2); "
        "}"
    );
}

void CalendarCell::setIsCurrentMonth(bool value) {
    if (m_isCurrentMonth == value) {
        return;
    }

    m_isCurrentMonth = value;
    updateDisplay();
}

void CalendarCell::setDate(const QDate &date) {
    m_date = date;
    updateDisplay();
}

void CalendarCell::setTodos(const QList<TodoItem> &todos) {
    m_todos = todos;
    updateDisplay();
}

void CalendarCell::addTodo(const TodoItem &todo) {
    m_todos.append(todo);
    updateDisplay();
}

void CalendarCell::clearTodos() {
    m_todos.clear();
    updateDisplay();
}

void CalendarCell::beginInlineEdit() {
    if (!m_date.isValid() || m_isEditing) {
        return;
    }

    m_isEditing = true;

    clearLayout(m_editorLayout);
    m_editorChecks.clear();
    m_editorEdits.clear();
    for (const TodoItem &todo : m_todos) {
        addEditorRow(todo);
    }
    addEditorRow(TodoItem(), true);

    m_todosContainer->setVisible(false);
    m_editorContainer->setVisible(true);
    m_editorActions->setVisible(true);
    qApp->installEventFilter(this);

    if (!m_editorEdits.isEmpty()) {
        m_editorEdits.first()->setFocus();
        m_editorEdits.first()->selectAll();
    }
}

void CalendarCell::cancelInlineEdit() {
    finishInlineEdit(false);
}

bool CalendarCell::eventFilter(QObject *watched, QEvent *event) {
    if (m_isEditing && watched == qApp && event->type() == QEvent::MouseButtonPress) {
        QWidget *targetWidget = qobject_cast<QWidget*>(qApp->widgetAt(QCursor::pos()));
        if (!targetWidget || (targetWidget != this && !isAncestorOf(targetWidget))) {
            finishInlineEdit(true);
            return false;
        }
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
                && (keyEvent->modifiers() & Qt::ControlModifier)) {
            finishInlineEdit(true);
            return true;
        }

        if (keyEvent->key() == Qt::Key_Escape) {
            finishInlineEdit(false);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void CalendarCell::addEditorRow(const TodoItem &todo, bool blankRow) {
    QWidget *rowWidget = new QWidget(m_editorContainer);
    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(4);

    QCheckBox *checkBox = new QCheckBox(rowWidget);
    checkBox->setChecked(todo.completed);
    rowLayout->addWidget(checkBox);

    QLineEdit *lineEdit = new QLineEdit(rowWidget);
    lineEdit->setPlaceholderText(blankRow ? "新增待办" : "");
    lineEdit->setText(todo.title);
    lineEdit->installEventFilter(this);
    rowLayout->addWidget(lineEdit);

    connect(checkBox, &QCheckBox::toggled, this, [this, lineEdit](bool checked) {
        QFont font = lineEdit->font();
        font.setStrikeOut(checked);
        lineEdit->setFont(font);
    });
    connect(lineEdit, &QLineEdit::textChanged, this, [this](const QString &) {
        ensureTrailingEmptyEditorRow();
    });

    QFont font = lineEdit->font();
    font.setStrikeOut(todo.completed);
    lineEdit->setFont(font);

    m_editorChecks.append(checkBox);
    m_editorEdits.append(lineEdit);
    m_editorLayout->addWidget(rowWidget);
}

void CalendarCell::clearLayout(QLayout *layout) {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->layout()) {
            clearLayout(child->layout());
        }
        delete child->widget();
        delete child;
    }
}

QList<TodoItem> CalendarCell::collectEditedTodos() const {
    QList<TodoItem> todos;
    for (int index = 0; index < m_editorEdits.size(); ++index) {
        const QString title = m_editorEdits.at(index)->text().trimmed();
        if (title.isEmpty()) {
            continue;
        }

        TodoItem todo;
        todo.date = m_date;
        todo.title = title;
        todo.completed = m_editorChecks.at(index)->isChecked();
        todos.append(todo);
    }

    return todos;
}

void CalendarCell::ensureTrailingEmptyEditorRow() {
    if (!m_isEditing || m_editorEdits.isEmpty()) {
        return;
    }

    if (!m_editorEdits.last()->text().trimmed().isEmpty()) {
        addEditorRow(TodoItem(), true);
    }
}

void CalendarCell::finishInlineEdit(bool accepted) {
    if (!m_isEditing) {
        return;
    }

    qApp->removeEventFilter(this);
    m_isEditing = false;

    if (accepted) {
        emit todosEdited(m_date, collectEditedTodos());
    }

    clearLayout(m_editorLayout);
    m_editorChecks.clear();
    m_editorEdits.clear();
    m_editorContainer->setVisible(false);
    m_editorActions->setVisible(false);
    m_todosContainer->setVisible(true);
    updateDisplay();
}

void CalendarCell::updateDisplay() {
    if (!m_date.isValid()) {
        m_dateLabel->setText("");
        QLayoutItem *child;
        while ((child = m_todosContainer->layout()->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        return;
    }

    m_dateLabel->setText(QString::number(m_date.day()));

    if (m_date == QDate::currentDate()) {
        m_dateLabel->setStyleSheet("color: #4CAF50; font-weight: bold;");
    } else if (!m_isCurrentMonth) {
        m_dateLabel->setStyleSheet("color: rgba(150, 150, 150, 0.5);");
    } else {
        m_dateLabel->setStyleSheet("color: white;");
    }

    if (m_isEditing) {
        return;
    }

    QLayoutItem *child;
    while ((child = m_todosContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QVBoxLayout *todosLayout = qobject_cast<QVBoxLayout*>(m_todosContainer->layout());
    for (const TodoItem &todo : m_todos) {
        QCheckBox *todoCheckBox = new QCheckBox(todo.title, m_todosContainer);
        todoCheckBox->setChecked(todo.completed);
        todoCheckBox->setStyleSheet(
            "QCheckBox { "
            "   color: white; "
            "   font-size: 11px; "
            "   spacing: 4px; "
            "}"
        );

        QFont font = todoCheckBox->font();
        font.setStrikeOut(todo.completed);
        todoCheckBox->setFont(font);

        connect(todoCheckBox, &QCheckBox::toggled, this, [this, todo](bool checked) {
            emit todoCompletionChanged(todo.id, checked);
        });
        todosLayout->addWidget(todoCheckBox);
    }

    todosLayout->addStretch();
}

QString CalendarCell::formatTodoText(const TodoItem &todo) {
    if (todo.completed) {
        return QString("<s>%1</s>").arg(todo.title);
    }
    return todo.title;
}

void CalendarCell::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_date.isValid() && !m_isEditing) {
        emit doubleClicked(m_date);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CalendarCell::enterEvent(QEvent *event) {
    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.25); "
        "   border: 1px solid rgba(200, 200, 200, 0.4); "
        "   border-radius: 4px; "
        "}"
    );
    QWidget::enterEvent(event);
}

void CalendarCell::leaveEvent(QEvent *event) {
    setStyleSheet(
        "CalendarCell { "
        "   background-color: rgba(255, 255, 255, 0.1); "
        "   border: 1px solid rgba(200, 200, 200, 0.2); "
        "   border-radius: 4px; "
        "}"
    );
    QWidget::leaveEvent(event);
}
