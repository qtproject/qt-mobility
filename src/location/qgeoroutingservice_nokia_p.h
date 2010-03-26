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

#ifndef QGEOROUTINGSERVICE_NOKIA_P_H
#define QGEOROUTINGSERVICE_NOKIA_P_H

#include "qgeoroutingservice.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>

QTM_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QGeoRoutingServiceNokia : public QGeoRoutingService
{
    Q_OBJECT
public:
    QGeoRoutingServiceNokia();
    ~QGeoRoutingServiceNokia();

    QString token() const;
    void setToken(const QString &token);

    QString referrer() const;
    void setReferrer(const QString &referrer);

    QString host() const;
    void setHost(const QString &host);

    QNetworkProxy proxy() const;
    void setProxy(const QNetworkProxy &proxy);

    virtual QGeoRouteReply* getRoute(const QGeoRouteRequest& request);

private slots:
    void finishedReply();
    void errorReply(QGeoRouteReply::ErrorCode errorCode, QString errorString);

private:
    QString m_token;
    QString m_referrer;
    QString m_host;
    QNetworkProxy m_proxy;
    QNetworkAccessManager m_nam;
};

QTM_END_NAMESPACE

#endif // QGEOROUTINGSERVICE_NOKIA_P_H
