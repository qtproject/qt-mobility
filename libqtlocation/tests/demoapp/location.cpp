#include "location.h"

Location::Location(ControlPanel* cp)
{
    cp_=cp;
    state = 0;
    source = QGeoPositionInfoSource::createDefaultSource(this); 

    if(source == 0){
        std::cerr << "Creating positioning source failed.\n";
        exit(1);
    }

    QObject::connect(source,SIGNAL(positionUpdated(const QGeoPositionInfo)),
                    cp_,SLOT(setCoordinate(const QGeoPositionInfo)));

    QObject::connect(cp_,SIGNAL(settings(bool,bool,int)),
                    this,SLOT(updateSettings(bool,bool,int)));

    QObject::connect(cp_,SIGNAL(tracking(bool)),
                    this,SLOT(updateState(bool)));

}


void  Location::updateSettings(bool sat, bool nonSat, int interval)
{
    std::cout << "DEMOAPP SETTINGS:      Sat:" << sat << "\n";
    std::cout << "DEMOAPP SETTINGS:   nonSat:" << nonSat << "\n";
    std::cout << "DEMOAPP SETTINGS: interval:" << interval << "\n";

    QGeoPositionInfoSource::PositioningMethod method;

    if(sat) method = (QGeoPositionInfoSource::SatellitePositioningMethods);
    if(nonSat) method = (QGeoPositionInfoSource::NonSatellitePositioningMethods);
    if((sat)&&(nonSat)) method=QGeoPositionInfoSource::AllPositioningMethods;
    
    if(method)
        source->setPreferredPositioningMethods(method);
    
    source->setUpdateInterval(interval*1000);

}

void Location::updateState(bool tracking)
{
    std::cout << "DEMOAPP STATE: Tracking:" << tracking << "\n";
    if(tracking) source->startUpdates();
    else         source->stopUpdates();

}

void  Location::shutdown()
{
    if(state==1)source->stopUpdates();
    delete source;
    exit(0);
}
