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

#ifndef QGALLERYFILTERREQUEST_H
#define QGALLERYFILTERREQUEST_H

#include "qgalleryabstractrequest.h"

#include "qgalleryfilter.h"

QTM_BEGIN_NAMESPACE

class QGalleryItemList;

class QGalleryFilterRequestPrivate;

class Q_GALLERY_EXPORT QGalleryFilterRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryFilterRequest)
    Q_PROPERTY(QStringList propertyNames READ propertyNames WRITE setPropertyNames)
    Q_PROPERTY(QStringList sortPropertyNames READ sortPropertyNames WRITE setSortPropertyNames)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(int initialCursorPosition READ initialCursorPosition WRITE setInitialCursorPosition)
    Q_PROPERTY(int minimumPagedItems READ minimumPagedItems WRITE setMinimumPagedItems)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QVariant containerId READ containerId WRITE setContainerId)
    Q_PROPERTY(QGalleryItemList* items READ items NOTIFY itemsChanged)
public:
    explicit QGalleryFilterRequest(QObject *parent = 0);
    explicit QGalleryFilterRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryFilterRequest();

    QStringList propertyNames() const;
    void setPropertyNames(const QStringList &names);

    QStringList sortPropertyNames() const;
    void setSortPropertyNames(const QStringList &names);

    bool isLive() const;
    void setLive(bool live);

    int initialCursorPosition() const;
    void setInitialCursorPosition(int index);

    int minimumPagedItems() const;
    void setMinimumPagedItems(int size);

    QString itemType() const;
    void setItemType(const QString &type);

    QVariant containerId() const;
    void setContainerId(const QVariant &id);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    QGalleryItemList *items() const;

Q_SIGNALS:
    void itemsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

QTM_END_NAMESPACE

#endif
