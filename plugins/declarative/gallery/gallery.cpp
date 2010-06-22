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

#include "gallerycountrequest.h"
#include "galleryfilter.h"
#include "galleryitemrequest.h"
#include "galleryqueryrequest.h"

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QAbstractGallery))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDocumentGallery))

QTM_BEGIN_NAMESPACE

class QGalleryDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.gallery"));

        qmlRegisterType<GalleryFilterBase>();
        qmlRegisterType<GalleryFilterUnionItem>();
        qmlRegisterType<GalleryFilterIntersectionItem>();
        qmlRegisterType<GalleryFilter>(uri, 1, 0, "GalleryFilter");
        qmlRegisterType<GalleryFilterLessThan>(uri, 1, 0, "GalleryPropertyLessThan");
        qmlRegisterType<GalleryFilterLessThanEquals>(uri, 1, 0, "GalleryPropertyLessThanEquals");
        qmlRegisterType<GalleryFilterGreaterThan>(uri, 1, 0, "GalleryPropertyGreaterThan");
        qmlRegisterType<GalleryFilterGreaterThanEquals>(uri, 1, 0, "GalleryPropertyGreaterThanEquals");
        qmlRegisterType<GalleryFilterExclusiveRange>(uri, 1, 0, "GalleryExclusivePropertyRange");
        qmlRegisterType<GalleryFilterInclusiveRange>(uri, 1, 0, "GalleryInclusivePropertyRange");
        qmlRegisterType<GalleryFilterUnion>(uri, 1, 0, "GalleryFilterUnion");
        qmlRegisterType<GalleryFilterIntersection>(uri, 1, 0, "GalleryFilterIntersection");
        qmlRegisterType<GalleryCountRequest>(uri, 1, 0, "GalleryCountQuery");
        qmlRegisterType<GalleryItemRequest>(uri, 1, 0, "GalleryItem");
        qmlRegisterType<GalleryQueryRequest>(uri, 1, 0, "GalleryQuery");
        qmlRegisterType<QAbstractGallery>();
        qmlRegisterType<QDocumentGallery>(uri, 1, 0, "DocumentGallery");
    }
};

#include "gallery.moc"

QTM_END_NAMESPACE

Q_EXPORT_PLUGIN2(qgallerydeclarativemodule, QTM_PREPEND_NAMESPACE(QGalleryDeclarativeModule));
