/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QApplication>
#include <QtTest/QtTest>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include <qdebug.h>

#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>

QTM_USE_NAMESPACE

class tst_bm_location : public QObject
{
    Q_OBJECT

public:    
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
//    void tst_createTime_data();
    void tst_ps_createTime();
//    void tst_ps_createTime_2nd();
    void tst_ps_setInterval();
    void tst_ps_interval();
    void tst_ps_setMethod();
    void tst_ps_method();
    void tst_ps_lastKnownPos();
    void tst_ps_supportedPodMethods();
    void tst_ps_minUpdateInterval();
    void tst_ps_startRequest();
    void tst_ps_stopUpdates();
    void tst_ps_requestUpdates();
    
    void tst_ps_getUpdate();
    
    void tst_si_createTime();
    void tst_si_startUpdates();
    void tst_si_stopUpdates();    
    void tst_si_requestUpdates();
    
    void tst_si_getUpdate();
    
    
public slots:
    void timeout();
    
    void positionUpdated(const QGeoPositionInfo &update);
    void updateTimeout();
    
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);

private:    
    QEventLoop *loop;
    QTimer *timer;
    
    int early_exit;

};


void tst_bm_location::initTestCase()
{
  loop = new QEventLoop;
  timer = new QTimer(this);
  timer->setSingleShot(true);
  connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

void tst_bm_location::cleanupTestCase()
{
  delete loop;
  delete timer;
}

void tst_bm_location::tst_ps_createTime()
{   
    QGeoPositionInfoSource *ps = NULL;

    QBENCHMARK {
        ps =  QGeoPositionInfoSource::createDefaultSource(NULL);
    }

    delete ps;
}

void tst_bm_location::tst_ps_setInterval()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->setUpdateInterval(3000);    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_interval()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->updateInterval();    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_setMethod()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);    
  }
  delete ps;    
}


void tst_bm_location::tst_ps_method()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->preferredPositioningMethods();    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_lastKnownPos()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->lastKnownPosition();    
  }
  delete ps;  

}
//void tst_bm_location::tst_ps_createTime_2nd()
//{
//    QObject *o = new QObject;
//    QGeoPositionInfoSource *ps = NULL;
//
//    QBENCHMARK {
//        ps =  QGeoPositionInfoSource::createDefaultSource(o);
//    }
//
//    delete ps;
//    delete o;
//}

void tst_bm_location::tst_ps_supportedPodMethods()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->supportedPositioningMethods();    
  }
  delete ps;    
}

void tst_bm_location::tst_ps_minUpdateInterval()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->minimumUpdateInterval();    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_startRequest()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->startUpdates();    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_stopUpdates()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->stopUpdates();    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_requestUpdates()
{
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    ps->requestUpdate(5000);    
  }
  delete ps;  
}

void tst_bm_location::tst_ps_getUpdate(){ // XXX test may not produce stable results, might need to be removed
  QGeoPositionInfoSource *ps = QGeoPositionInfoSource::createDefaultSource(NULL);

  early_exit = 0;
  connect(ps, SIGNAL(updateTimeout()), this, SLOT(updateTimeout()));
  connect(ps, SIGNAL(positionUpdated(const QGeoPositionInfo &)), 
      this, SLOT(positionUpdated(const QGeoPositionInfo &)));
    
  timer->start(20000);
  ps->startUpdates(); // XXX work around for bug, remove me
  
  QBENCHMARK {
    ps->requestUpdate(15000);    
    if(early_exit || (loop->exec() > 1)){
      QFAIL("Test failed to provide any results within backup timeout");      
    }
  }
  
  ps->stopUpdates();
  delete ps;  
}

void tst_bm_location::updateTimeout()
{
  qDebug() << "Update timeout";
  early_exit = 1;
  loop->exit(1);
}

void tst_bm_location::timeout()
{
  qDebug() << "Timer timeout";
  early_exit = 1;
  loop->exit(2);  
}

void tst_bm_location::positionUpdated(const QGeoPositionInfo &update)
{
//  qDebug() << "Got location info" << update.timestamp() << " - " << update.isValid();
  early_exit = 1;
  loop->exit(0);
}

void tst_bm_location::tst_si_createTime()
{ 
  QGeoSatelliteInfoSource *si = NULL;
  QBENCHMARK {
   si = QGeoSatelliteInfoSource::createDefaultSource(NULL);        
  }
  delete si;    
}

void tst_bm_location::tst_si_startUpdates()
{
  QGeoSatelliteInfoSource *si = QGeoSatelliteInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    si->startUpdates();
  }
  delete si;
}

void tst_bm_location::tst_si_stopUpdates()
{
  QGeoSatelliteInfoSource *si = QGeoSatelliteInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    si->stopUpdates();
  }
  delete si;
}

void tst_bm_location::tst_si_requestUpdates()
{
  QGeoSatelliteInfoSource *si = QGeoSatelliteInfoSource::createDefaultSource(NULL);
  
  QBENCHMARK {
    si->requestUpdate(0);
  }
  delete si;
}

void tst_bm_location::tst_si_getUpdate(){ // XXX test may not produce stable results, might need to be removed
  QGeoSatelliteInfoSource *si = QGeoSatelliteInfoSource::createDefaultSource(NULL);
  
  early_exit = 0;
  connect(si, SIGNAL(requestTimeout()), this, SLOT(updateTimeout()));
  connect(si, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)), 
      this, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
  connect(si, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)), 
      this, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
    
  timer->start(20000);
  si->startUpdates(); // XXX work around for bug, remove me
  
  QBENCHMARK {
    si->requestUpdate(15000);    
    if(early_exit || (loop->exec() > 1)){
      QFAIL("Test failed to provide any results within backup timeout");      
    }
  }
  
  si->stopUpdates();
  delete si;  
}

void tst_bm_location::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)
{
//  qDebug() << "Got location info" << update.timestamp() << " - " << update.isValid();
  early_exit = 1;
  loop->exit(0);
}

void tst_bm_location::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)
{
//  qDebug() << "Got location info" << update.timestamp() << " - " << update.isValid();
  early_exit = 1;
  loop->exit(0);
}



int main(int argc, char **argv){

    QApplication app(argc, argv);
    
    tst_bm_location test1;
    QTest::qExec(&test1, argc, argv);
    
}
//QTEST_MAIN(tst_messaging);


#include "tst_bm_location.moc"

