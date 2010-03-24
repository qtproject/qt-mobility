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

#include "qgallerycountresponse_p.h"

namespace QWindowsSearch
{

QGalleryCountResponse::QGalleryCountResponse(IRowsetScroll *rowSet, QObject *parent)
    : QGalleryRowSetResponse(rowSet, parent)
    , m_count(0)
{
}

QGalleryCountResponse::~QGalleryCountResponse()
{
}

QList<int> QGalleryCountResponse::keys() const
{
    return QList<int>();
}

QString QGalleryCountResponse::toString(int) const
{
    return QString();
}

int QGalleryCountResponse::count() const
{
    return m_count;
}

QString QGalleryCountResponse::id(int) const
{
    return QString();
}

QUrl QGalleryCountResponse::url(int) const
{
    return QUrl();
}

QString QGalleryCountResponse::type(int) const
{
    return QString();
}

QList<QGalleryResource> QGalleryCountResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QVariant QGalleryCountResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryCountResponse::setMetaData(int, int, const QVariant &)
{
}

QGalleryItemList::MetaDataFlags QGalleryCountResponse::metaDataFlags(int, int) const
{
    return 0;
}

void QGalleryCountResponse::customEvent(QEvent *event)
{
    if (event->type() == QGalleryEvent::ItemsInserted) {
        QGalleryItemsInsertedEvent *insertEvent
                = static_cast<QGalleryItemsInsertedEvent *>(event);
        int index = m_count;

        m_count += insertEvent->count;

        emit inserted(index, insertEvent->count);
    } else {
        QGalleryRowSetResponse::customEvent(event);
    }
}

}
