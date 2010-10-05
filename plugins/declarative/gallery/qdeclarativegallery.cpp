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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include <qdocumentgallery.h>

#include "qdeclarativedocumentgallery.h"
#include "qdeclarativegalleryfilter.h"
#include "qdeclarativegalleryitem.h"
#include "qdeclarativegalleryquerymodel.h"
#include "qdeclarativegallerytype.h"

QTM_BEGIN_NAMESPACE

class QGalleryDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.gallery"));

        qRegisterMetaType<QDeclarativeDocumentGallery::ItemType>();

        qmlRegisterUncreatableType<QDeclarativeDocumentGallery>(uri, 1, 1, "DocumentGallery", QDeclarativeDocumentGallery::tr("DocumentGallery is a namespace class"));
        qmlRegisterType<QDeclarativeGalleryFilterBase>();
        qmlRegisterType<QDeclarativeGalleryEqualsFilter>(uri, 1, 1, "GalleryEqualsFilter");
        qmlRegisterType<QDeclarativeGalleryLessThanFilter>(uri, 1, 1, "GalleryLessThanFilter");
        qmlRegisterType<QDeclarativeGalleryLessThanEqualsFilter>(uri, 1, 1, "GalleryLessThanEqualsFilter");
        qmlRegisterType<QDeclarativeGalleryGreaterThanFilter>(uri, 1, 1, "GalleryGreaterThanFilter");
        qmlRegisterType<QDeclarativeGalleryGreaterThanEqualsFilter>(uri, 1, 1, "GalleryGreaterThanEqualsFilter");
        qmlRegisterType<QDeclarativeGalleryContainsFilter>(uri, 1, 1, "GalleryContainsFilter");
        qmlRegisterType<QDeclarativeGalleryStartsWithFilter>(uri, 1, 1, "GalleryStartsWithFilter");
        qmlRegisterType<QDeclarativeGalleryEndsWithFilter>(uri, 1, 1, "GalleryEndsWithFilter");
        qmlRegisterType<QDeclarativeGalleryWildcardFilter>(uri, 1, 1, "GalleryWildcardFilter");
        qmlRegisterType<QDeclarativeGalleryFilterUnion>(uri, 1, 1, "GalleryFilterUnion");
        qmlRegisterType<QDeclarativeGalleryFilterIntersection>(uri, 1, 1, "GalleryFilterIntersection");
        qmlRegisterType<QDeclarativeDocumentGalleryItem>(uri, 1, 1, "DocumentGalleryItem");
        qmlRegisterType<QDeclarativeDocumentGalleryModel>(uri, 1, 1, "DocumentGalleryModel");
        qmlRegisterType<QDeclarativeDocumentGalleryType>(uri, 1, 1, "DocumentGalleryType");
    }
};

#include "qdeclarativegallery.moc"

QTM_END_NAMESPACE

Q_EXPORT_PLUGIN2(qgallerydeclarativemodule, QTM_PREPEND_NAMESPACE(QGalleryDeclarativeModule));
