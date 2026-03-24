/****************************************************************************
** Meta object code from reading C++ file 'calendarcell.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "calendarcell.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calendarcell.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CalendarCell_t {
    QByteArrayData data[10];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalendarCell_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalendarCell_t qt_meta_stringdata_CalendarCell = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CalendarCell"
QT_MOC_LITERAL(1, 13, 13), // "doubleClicked"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "date"
QT_MOC_LITERAL(4, 33, 11), // "todosEdited"
QT_MOC_LITERAL(5, 45, 15), // "QList<TodoItem>"
QT_MOC_LITERAL(6, 61, 5), // "todos"
QT_MOC_LITERAL(7, 67, 21), // "todoCompletionChanged"
QT_MOC_LITERAL(8, 89, 6), // "todoId"
QT_MOC_LITERAL(9, 96, 9) // "completed"

    },
    "CalendarCell\0doubleClicked\0\0date\0"
    "todosEdited\0QList<TodoItem>\0todos\0"
    "todoCompletionChanged\0todoId\0completed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalendarCell[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,
       7,    2,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QDate,    3,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 5,    3,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    8,    9,

       0        // eod
};

void CalendarCell::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CalendarCell *_t = static_cast<CalendarCell *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doubleClicked((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: _t->todosEdited((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const QList<TodoItem>(*)>(_a[2]))); break;
        case 2: _t->todoCompletionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CalendarCell::*)(const QDate & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarCell::doubleClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CalendarCell::*)(const QDate & , const QList<TodoItem> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarCell::todosEdited)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CalendarCell::*)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarCell::todoCompletionChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CalendarCell::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CalendarCell.data,
      qt_meta_data_CalendarCell,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CalendarCell::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalendarCell::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CalendarCell.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CalendarCell::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CalendarCell::doubleClicked(const QDate & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CalendarCell::todosEdited(const QDate & _t1, const QList<TodoItem> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CalendarCell::todoCompletionChanged(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
