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

#ifndef QGEOTILEDMAPPINGMANAGERTHREAD_H
#define QGEOTILEDMAPPINGMANAGERTHREAD_H

#include "qgeomapwidget.h"
#include "qgeotiledmapreply.h"
#include "qgeotiledmaprequest.h"

#include <QThread>

QTM_BEGIN_NAMESPACE

class QGeoTiledMapData;
class QGeoTiledMappingManagerEngine;

class QGeoTiledMappingManagerThreadPrivate;

class Q_LOCATION_EXPORT QGeoTiledMappingManagerThread : public QThread
{
    Q_OBJECT
public:
    QGeoTiledMappingManagerThread(QGeoTiledMappingManagerEngine* engine, QObject *parent = 0);
    virtual ~QGeoTiledMappingManagerThread();

    virtual void initialize();

    virtual QGeoTiledMapReply* getTileImage(const QGeoTiledMapRequest &request) = 0;
    void removeMapData(QGeoTiledMapData* mapData);

protected:
    void run();

public slots:
    void setRequests(QGeoTiledMapData* mapData, const QList<QGeoTiledMapRequest> &requests);

signals:
    void tileFinished(QGeoTiledMapReply *reply);
    void tileError(QGeoTiledMapReply *reply, QGeoTiledMapReply::Error error, QString errorString);

private:
    QGeoTiledMappingManagerThreadPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoTiledMappingManagerThread)
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGeoTiledMapRequest))
Q_DECLARE_METATYPE(QList < QTM_PREPEND_NAMESPACE(QGeoTiledMapRequest >))
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGeoTiledMapReply::Error))

#endif
