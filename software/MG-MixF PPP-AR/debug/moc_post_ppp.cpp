/****************************************************************************
** Meta object code from reading C++ file 'post_ppp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ppp_model/post_ppp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'post_ppp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_post_ppp_t {
    QByteArrayData data[6];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_post_ppp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_post_ppp_t qt_meta_stringdata_post_ppp = {
    {
QT_MOC_LITERAL(0, 0, 8), // "post_ppp"
QT_MOC_LITERAL(1, 9, 9), // "changexyz"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 15), // "wigdet_transmit"
QT_MOC_LITERAL(4, 36, 4), // "test"
QT_MOC_LITERAL(5, 41, 7) // "sysMain"

    },
    "post_ppp\0changexyz\0\0wigdet_transmit\0"
    "test\0sysMain"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_post_ppp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void post_ppp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        post_ppp *_t = static_cast<post_ppp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changexyz((*reinterpret_cast< wigdet_transmit(*)>(_a[1]))); break;
        case 1: _t->test((*reinterpret_cast< wigdet_transmit(*)>(_a[1]))); break;
        case 2: _t->sysMain(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (post_ppp::*_t)(wigdet_transmit );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&post_ppp::changexyz)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (post_ppp::*_t)(wigdet_transmit );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&post_ppp::test)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject post_ppp::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_post_ppp.data,
      qt_meta_data_post_ppp,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *post_ppp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *post_ppp::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_post_ppp.stringdata0))
        return static_cast<void*>(const_cast< post_ppp*>(this));
    return QThread::qt_metacast(_clname);
}

int post_ppp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void post_ppp::changexyz(wigdet_transmit _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void post_ppp::test(wigdet_transmit _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
