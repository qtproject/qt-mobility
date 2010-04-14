/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgalleryerrorresponse_p.h"

QTM_BEGIN_NAMESPACE

QGalleryErrorResponse::QGalleryErrorResponse(int result, QObject *parent)
    : QGalleryAbstractResponse(parent)
{
    finish(result);
}

QGalleryErrorResponse::~QGalleryErrorResponse()
{
}

QStringList QGalleryErrorResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryErrorResponse::propertyKey(const QString &) const
{
    return -1;
}
QGalleryProperty::Attributes QGalleryErrorResponse::propertyAttributes(int key) const
{
    return QGalleryProperty::Attributes();
}

int QGalleryErrorResponse::count() const
{
    return 0;
}

QString QGalleryErrorResponse::id(int) const
{
    return QString();
}

QUrl QGalleryErrorResponse::url(int) const
{
    return QUrl();
}

QString QGalleryErrorResponse::type(int) const
{
    return QString();
}

QString QGalleryErrorResponse::parentId(int) const
{
    return QString();
}

QList<QGalleryResource> QGalleryErrorResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QVariant QGalleryErrorResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryErrorResponse::setMetaData(int, int, const QVariant &)
{
}

bool QGalleryErrorResponse::waitForFinished(int)
{
    return true;
}

#include "moc_qgalleryerrorresponse_p.cpp"

QTM_END_NAMESPACE

