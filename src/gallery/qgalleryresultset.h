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

#ifndef QGALLERYRESULTSET_H
#define QGALLERYRESULTSET_H

#include <qgalleryabstractresponse.h>
#include <qgalleryproperty.h>

#include <QtCore/qmap.h>
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QTM_BEGIN_NAMESPACE

class QGalleryResource;

class QGalleryResultSetPrivate;

class Q_GALLERY_EXPORT QGalleryResultSet : public QGalleryAbstractResponse
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryResultSet)
public:
    QGalleryResultSet(QObject *parent = 0);
    ~QGalleryResultSet();

    virtual int propertyKey(const QString &property) const = 0;
    virtual QGalleryProperty::Attributes propertyAttributes(int key) const = 0;
    virtual QVariant::Type propertyType(int key) const = 0;

    virtual int itemCount() const = 0;

    virtual bool isValid() const;

    virtual QVariant itemId() const = 0;
    virtual QUrl itemUrl() const = 0;
    virtual QString itemType() const = 0;
    virtual QList<QGalleryResource> resources() const;

    virtual QVariant metaData(int key) const = 0;
    virtual bool setMetaData(int key, const QVariant &value) = 0;

    virtual int currentIndex() const = 0;
    virtual bool fetch(int index) = 0;
    virtual bool fetchNext();
    virtual bool fetchPrevious();
    virtual bool fetchFirst();
    virtual bool fetchLast();

Q_SIGNALS:
    void currentItemChanged();
    void currentIndexChanged(int index);
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);

    void metaDataChanged(int index, int count, const QList<int> &keys);

protected:
    QGalleryResultSet(QGalleryResultSetPrivate &dd, QObject *parent);
};

QTM_END_NAMESPACE

#endif
