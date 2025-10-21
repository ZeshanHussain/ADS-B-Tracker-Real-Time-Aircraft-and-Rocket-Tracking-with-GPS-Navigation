/****************************************************************************
** Meta object code from reading C++ file 'systemgps.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/systemgps.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'systemgps.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9SystemGPSE_t {};
} // unnamed namespace

template <> constexpr inline auto SystemGPS::qt_create_metaobjectdata<qt_meta_tag_ZN9SystemGPSE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SystemGPS",
        "positionUpdated",
        "",
        "lat",
        "lon",
        "altitude",
        "speedUpdated",
        "speed",
        "headingUpdated",
        "heading",
        "accuracyUpdated",
        "accuracy",
        "gpsError",
        "message",
        "onPositionUpdated",
        "QGeoPositionInfo",
        "info",
        "onPositionError",
        "QGeoPositionInfoSource::Error",
        "error",
        "onTimeout"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'positionUpdated'
        QtMocHelpers::SignalData<void(double, double, double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 }, { QMetaType::Double, 4 }, { QMetaType::Double, 5 },
        }}),
        // Signal 'speedUpdated'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 7 },
        }}),
        // Signal 'headingUpdated'
        QtMocHelpers::SignalData<void(double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 9 },
        }}),
        // Signal 'accuracyUpdated'
        QtMocHelpers::SignalData<void(double)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 11 },
        }}),
        // Signal 'gpsError'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'onPositionUpdated'
        QtMocHelpers::SlotData<void(const QGeoPositionInfo &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'onPositionError'
        QtMocHelpers::SlotData<void(QGeoPositionInfoSource::Error)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'onTimeout'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SystemGPS, qt_meta_tag_ZN9SystemGPSE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SystemGPS::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SystemGPSE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SystemGPSE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9SystemGPSE_t>.metaTypes,
    nullptr
} };

void SystemGPS::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SystemGPS *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->positionUpdated((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 1: _t->speedUpdated((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->headingUpdated((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->accuracyUpdated((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->gpsError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onPositionUpdated((*reinterpret_cast< std::add_pointer_t<QGeoPositionInfo>>(_a[1]))); break;
        case 6: _t->onPositionError((*reinterpret_cast< std::add_pointer_t<QGeoPositionInfoSource::Error>>(_a[1]))); break;
        case 7: _t->onTimeout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SystemGPS::*)(double , double , double )>(_a, &SystemGPS::positionUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SystemGPS::*)(double )>(_a, &SystemGPS::speedUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SystemGPS::*)(double )>(_a, &SystemGPS::headingUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SystemGPS::*)(double )>(_a, &SystemGPS::accuracyUpdated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SystemGPS::*)(const QString & )>(_a, &SystemGPS::gpsError, 4))
            return;
    }
}

const QMetaObject *SystemGPS::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SystemGPS::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SystemGPSE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SystemGPS::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SystemGPS::positionUpdated(double _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void SystemGPS::speedUpdated(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SystemGPS::headingUpdated(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SystemGPS::accuracyUpdated(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void SystemGPS::gpsError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
