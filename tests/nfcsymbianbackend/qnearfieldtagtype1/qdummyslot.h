#ifndef DUMMYSLOT_H
#define DUMMYSLOT_H

#include <QObject>
#include "qnearfieldtarget.h"
#include "qnfctagtestcommon.h"

class QDummySlot : public QObject
{
    Q_OBJECT
public:
    explicit QDummySlot(QObject *parent = 0);

public slots:
    void errorHandling(QNearFieldTarget::Error error);
public:
    // NOT Own
    QNfcTagTestCommon * tester;
};

#endif // DUMMYSLOT_H
