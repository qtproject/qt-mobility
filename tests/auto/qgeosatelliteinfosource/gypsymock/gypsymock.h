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

#ifndef GYPSYMOCK_H
#define GYPSYMOCK_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <glib.h>
#include <glib-object.h>
#include <gypsy/gypsy-satellite.h>
#include <gypsy/gypsy-control.h>
#include <gypsy/gypsy-device.h>
#include <gconf/gconf-client.h>

//#define TST_GYPSY_MOCK_TRACE 1

#ifdef TST_GYPSY_MOCK_TRACE
#define TRACE0 qDebug() << "=GypsyMock=" << __FUNCTION__
#define TRACE1(STR1) TRACE0 << STR1;
#define TRACE2(STR1,STR2) TRACE0 << STR1 << STR2;
#define TRACE3(STR1, STR2, STR3) TRACE0 << STR1 << STR2 << STR3;
#else
#define TRACE0
#define TRACE1(STR1)
#define TRACE2(STR1,STR2)
#define TRACE3(STR1, STR2, STR3)
#endif

// ================  Misc functions
void gypsymock_setjournal(QString journal);
typedef void (*SatellitesChangedCallback) (GypsySatellite *satellite,
                                           GPtrArray *satellites,
                                           gpointer userdata);

class GypsyMock: public QObject
{
    Q_OBJECT
public:
    explicit GypsyMock();
    ~GypsyMock(); 
    void disconnectSignal(gpointer callback, gpointer positionSource);
    void setSatellitesChangedCallback(void (*callback)());
    void singleUpdate();

    void disableUpdates(bool value);
    GPtrArray* satelliteArray();

    enum UPDATE_TYPE {
        INVALID_UPDATE = 0,
        REGULAR_UPDATE,
        SINGLE_UPDATE,
    };
    typedef struct {
        int timerId;
        int timeout;
        int repeats;
        GPtrArray* satellites;
        UPDATE_TYPE updateType;
    } UpdateData;

public slots:
    void start();
    void stop();
    void disconnectSignal(int callback, int positionSource);
    void connectSignal(int signal, int callback, int positionSource);

protected:
    void timerEvent(QTimerEvent *);

private:
    UPDATE_TYPE readNextEntry();
    void parseRegularUpdate(QTextStream& stream);
    void parseSatelliteData(QTextStream& stream);

private:
    QString m_currentJournal;
    QObject* m_satelliteSource;
    QFile m_currentJournalFile;
    UpdateData m_regular;
    QThread m_thread;
    SatellitesChangedCallback m_satellitesChangedCallback;
    bool m_disableUpdates;
};

#endif // GYPSYMOCK_H
