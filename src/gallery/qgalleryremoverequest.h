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

#ifndef QGALLERYREMOVEREQUEST_H
#define QGALLERYREMOVEREQUEST_H

#include <qgalleryabstractrequest.h>

#include <QtCore/qvariant.h>

QTM_BEGIN_NAMESPACE

class QGalleryItemList;

class QGalleryRemoveRequestPrivate;

class Q_GALLERY_EXPORT QGalleryRemoveRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryRemoveRequest)
    Q_PROPERTY(QStringList propertyNames READ propertyNames WRITE setPropertyNames)
    Q_PROPERTY(QVariant itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged);
    Q_PROPERTY(QVariantList itemIds READ itemIds WRITE setItemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QGalleryItemList *currentItem READ currentItem NOTIFY currentItemChanged)
public:
    explicit QGalleryRemoveRequest(QObject *parent = 0);
    explicit QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryRemoveRequest();

    QStringList propertyNames() const;
    void setPropertyNames(const QStringList &names);

    QVariant itemId() const;
    void setItemId(const QVariant &id);

    QVariantList itemIds() const;
    void setItemIds(const QVariantList &id);

    QGalleryItemList *currentItem() const;

Q_SIGNALS:
    void itemIdsChanged();
    void currentItemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

QTM_END_NAMESPACE

#endif
