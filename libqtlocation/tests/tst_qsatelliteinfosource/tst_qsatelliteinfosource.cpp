#include "tst_qsatelliteinfosource.h"
#include <stdlib.h>
#include <iomanip>

using namespace std;

TestApp::TestApp():i(0), count(5) 
{
}


void TestApp::viewInfo(const QList<QGeoSatelliteInfo> &satellites)
{
    int cnt = satellites.size();
    cout << "Number of satellites in view: " << cnt << "\n";

    for (int s = 0; s < cnt; s++) {
        printSatInfo(satellites.at(s));
    }

    if (i==count) {
        emit stopTracking();
        sleep(1);
        emit theEnd();
    }
}

void TestApp::usedInfo(const QList<QGeoSatelliteInfo> &satellites)
{
    int cnt = satellites.size();
    cout << "Number of satellites used: " << cnt << "\n";

    for(int s = 0; s < cnt; s++){
        printSatInfo(satellites.at(s));
    }
    
    if(i==count) {
        emit stopTracking();
        sleep(1);
        emit theEnd();
    }
}

void TestApp::printSatInfo(const QGeoSatelliteInfo &sat) const
{
    if (sat.signalStrength() != -1) {
        cout << "prn: " << setw(2) << sat.prnNumber();
        cout << " snr: " <<  setw(3) << sat.signalStrength(); 
        cout << " azimuth: " << setw(6) << sat.attribute(QGeoSatelliteInfo::Azimuth);
        cout << " elevation: " << sat.attribute(QGeoSatelliteInfo::Elevation) << endl; 
    }
}

void TestApp::start()
{
    cout << "Application start - request satellite info updates" << endl;
    emit startTracking();
}



int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  TestApp testapp;

  if(argc == 2) {
      testapp.count = strtol(argv[1],NULL, 10);
  }

  QGeoSatelliteInfoSource* source = QGeoSatelliteInfoSource::createDefaultSource(0);
  QGeoPositionInfoSource* psource = QGeoPositionInfoSource::createDefaultSource(0);

  if(source == 0) {
      cerr << "Satellite Info Source Failure.\n";
      return -1;
  }

  QObject::connect(&testapp,SIGNAL(startTracking()),
                   source,SLOT(startUpdates()));

  QObject::connect(&testapp,SIGNAL(stopTracking()),
                   source,SLOT(stopUpdates()));

  QObject::connect(source,SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)),
                   &testapp,SLOT(viewInfo(const QList<QGeoSatelliteInfo> &)));
  QObject::connect(source,SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)),
                   &testapp,SLOT(usedInfo(const QList<QGeoSatelliteInfo> &)));
  
  QObject::connect(&testapp,SIGNAL(theEnd()), &app, SLOT(quit()));

  psource->setUpdateInterval(1000);
  psource->startUpdates();
  testapp.start();

  app.exec();

  return 0;
}

