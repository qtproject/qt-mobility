#ifndef _LOCATION_H_
#define _LOCATION_H_

#include <QObject>
#include "controlpanel.h"
#include "qgeocoordinate.h"
#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"
#include <iostream>

class Location : public QObject
{
    Q_OBJECT

public:
    Location(ControlPanel* cp);

private:    
    ControlPanel* cp_;
    int state;
    QGeoPositionInfoSource* source;

public slots:
    void updateSettings(bool sat, bool nonsat, int interval);   
    void updateState(bool tracking);
    void shutdown(); 

};

#endif
