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

#ifndef QNETWORKCONFIGURATIONPRIVATE_H
#define QNETWORKCONFIGURATIONPRIVATE_H

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

#include <qnetworkconfiguration.h>
#include <QtCore/qshareddata.h>

QTM_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate : public QSharedData
{
public:
    enum Bearer {
        BearerEthernet,
        BearerWLAN,
        Bearer2G,
        BearerCDMA2000,
        BearerWCDMA,
        BearerHSPA,
        BearerBluetooth,
        BearerWiMAX,
        BearerUnknown = -1
    };    

    QNetworkConfigurationPrivate();
    ~QNetworkConfigurationPrivate();

    QString name;
    bool isValid;
    QString id;
    QNetworkConfiguration::StateFlags state;
    QNetworkConfiguration::Type type;
    bool roamingSupported;
    QNetworkConfiguration::Purpose purpose;

    QList<QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > serviceNetworkMembers;

    QNetworkConfigurationPrivate::Bearer bearer;
    QString bearerName() const;
    // So called IAP id from the platform. Remains constant as long as the
    // platform is aware of the configuration ie. it is stored in the databases
    // --> does not depend on whether connections are currently open or not.
    // In practice is the same for the lifetime of the QNetworkConfiguration.
    TUint32 numericId;
    // So called connection id, or connection monitor ID. A dynamic ID assigned
    // by RConnectionMonitor whenever a new connection is opened. ConnectionID and
    // numericId/IAP id have 1-to-1 mapping during the lifetime of the connection at
    // connection monitor. Notably however it changes whenever a new connection to
    // a given IAP is created. In a sense it is constant during the time the
    // configuration remains between states Discovered..Active..Discovered, do not
    // however relay on this.
    TUint connectionId;
    
    TAny* manager;
 
    QString mappingName;
 
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> serviceNetworkPtr; 

private:
    // disallow detaching
    QNetworkConfigurationPrivate &operator=(const QNetworkConfigurationPrivate &other);
    QNetworkConfigurationPrivate(const QNetworkConfigurationPrivate &other);
};

QTM_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONPRIVATE_H

