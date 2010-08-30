/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomappingmanager.h"
#include "qgeomappingmanager_p.h"
#include "qgeomappingmanagerengine.h"

#include <QNetworkProxy>
#include <QLocale>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManager
    \brief The QGeoMappingManager class provides support for displaying
    and interacting with maps.

    \inmodule QtLocation

    \ingroup maps-mapping

    A QGeoMappingManager instance can create QGeoMapData instances with
    createMapData(). These instances can be used to contain and manage
    information concerning what a particular QGeoMapWidget is viewing.

    The functions in this class will typically not be used by clients of this
    API, as the most common uses will only need to obtain a QGeoMappingManager
    instance and associate it with a QGeoMapWidget instance:
    \code
        QGeoServiceProvider serviceProvider("nokia");
        QGeoMappingManager *manager = serviceProvider.mappingManager();
        QGeoMapWidget *widget = new QGeoMapWidget(manager);
    \endcode
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used internally by QGeoServiceProviderFactory. Regular
    users should aquire instance of QGeoMappingManager with
    QGeoServiceProvider::mappingManager();
*/
QGeoMappingManager::QGeoMappingManager(QGeoMappingManagerEngine *engine, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoMappingManagerPrivate)
{
    d_ptr->engine = engine;
    if (d_ptr->engine) {
        d_ptr->engine->setParent(this);
    } else {
        qFatal("The mapping manager engine that was set for this mapping manager was NULL.");
    }
}

/*!
    Destroys this QGeoMappingManager object.
*/
QGeoMappingManager::~QGeoMappingManager()
{
    delete d_ptr;
}

/*!
    Returns the name of the engine which implements the behaviour of this
    mapping manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
QString QGeoMappingManager::managerName() const
{
//    if (!d_ptr->engine)
//        return QString();

    return d_ptr->engine->managerName();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    mapping manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoMappingManager::managerVersion() const
{
//    if (!d_ptr->engine)
//        return -1;

    return d_ptr->engine->managerVersion();
}

/*!
    Returns a new QGeoMapData instance for \a graphicsItem which will be managed by this manager.
*/
QGeoMapData* QGeoMappingManager::createMapData(QGraphicsGeoMap *geoMap)
{
//    if (!d_ptr->engine)
//        return 0;

    return d_ptr->engine->createMapData(geoMap);
}

///*!
//    Updates the map image stored in \a mapData based on the viewport
//    data contained within \a mapData.

//    The image may be updated incrementally, as will happen with
//    tile based mapping managers.
//*/
//void QGeoMappingManager::updateMapImage(QGeoMapData *mapData)
//{
////    if (d_ptr->engine)
//        d_ptr->engine->updateMapImage(mapData);
//}

/*!
    Returns a list of the map types supported by this manager.
*/
QList<QGraphicsGeoMap::MapType> QGeoMappingManager::supportedMapTypes() const
{
//    if (!d_ptr->engine)
//        return QList<QGeoMapWidget::MapType>();

    return d_ptr->engine->supportedMapTypes();
}

/*!
    Returns the minimum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::minimumZoomLevel() const
{
//    // TODO document this behaviour
//    if (!d_ptr->engine)
//        return -1.0;

    return d_ptr->engine->minimumZoomLevel();
}

/*!
    Returns the maximum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::maximumZoomLevel() const
{
//    // TODO document this behaviour
//    if (!d_ptr->engine)
//        return -1.0;

    return d_ptr->engine->maximumZoomLevel();
}

/*!
    Returns the size of the smallest map image which is supported by this
    manager.

    An invalid size indicates that this manager places
    no restrictions on the minimum size of the map image.
*/
QSize QGeoMappingManager::minimumImageSize() const
{
//    if (!d_ptr->engine)
//        return QSize();

    return d_ptr->engine->minimumImageSize();
}

/*!
    Returns the size of the largest map image which is supported by this
    manager.

    An invalid size indicates that this manager places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManager::maximumImageSize() const
{
//    if (!d_ptr->engine)
//        return QSize();

    return d_ptr->engine->maximumImageSize();
}

/*!
*/
void QGeoMappingManager::setLocale(const QLocale &locale)
{
    d_ptr->engine->setLocale(locale);
}

/*!
*/
QLocale QGeoMappingManager::locale() const
{
    return d_ptr->engine->locale();
}

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate()
        : engine(0) {}

QGeoMappingManagerPrivate::~QGeoMappingManagerPrivate()
{
    if (engine)
        delete engine;
}

#include "moc_qgeomappingmanager.cpp"

QTM_END_NAMESPACE
