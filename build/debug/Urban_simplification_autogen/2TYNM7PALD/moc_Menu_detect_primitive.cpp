/****************************************************************************
** Meta object code from reading C++ file 'Menu_detect_primitive.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/Menu_detect_primitive.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Menu_detect_primitive.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Menu_detect_primitive_t {
    QByteArrayData data[9];
    char stringdata0[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Menu_detect_primitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Menu_detect_primitive_t qt_meta_stringdata_Menu_detect_primitive = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Menu_detect_primitive"
QT_MOC_LITERAL(1, 22, 17), // "detect_primitives"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 16), // "Detection_params"
QT_MOC_LITERAL(4, 58, 6), // "params"
QT_MOC_LITERAL(5, 65, 21), // "regularize_primitives"
QT_MOC_LITERAL(6, 87, 20), // "Merge_proxies_params"
QT_MOC_LITERAL(7, 108, 22), // "send_detect_primitives"
QT_MOC_LITERAL(8, 131, 26) // "send_regularize_primitives"

    },
    "Menu_detect_primitive\0detect_primitives\0"
    "\0Detection_params\0params\0regularize_primitives\0"
    "Merge_proxies_params\0send_detect_primitives\0"
    "send_regularize_primitives"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Menu_detect_primitive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   40,    2, 0x0a /* Public */,
       8,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Menu_detect_primitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Menu_detect_primitive *_t = static_cast<Menu_detect_primitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->detect_primitives((*reinterpret_cast< const Detection_params(*)>(_a[1]))); break;
        case 1: _t->regularize_primitives((*reinterpret_cast< const Merge_proxies_params(*)>(_a[1]))); break;
        case 2: _t->send_detect_primitives(); break;
        case 3: _t->send_regularize_primitives(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Menu_detect_primitive::*_t)(const Detection_params & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_detect_primitive::detect_primitives)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Menu_detect_primitive::*_t)(const Merge_proxies_params & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_detect_primitive::regularize_primitives)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject Menu_detect_primitive::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Menu_detect_primitive.data,
      qt_meta_data_Menu_detect_primitive,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Menu_detect_primitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Menu_detect_primitive::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Menu_detect_primitive.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::Menu_detect_primitive"))
        return static_cast< Ui::Menu_detect_primitive*>(this);
    return QDialog::qt_metacast(_clname);
}

int Menu_detect_primitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Menu_detect_primitive::detect_primitives(const Detection_params & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Menu_detect_primitive::regularize_primitives(const Merge_proxies_params & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
