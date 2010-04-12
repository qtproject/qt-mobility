#include "tst_qgeopositioninfosource.h"
#include <stdlib.h>
using namespace std;

tst_QGeoPositionInfoSource::tst_QGeoPositionInfoSource():i(0) {}


int tst_QGeoPositionInfoSource::init(int c, int i, bool useGPS)
{
    source = QGeoPositionInfoSource::createDefaultSource(this);
    if(source == 0) {
        cerr << "###TEST### Location Source Failure.\n";
        return -1;
    }

    QObject::connect(source,SIGNAL(positionUpdated(const QGeoPositionInfo)),
                     this,SLOT(newPosition(const QGeoPositionInfo)));
    count = c;
    cout << "###TEST### Updates: " << c << "\n";

    source->setUpdateInterval(i);
    cout << "###TEST### Interval: " << i << "\n";

    if(useGPS) {
        cout << "###TEST### Method: GPS\n";
        source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    }
    else {
        cout << "###TEST### Method: Network\n";
        source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    }

    source->startUpdates();
 
    return 0;
}


void tst_QGeoPositionInfoSource::newPosition(const QGeoPositionInfo &update)
{
    i++;
    QGeoCoordinate c = update.coordinate();
    cout << "###TEST###  Lat: " << c.latitude();
    cout << " Lon: " << c.longitude();
    cout << " Alt: " << c.altitude();
    cout << " Heading: " << update.attribute(QGeoPositionInfo::Direction);
    cout << " HSpeed: " << update.attribute(QGeoPositionInfo::GroundSpeed);
    cout << " VSpeed: " << update.attribute(QGeoPositionInfo::VerticalSpeed);
    cout << endl;

    if(i==count) {
        cout << "###TEST### End.\n";
        source->stopUpdates();
        sleep(1);
        emit the_end();
    }

}


void tst_QGeoPositionInfoSource::start()
{
    cout << "Application start - request position updates" << endl;
    emit startTracking();
}


#define INTERVAL 1000
int main(int argc, char **argv)
{
  const char *gpsStr = "gps";
  QCoreApplication app(argc, argv);

  tst_QGeoPositionInfoSource *testapp = new tst_QGeoPositionInfoSource();
  QObject::connect(testapp,SIGNAL(the_end()), &app, SLOT(quit()));

  int count = 5, interval = 5000;
  bool useGPS = false;


  if(argc >= 2) {
      count = strtol(argv[1],NULL, 10);
  }
  if(argc >= 3) {
      interval =  strtol(argv[2],NULL, 10);
  }
  if(argc >= 4) {
      if(strncmp(argv[3],gpsStr,3) == 0) useGPS = true;
  }


  if(testapp->init(count, interval, useGPS) == -1){
      return 1;
  }

  app.exec();

  return 0;
}

