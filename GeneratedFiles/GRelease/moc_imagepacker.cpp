/****************************************************************************
** Meta object code from reading C++ file 'imagepacker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../imagepacker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'imagepacker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ImagePacker_t {
    QByteArrayData data[7];
    char stringdata[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ImagePacker_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ImagePacker_t qt_meta_stringdata_ImagePacker = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 12),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 11),
QT_MOC_LITERAL(4, 38, 10),
QT_MOC_LITERAL(5, 49, 9),
QT_MOC_LITERAL(6, 59, 5)
    },
    "ImagePacker\0browseConfig\0\0browseImage\0"
    "saveConfig\0onClicked\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImagePacker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    1,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    6,

       0        // eod
};

void ImagePacker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ImagePacker *_t = static_cast<ImagePacker *>(_o);
        switch (_id) {
        case 0: _t->browseConfig(); break;
        case 1: _t->browseImage(); break;
        case 2: _t->saveConfig(); break;
        case 3: _t->onClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ImagePacker::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ImagePacker.data,
      qt_meta_data_ImagePacker,  qt_static_metacall, 0, 0}
};


const QMetaObject *ImagePacker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImagePacker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImagePacker.stringdata))
        return static_cast<void*>(const_cast< ImagePacker*>(this));
    if (!strcmp(_clname, "CXCallBack"))
        return static_cast< CXCallBack*>(const_cast< ImagePacker*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ImagePacker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
