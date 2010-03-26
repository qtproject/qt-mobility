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

#ifndef QLOCATION_GEONETWORKMANAGERPRIVATE_P_H
#define QLOCATION_GEONETWORKMANAGERPRIVATE_P_H

#include <QString>
#include <QHash>
#include <QNetworkProxy>
#include <QNetworkAccessManager>

#include "qgeonetworkmanager.h"
#include "qgeomaptilecache.h"

QTM_BEGIN_NAMESPACE

class QGeoNetworkManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QGeoNetworkManagerPrivate(QGeoNetworkManager *parent);
    ~QGeoNetworkManagerPrivate();

    virtual QGeoRouteReply* get(const QGeoRouteRequest& request);
    virtual QGeocodingReply* get(const QGeocodingRequest& request);
    virtual QGeocodingReply* get(const QReverseGeocodingRequest& request);
    virtual QGeoMapTileReply* get(const QGeoMapTileRequest& request);

    QNetworkAccessManager netManager; //!< The internal network manager
    QString geocdSrv;
    QString rtSrv;
    QString mapSrv;
    QNetworkProxy geocdProx;
    QNetworkProxy rtProx;
    QNetworkProxy mapProx;
    QString token;
    QString referrer;
    QGeoMapTileCache cache; //!< The map tile cache

    QHash<QString, MapVersion> mapVersions;
    QHash<QString, MapResolution> mapResolutions;
    QHash<QString, MapFormat> mapFormats;
    QHash<QString, MapScheme> mapSchemes;

    QGeoNetworkManager * const q_ptr;
    Q_DECLARE_PUBLIC(QGeoNetworkManager);

private slots:
    void finishedRouteRequest();
    void finishedGeocodingRequest();
    void finishedMapTileRequest();

    void errorRouteRequest(QGeoRouteReply::ErrorCode errorCode, const QString &errorString = QString());
    void errorGeocodingRequest(QGeocodingReply::ErrorCode errorCode, const QString &errorString = QString());
    void errorMapTileRequest(QGeoMapTileReply::ErrorCode errorCode, const QString &errorString = QString());
};

QTM_END_NAMESPACE

#endif
