/****************************************************************************
** Meta object code from reading C++ file 'calendarview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "calendarview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calendarview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CalendarView_t {
    QByteArrayData data[12];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalendarView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalendarView_t qt_meta_stringdata_CalendarView = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CalendarView"
QT_MOC_LITERAL(1, 13, 17), // "dateDoubleClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "date"
QT_MOC_LITERAL(4, 37, 19), // "onCellDoubleClicked"
QT_MOC_LITERAL(5, 57, 17), // "onCellTodosEdited"
QT_MOC_LITERAL(6, 75, 15), // "QList<TodoItem>"
QT_MOC_LITERAL(7, 91, 5), // "todos"
QT_MOC_LITERAL(8, 97, 23), // "onTodoCompletionChanged"
QT_MOC_LITERAL(9, 121, 6), // "todoId"
QT_MOC_LITERAL(10, 128, 9), // "completed"
QT_MOC_LITERAL(11, 138, 20) // "onTransitionFinished"

    },
    "CalendarView\0dateDoubleClicked\0\0date\0"
    "onCellDoubleClicked\0onCellTodosEdited\0"
    "QList<TodoItem>\0todos\0onTodoCompletionChanged\0"
    "todoId\0completed\0onTransitionFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalendarView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   42,    2, 0x08 /* Private */,
       5,    2,   45,    2, 0x08 /* Private */,
       8,    2,   50,    2, 0x08 /* Private */,
      11,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QDate,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QDate,    3,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 6,    3,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    9,   10,
    QMetaType::Void,

       0        // eod
};

void CalendarView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CalendarView *_t = static_cast<CalendarView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dateDoubleClicked((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: _t->onCellDoubleClicked((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 2: _t->onCellTodosEdited((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const QList<TodoItem>(*)>(_a[2]))); break;
        case 3: _t->onTodoCompletionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->onTransitionFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CalendarView::*)(const QDate & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarView::dateDoubleClicked)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CalendarView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CalendarView.data,
      qt_meta_data_CalendarView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CalendarView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalendarView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CalendarView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CalendarView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CalendarView::dateDoubleClicked(const QDate & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
