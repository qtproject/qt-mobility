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

#ifndef QGALLERYURLREQUEST_H
#define QGALLERYURLREQUEST_H

#include "qgalleryabstractrequest.h"

#include <QtCore/qurl.h>

QTM_BEGIN_NAMESPACE

class QGalleryItemList;

class QGalleryUrlRequestPrivate;

class Q_GALLERY_EXPORT QGalleryUrlRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_PROPERTY(QUrl itemUrl READ itemUrl WRITE setItemUrl)
    Q_PROPERTY(bool create READ create WRITE setCreate)
    Q_PROPERTY(QString itemId READ itemId NOTIFY itemChanged)
    Q_PROPERTY(QString itemType READ itemType NOTIFY itemChanged)
public:
    QGalleryUrlRequest(QObject *parent = 0);
    QGalleryUrlRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryUrlRequest();

    QUrl itemUrl() const;
    void setItemUrl(const QUrl &url);

    bool create() const;
    void setCreate(bool create);

    // Results

    QString itemId() const;
    QString itemType() const;

Q_SIGNALS:
    void itemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);

private:
    Q_DECLARE_PRIVATE(QGalleryUrlRequest)
    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted(int, int))
};

QTM_END_NAMESPACE

#endif
