/****************************************************************************
** Meta object code from reading C++ file 'Viewer_instructor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/Viewer_instructor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Viewer_instructor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Viewer_instructor_t {
    QByteArrayData data[13];
    char stringdata0[262];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Viewer_instructor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Viewer_instructor_t qt_meta_stringdata_Viewer_instructor = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Viewer_instructor"
QT_MOC_LITERAL(1, 18, 12), // "sceneChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 20), // "change_draw_vertices"
QT_MOC_LITERAL(4, 53, 17), // "change_draw_edges"
QT_MOC_LITERAL(5, 71, 21), // "change_draw_triangles"
QT_MOC_LITERAL(6, 93, 22), // "change_draw_primitives"
QT_MOC_LITERAL(7, 116, 28), // "change_draw_primitives_graph"
QT_MOC_LITERAL(8, 145, 29), // "change_draw_primitives_border"
QT_MOC_LITERAL(9, 175, 29), // "change_draw_primitives_corner"
QT_MOC_LITERAL(10, 205, 21), // "change_draw_best_edge"
QT_MOC_LITERAL(11, 227, 21), // "change_draw_new_point"
QT_MOC_LITERAL(12, 249, 12) // "change_light"

    },
    "Viewer_instructor\0sceneChanged\0\0"
    "change_draw_vertices\0change_draw_edges\0"
    "change_draw_triangles\0change_draw_primitives\0"
    "change_draw_primitives_graph\0"
    "change_draw_primitives_border\0"
    "change_draw_primitives_corner\0"
    "change_draw_best_edge\0change_draw_new_point\0"
    "change_light"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Viewer_instructor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x0a /* Public */,
       4,    0,   71,    2, 0x0a /* Public */,
       5,    0,   72,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x0a /* Public */,
       7,    0,   74,    2, 0x0a /* Public */,
       8,    0,   75,    2, 0x0a /* Public */,
       9,    0,   76,    2, 0x0a /* Public */,
      10,    1,   77,    2, 0x0a /* Public */,
      11,    1,   80,    2, 0x0a /* Public */,
      12,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

       0        // eod
};

void Viewer_instructor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Viewer_instructor *_t = static_cast<Viewer_instructor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sceneChanged(); break;
        case 1: _t->change_draw_vertices(); break;
        case 2: _t->change_draw_edges(); break;
        case 3: _t->change_draw_triangles(); break;
        case 4: _t->change_draw_primitives(); break;
        case 5: _t->change_draw_primitives_graph(); break;
        case 6: _t->change_draw_primitives_border(); break;
        case 7: _t->change_draw_primitives_corner(); break;
        case 8: _t->change_draw_best_edge((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->change_draw_new_point((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->change_light(); break;
        default: ;
        }
    }
}

const QMetaObject Viewer_instructor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Viewer_instructor.data,
      qt_meta_data_Viewer_instructor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Viewer_instructor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Viewer_instructor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Viewer_instructor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Viewer_instructor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
