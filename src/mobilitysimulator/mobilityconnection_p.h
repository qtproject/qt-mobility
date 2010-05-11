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

#ifndef QSIMULATORMOBILITY_H
#define QSIMULATORMOBILITY_H

#include "qmobilityglobal.h"
#include "mobilitysimulatorglobal.h"

#include <QtCore/QObject>
#include <QtCore/QVector>

QT_BEGIN_HEADER

#define SIMULATOR_MOBILITY_SERVERNAME "QtSimulator_Mobility_ServerName"

inline QString qt_mobilityServerName(qint64 pid)
{
    QString share = QLatin1String(SIMULATOR_MOBILITY_SERVERNAME);
    share += QString::number(pid);
    return share;
}

class QLocalSocket;
class QLocalServer;

QTM_BEGIN_NAMESPACE

namespace Simulator {

class Q_MOBILITYSIMULATOR_EXPORT MobilityConnection : public QObject
{
    Q_OBJECT
public:
    MobilityConnection(QObject *parent = 0);
    ~MobilityConnection();

    static MobilityConnection *instance();
    void connectToSimulator();

    void addMessageHandler(QObject *handler);
    QLocalSocket *sendSocket();
    QLocalSocket *receiveSocket();

public slots:
    void onReadyRead();

private:
    QLocalSocket *mSendSocket;
    QLocalSocket *mReceiveSocket;
    QByteArray mReadBuffer;
    QVector<QObject *> mHandlers;
};

} // end Simulator namespace

QTM_END_NAMESPACE
QT_END_HEADER

#endif // QSIMULATORMOBILITY_H
