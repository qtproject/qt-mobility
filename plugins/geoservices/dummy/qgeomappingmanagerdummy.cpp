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

#include "qgeomappingmanagerdummy.h"
#include <QGeoCoordinate>

QGeoMappingManagerDummy::QGeoMappingManagerDummy()
        : QGeoMappingManager()
{
}

QGeoMappingManagerDummy::~QGeoMappingManagerDummy()
{
}

QGeoMapReply* QGeoMappingManagerDummy::getMapImage(const QGeoCoordinate &center, qreal zoomLevel,
    const QSize &size, const QGeoMapRequestOptions &requestOptions)
{
    QGeoMapReplyDummy* reply = new QGeoMapReplyDummy(size);
    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(mappingFinished()));

    return reply;
}

QGeoMapReply* QGeoMappingManagerDummy::getTileImage(qint32 row, qint32 col, qint32 zoomLevel,
    const QSize &size, const QGeoMapRequestOptions &requestOptions)
{
    QGeoMapReplyDummy* reply = new QGeoMapReplyDummy(size);
    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(mappingFinished()));

    return reply;
}

void QGeoMappingManagerDummy::getTileQuadKey(const QGeoCoordinate& coordinate,
                                qint32 zoomLevel,
                                qint32* row, qint32* col)
{
}

void QGeoMappingManagerDummy::mappingFinished()
{
    QGeoMapReply *reply = qobject_cast<QGeoMapReply*>(sender());
    if (!reply)
        return;

    if(receivers(SIGNAL(finished(QGeoMapReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoMappingManagerDummy::mappingError(QGeoMapReply::Error error, const QString &errorString)
{
    QGeoMapReply *reply = qobject_cast<QGeoMapReply*>(sender());
    if (!reply)
        return;
    
    if(receivers(SIGNAL(finished(QGeoPlacesReply*, QGeoPlacesReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

