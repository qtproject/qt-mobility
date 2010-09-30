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

#include "qdeclarativedocumentgallery.h"

#include <qgalleryabstractrequest.h>

#include <QtCore/qmetaobject.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QDocumentGallery, qt_declarativeDocumentGalleryInstance);

QString QDeclarativeDocumentGallery::toString(ItemType type)
{
    return type != InvalidType
            ? QString::fromLatin1(staticMetaObject.enumerator(0).valueToKey(type))
            : QString();
}

QDeclarativeDocumentGallery::ItemType QDeclarativeDocumentGallery::itemTypeFromString(
        const QString &string)
{
    const int key = staticMetaObject.enumerator(0).keyToValue(string.toLatin1().constData());

    return key != -1
            ? ItemType(key)
            : InvalidType;
}

QAbstractGallery *QDeclarativeDocumentGallery::gallery(QObject *object)
{
#ifndef QTM_BUILD_UNITTESTS
    Q_UNUSED(object);
#else
    if  (QDeclarativeContext *context = QDeclarativeEngine::contextForObject(object)) {
        if (QAbstractGallery *gallery = qobject_cast<QAbstractGallery *>(
                context->contextProperty(QLatin1String("qt_testGallery")).value<QObject *>())) {
            return gallery;
        }
    }
#endif
    return qt_declarativeDocumentGalleryInstance();
}

#include "moc_qdeclarativedocumentgallery.cpp"

QTM_END_NAMESPACE
