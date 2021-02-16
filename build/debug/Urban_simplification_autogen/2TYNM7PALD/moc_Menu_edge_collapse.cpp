/****************************************************************************
** Meta object code from reading C++ file 'Menu_edge_collapse.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/Menu_edge_collapse.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Menu_edge_collapse.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Menu_edge_collapse_t {
    QByteArrayData data[17];
    char stringdata0[313];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Menu_edge_collapse_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Menu_edge_collapse_t qt_meta_stringdata_Menu_edge_collapse = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Menu_edge_collapse"
QT_MOC_LITERAL(1, 19, 14), // "collapse_edges"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "show_primitives"
QT_MOC_LITERAL(4, 51, 11), // "show_points"
QT_MOC_LITERAL(5, 63, 9), // "show_heap"
QT_MOC_LITERAL(6, 73, 19), // "update_num_vertices"
QT_MOC_LITERAL(7, 93, 12), // "num_vertices"
QT_MOC_LITERAL(8, 106, 20), // "update_num_collapses"
QT_MOC_LITERAL(9, 127, 17), // "new_num_collapses"
QT_MOC_LITERAL(10, 145, 18), // "update_gui_numbers"
QT_MOC_LITERAL(11, 164, 19), // "update_slider_value"
QT_MOC_LITERAL(12, 184, 40), // "box_value_num_remaining_verti..."
QT_MOC_LITERAL(13, 225, 9), // "new_value"
QT_MOC_LITERAL(14, 235, 39), // "box_value_num_remaining_verti..."
QT_MOC_LITERAL(15, 275, 20), // "slider_value_changed"
QT_MOC_LITERAL(16, 296, 16) // "new_slider_value"

    },
    "Menu_edge_collapse\0collapse_edges\0\0"
    "show_primitives\0show_points\0show_heap\0"
    "update_num_vertices\0num_vertices\0"
    "update_num_collapses\0new_num_collapses\0"
    "update_gui_numbers\0update_slider_value\0"
    "box_value_num_remaining_vertices_changed\0"
    "new_value\0box_value_num_remaining_vertices_edited\0"
    "slider_value_changed\0new_slider_value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Menu_edge_collapse[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       3,    0,   77,    2, 0x06 /* Public */,
       4,    0,   78,    2, 0x06 /* Public */,
       5,    0,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   80,    2, 0x0a /* Public */,
       8,    1,   83,    2, 0x0a /* Public */,
      10,    0,   86,    2, 0x0a /* Public */,
      11,    0,   87,    2, 0x0a /* Public */,
      12,    1,   88,    2, 0x0a /* Public */,
      14,    0,   91,    2, 0x0a /* Public */,
      15,    1,   92,    2, 0x0a /* Public */,
       1,    0,   95,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,

       0        // eod
};

void Menu_edge_collapse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Menu_edge_collapse *_t = static_cast<Menu_edge_collapse *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->collapse_edges((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->show_primitives(); break;
        case 2: _t->show_points(); break;
        case 3: _t->show_heap(); break;
        case 4: _t->update_num_vertices((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->update_num_collapses((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->update_gui_numbers(); break;
        case 7: _t->update_slider_value(); break;
        case 8: _t->box_value_num_remaining_vertices_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->box_value_num_remaining_vertices_edited(); break;
        case 10: _t->slider_value_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->collapse_edges(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Menu_edge_collapse::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_edge_collapse::collapse_edges)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Menu_edge_collapse::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_edge_collapse::show_primitives)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Menu_edge_collapse::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_edge_collapse::show_points)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Menu_edge_collapse::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Menu_edge_collapse::show_heap)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject Menu_edge_collapse::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Menu_edge_collapse.data,
      qt_meta_data_Menu_edge_collapse,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Menu_edge_collapse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Menu_edge_collapse::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Menu_edge_collapse.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::Menu_edge_collapse"))
        return static_cast< Ui::Menu_edge_collapse*>(this);
    return QDialog::qt_metacast(_clname);
}

int Menu_edge_collapse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Menu_edge_collapse::collapse_edges(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Menu_edge_collapse::show_primitives()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Menu_edge_collapse::show_points()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Menu_edge_collapse::show_heap()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
