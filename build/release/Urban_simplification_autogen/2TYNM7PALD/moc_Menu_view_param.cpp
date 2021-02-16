/****************************************************************************
** Meta object code from reading C++ file 'Menu_view_param.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/Menu_view_param.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Menu_view_param.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Menu_view_param_t {
    QByteArrayData data[15];
    char stringdata0[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Menu_view_param_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Menu_view_param_t qt_meta_stringdata_Menu_view_param = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Menu_view_param"
QT_MOC_LITERAL(1, 16, 18), // "update_vertex_size"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 8), // "new_size"
QT_MOC_LITERAL(4, 45, 19), // "update_vertex_style"
QT_MOC_LITERAL(5, 65, 9), // "new_style"
QT_MOC_LITERAL(6, 75, 16), // "update_edge_size"
QT_MOC_LITERAL(7, 92, 17), // "update_edge_style"
QT_MOC_LITERAL(8, 110, 22), // "update_edge_luminosity"
QT_MOC_LITERAL(9, 133, 9), // "new_value"
QT_MOC_LITERAL(10, 143, 26), // "update_triangle_luminosity"
QT_MOC_LITERAL(11, 170, 12), // "update_theta"
QT_MOC_LITERAL(12, 183, 9), // "new_theta"
QT_MOC_LITERAL(13, 193, 10), // "update_phi"
QT_MOC_LITERAL(14, 204, 21) // "update_relative_light"

    },
    "Menu_view_param\0update_vertex_size\0\0"
    "new_size\0update_vertex_style\0new_style\0"
    "update_edge_size\0update_edge_style\0"
    "update_edge_luminosity\0new_value\0"
    "update_triangle_luminosity\0update_theta\0"
    "new_theta\0update_phi\0update_relative_light"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Menu_view_param[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a /* Public */,
       4,    1,   62,    2, 0x0a /* Public */,
       6,    1,   65,    2, 0x0a /* Public */,
       7,    1,   68,    2, 0x0a /* Public */,
       8,    1,   71,    2, 0x0a /* Public */,
      10,    1,   74,    2, 0x0a /* Public */,
      11,    1,   77,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      14,    1,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void Menu_view_param::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Menu_view_param *_t = static_cast<Menu_view_param *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->update_vertex_size((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->update_vertex_style((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->update_edge_size((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->update_edge_style((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->update_edge_luminosity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->update_triangle_luminosity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->update_theta((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->update_phi((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->update_relative_light((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Menu_view_param::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Menu_view_param.data,
      qt_meta_data_Menu_view_param,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Menu_view_param::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Menu_view_param::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Menu_view_param.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::Menu_view_param"))
        return static_cast< Ui::Menu_view_param*>(this);
    return QDialog::qt_metacast(_clname);
}

int Menu_view_param::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
