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


private:    

};


void tst_bm_location::initTestCase()
{
}

void tst_bm_location::cleanupTestCase()
{
}

void tst_bm_location::tst_ps_createTime()
{   
    QObject *o = new QObject;
    QGeoPositionInfoSource *ps = NULL;

    QBENCHMARK {
        ps =  QGeoPositionInfoSource::createDefaultSource(o);
    }

    delete ps;
    delete o;
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

void

int main(int argc, char **argv){

    QApplication app(argc, argv);
    
    tst_bm_location test1;
    QTest::qExec(&test1, argc, argv);
    
}
//QTEST_MAIN(tst_messaging);


#include "tst_bm_location.moc"

