#ifndef QGEOSATELLITEINFOSOURCE_MEEGO_H
#define QGEOSATELLITEINFOSOURCE_MEEGO_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"

#include <gypsy/gypsy-satellite.h>
QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourceMeego : public QGeoSatelliteInfoSource
  {
      Q_OBJECT

public:
      explicit QGeoSatelliteInfoSourceMeego(QObject *parent = 0);
      int init();

public slots:
      virtual void startUpdates();
      void stopUpdates();
      void requestUpdate(int timeout = 5000);
      void satellitesChanged(GypsySatellite* satellite, GPtrArray* satellites);

signals:
      void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
      void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
      void requestTimeout();

private:
      Q_DISABLE_COPY(QGeoSatelliteInfoSourceMeego)
      GypsySatellite *satellite;
  };

QTM_END_NAMESPACE

#endif // QGEOSATELLITEINFOSOURCE_MEEGO_H

