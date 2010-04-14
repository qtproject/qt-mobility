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

#ifndef QGALLERYITEMREQUEST_H
#define QGALLERYITEMREQUEST_H

#include "qgalleryabstractrequest.h"

class QGalleryItemList;

class QGalleryItemRequestPrivate;

class Q_GALLERY_EXPORT QGalleryItemRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryItemRequest)
    Q_PROPERTY(QStringList propertyNames READ propertyNames WRITE setPropertyNames)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged)
    Q_PROPERTY(QGalleryItemList* item READ item NOTIFY itemChanged)
public:
    explicit QGalleryItemRequest(QObject *parent = 0);
    explicit QGalleryItemRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryItemRequest();

    QStringList propertyNames() const;
    void setPropertyNames(const QStringList &names);

    bool isLive() const;
    void setLive(bool live);

    QString itemId() const;
    void setItemId(const QString &id);

    QGalleryItemList *item() const;

Q_SIGNALS:
    void itemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

#endif
