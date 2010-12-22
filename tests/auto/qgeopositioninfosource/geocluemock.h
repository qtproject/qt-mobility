/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GEOCLUEMOCK_H
#define GEOCLUEMOCK_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <qgeopositioninfosource_geocluemaster_p.h>
#include <geoclue/geoclue-master.h>
#include <geoclue/geoclue-master-client.h>
#include <geoclue/geoclue-velocity.h>
#include <geoclue/geoclue-position.h>

//#define TST_GEOCLUE_MOCK_TRACE 1

void geocluemock_setjournal(QString journal);

typedef void (*PositionChangedCallback) (GeocluePosition*,
                   GeocluePositionFields,
                   int,
                   double,
                   double,
                   double,
                   GeoclueAccuracy*,
                   gpointer);

typedef void (*VelocityChangedCallback) (GeoclueVelocity*,
                              GeoclueVelocityFields,
                              int,
                              double,
                              double,
                              double,
                              gpointer);

typedef void (*PositionCallback) (GeocluePosition*,
                   GeocluePositionFields,
                   int,
                   double,
                   double,
                   double,
                   GeoclueAccuracy*,
                   GError*,
                   gpointer);

class GeoclueMock: public QObject
{
    Q_OBJECT
public:
    explicit GeoclueMock();
    ~GeoclueMock();
    void setPositionSource(QObject* positionSource);
    void setPositionChangedCallback(void (*callback)());
    void setVelocityChangedCallback(void (*callback)());
    void setPositionCallback(GeocluePositionCallback callback);
    void disconnectSignal(gpointer callback, gpointer positionSource);
    void singleUpdate();

    enum UPDATE_TYPE {
        INVALID_UPDATE = 0,
        REGULAR_UPDATE,
        SINGLE_UPDATE,
        VELOCITY_UPDATE
    };

    typedef struct {
        int timerId;
        int timeout;
        int repeats;
        int timestamp;
        int fields;
        double latitude;
        double longitude;
        double altitude;
        double speed;
        double direction;
        double climb;
        UPDATE_TYPE updateType;
    } UpdateData;

public slots:
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent *);

private:
    UPDATE_TYPE readNextEntry();
    void parseRegularUpdate(QTextStream& stream);
    void parseSingleUpdate(QTextStream& stream);
    void parseVelocityUpdate(QTextStream& stream);

private:
    QFile m_currentJournalFile;
    QObject* m_positionSource;
    UpdateData m_regular;
    UpdateData m_single;
    PositionChangedCallback m_positionChangedCallback;
    VelocityChangedCallback m_velocityChangedCallback;
    PositionCallback m_positionCallback;
};

#endif // GEOCLUEMOCK_H
