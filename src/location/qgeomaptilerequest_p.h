#ifndef QMAPTILEREQUEST_P_H
#define QMAPTILEREQUEST_P_H

#include "qmobilityglobal.h"
#include "qgeomaptile.h"

QTM_BEGIN_NAMESPACE

class QGeoMapTileRequestPrivate
{
public:
    QGeoMapTileRequestPrivate();
    QGeoMapTileRequestPrivate(const QGeoMapTileRequestPrivate &mtrp);
    QGeoMapTileRequestPrivate& operator= (const QGeoMapTileRequestPrivate &mtrp);

    QString requestString(const QString &host, const QString &token, const QString &referrer) const;

    quint32 cl;
    quint32 rw;
    MapVersion ver;
    quint16 zoom;
    MapScheme schm;
    MapResolution res;
    MapFormat frmt;
};

QTM_END_NAMESPACE

#endif // QMAPTILEREQUEST_P_H
