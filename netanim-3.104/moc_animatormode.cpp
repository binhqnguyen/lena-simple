/****************************************************************************
** Meta object code from reading C++ file 'animatormode.h'
**
** Created: Fri Jan 3 00:05:26 2014
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "animator/animatormode.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'animatormode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_netanim__AnimatorMode[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      34,   22,   22,   22, 0x08,
      59,   22,   22,   22, 0x08,
      77,   22,   22,   22, 0x08,
      96,   22,   22,   22, 0x08,
     112,   22,   22,   22, 0x08,
     129,   22,   22,   22, 0x08,
     145,   22,   22,   22, 0x08,
     177,  171,   22,   22, 0x08,
     214,  171,   22,   22, 0x08,
     238,   22,   22,   22, 0x08,
     262,  171,   22,   22, 0x08,
     287,  171,   22,   22, 0x08,
     315,   22,   22,   22, 0x08,
     341,   22,   22,   22, 0x08,
     361,   22,   22,   22, 0x08,
     378,   22,   22,   22, 0x08,
     393,   22,   22,   22, 0x08,
     410,   22,   22,   22, 0x08,
     436,   22,   22,   22, 0x08,
     458,   22,   22,   22, 0x08,
     486,   22,   22,   22, 0x08,
     499,   22,   22,   22, 0x08,
     513,   22,   22,   22, 0x08,
     535,   22,   22,   22, 0x08,
     555,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_netanim__AnimatorMode[] = {
    "netanim::AnimatorMode\0\0testSlot()\0"
    "clickTraceFileOpenSlot()\0clickZoomInSlot()\0"
    "clickZoomOutSlot()\0clickSaveSlot()\0"
    "clickResetSlot()\0clickPlaySlot()\0"
    "clickAddCustomImageSlot()\0value\0"
    "updatePacketPersistenceSlot(QString)\0"
    "updateTimelineSlot(int)\0updateRateTimeoutSlot()\0"
    "updateGridLinesSlot(int)\0"
    "updateNodeSizeSlot(QString)\0"
    "updateUpdateRateSlot(int)\0showGridLinesSlot()\0"
    "showNodeIdSlot()\0showMetaSlot()\0"
    "showPacketSlot()\0showWirelessCirclesSlot()\0"
    "showPacketStatsSlot()\0showNodePositionStatsSlot()\0"
    "showIpSlot()\0showMacSlot()\0"
    "setUnicastMatchSlot()\0showRoutePathSlot()\0"
    "showBatteryCapacitySlot()\0"
};

const QMetaObject netanim::AnimatorMode::staticMetaObject = {
    { &Mode::staticMetaObject, qt_meta_stringdata_netanim__AnimatorMode,
      qt_meta_data_netanim__AnimatorMode, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &netanim::AnimatorMode::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *netanim::AnimatorMode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *netanim::AnimatorMode::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_netanim__AnimatorMode))
        return static_cast<void*>(const_cast< AnimatorMode*>(this));
    return Mode::qt_metacast(_clname);
}

int netanim::AnimatorMode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Mode::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: testSlot(); break;
        case 1: clickTraceFileOpenSlot(); break;
        case 2: clickZoomInSlot(); break;
        case 3: clickZoomOutSlot(); break;
        case 4: clickSaveSlot(); break;
        case 5: clickResetSlot(); break;
        case 6: clickPlaySlot(); break;
        case 7: clickAddCustomImageSlot(); break;
        case 8: updatePacketPersistenceSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: updateTimelineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: updateRateTimeoutSlot(); break;
        case 11: updateGridLinesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: updateNodeSizeSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: updateUpdateRateSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: showGridLinesSlot(); break;
        case 15: showNodeIdSlot(); break;
        case 16: showMetaSlot(); break;
        case 17: showPacketSlot(); break;
        case 18: showWirelessCirclesSlot(); break;
        case 19: showPacketStatsSlot(); break;
        case 20: showNodePositionStatsSlot(); break;
        case 21: showIpSlot(); break;
        case 22: showMacSlot(); break;
        case 23: setUnicastMatchSlot(); break;
        case 24: showRoutePathSlot(); break;
        case 25: showBatteryCapacitySlot(); break;
        default: ;
        }
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
