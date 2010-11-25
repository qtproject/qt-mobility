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

#ifndef QGEOSERVICEPROVIDER_TEST_H
#define QGEOSERVICEPROVIDER_TEST_H

#include <qgeoserviceproviderfactory.h>
#include <qgeomappingmanagerengine.h>
#include <qgeosearchmanagerengine.h>
#include <qgeoroutingmanagerengine.h>
#include <qgeomapdata.h>
#include <QObject>

QTM_USE_NAMESPACE
class QGeoRouteReplyTest: public QGeoRouteReply
{
Q_OBJECT
public:
    QGeoRouteReplyTest(const QGeoRouteRequest &request, QObject *parent = 0) :
        QGeoRouteReply(request, parent)
    {
    }
};

class QGeoRoutingManagerEngineTest: public QGeoRoutingManagerEngine
{
Q_OBJECT
public:
    QGeoRoutingManagerEngineTest(const QMap<QString, QVariant> &parameters, QObject *parent = 0) :
        QGeoRoutingManagerEngine(parameters, parent)
    {
    }
    virtual QGeoRouteReply* calculateRoute(const QGeoRouteRequest& request)
    {
        return new QGeoRouteReplyTest(request);
    }
};

class QGeoMapDataTest: public QGeoMapData
{
Q_OBJECT
public:
    QGeoMapDataTest(QGeoMappingManagerEngine *engine) :
        QGeoMapData(engine)
    {
    }

    virtual QGeoBoundingBox viewport() const
    {
        return QGeoBoundingBox();
    }
    virtual void fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter = false)
    {
        if (!preserveViewportCenter)
            setCenter(bounds.center());
    }
    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
    {
        return QPointF(coordinate.longitude(), coordinate.latitude());
    }
    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const
    {
        return QGeoCoordinate(screenPosition.y(), screenPosition.x());
    }
};

class QGeoMappingManagerEngineTest: public QGeoMappingManagerEngine
{
Q_OBJECT
public:
    QGeoMappingManagerEngineTest(const QMap<QString, QVariant> &parameters, QObject *parent = 0) :
        QGeoMappingManagerEngine(parameters, parent)
    {
    }
    virtual QGeoMapData* createMapData()
    {
        return new QGeoMapDataTest(this);
    }
};

class QGeoServiceProviderFactoryTest2: public QObject, public QGeoServiceProviderFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QGeoServiceProviderFactory)
public:
    QGeoServiceProviderFactoryTest2();
    ~QGeoServiceProviderFactoryTest2();

    QString providerName() const;
    int providerVersion() const;

    virtual QGeoSearchManagerEngine* createSearchManagerEngine(
        const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error,
        QString *errorString) const
    {
        return new QGeoSearchManagerEngine(parameters);
    }
    virtual QGeoMappingManagerEngine* createMappingManagerEngine(
        const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error,
        QString *errorString) const
    {
        return new QGeoMappingManagerEngineTest(parameters);
    }
    virtual QGeoRoutingManagerEngine* createRoutingManagerEngine(
        const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error,
        QString *errorString) const
    {
        return new QGeoRoutingManagerEngineTest(parameters);
    }

};

#endif
