/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
