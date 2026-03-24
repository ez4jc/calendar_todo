/****************************************************************************
** Meta object code from reading C++ file 'tododialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "tododialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tododialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TodoDialog_t {
    QByteArrayData data[12];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TodoDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TodoDialog_t qt_meta_stringdata_TodoDialog = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TodoDialog"
QT_MOC_LITERAL(1, 11, 9), // "todoSaved"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 8), // "TodoItem"
QT_MOC_LITERAL(4, 31, 4), // "todo"
QT_MOC_LITERAL(5, 36, 11), // "todoDeleted"
QT_MOC_LITERAL(6, 48, 2), // "id"
QT_MOC_LITERAL(7, 51, 6), // "onSave"
QT_MOC_LITERAL(8, 58, 8), // "onDelete"
QT_MOC_LITERAL(9, 67, 13), // "onItemClicked"
QT_MOC_LITERAL(10, 81, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(11, 98, 4) // "item"

    },
    "TodoDialog\0todoSaved\0\0TodoItem\0todo\0"
    "todoDeleted\0id\0onSave\0onDelete\0"
    "onItemClicked\0QListWidgetItem*\0item"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TodoDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x08 /* Private */,
       8,    0,   46,    2, 0x08 /* Private */,
       9,    1,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void TodoDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TodoDialog *_t = static_cast<TodoDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->todoSaved((*reinterpret_cast< const TodoItem(*)>(_a[1]))); break;
        case 1: _t->todoDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onSave(); break;
        case 3: _t->onDelete(); break;
        case 4: _t->onItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TodoDialog::*)(const TodoItem & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TodoDialog::todoSaved)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TodoDialog::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TodoDialog::todoDeleted)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TodoDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TodoDialog.data,
      qt_meta_data_TodoDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TodoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TodoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TodoDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int TodoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void TodoDialog::todoSaved(const TodoItem & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TodoDialog::todoDeleted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
