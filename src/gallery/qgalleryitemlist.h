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

#ifndef QGALLERYITEMLIST_H
#define QGALLERYITEMLIST_H

#include <qmobilityglobal.h>

#include <QtCore/qmap.h>
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

#include <qgalleryproperty.h>

QTM_BEGIN_NAMESPACE

class Q_GALLERY_EXPORT QGalleryResource
{
public:
    QGalleryResource();
    QGalleryResource(const QUrl &url);
    QGalleryResource(const QUrl &url, QMap<int, QVariant> attributes);
    QGalleryResource(const QGalleryResource &resource);
    ~QGalleryResource();

    QGalleryResource &operator =(const QGalleryResource &resource);

    bool operator ==(const QGalleryResource &resource) const;
    bool operator !=(const QGalleryResource &resource) const;

    QUrl url() const;
    QMap<int, QVariant> attributes() const;

    QVariant attribute(int key) const;

private:
    QUrl m_url;
    QMap<int, QVariant> m_attributes;
};

class QGalleryItemListPrivate;

class Q_GALLERY_EXPORT QGalleryItemList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition)
    Q_PROPERTY(int minimumPagedItems READ minimumPagedItems)
    Q_DECLARE_PRIVATE(QGalleryItemList)
public:
    enum ItemStatusFlag
    {
        OutOfRange = 0x01,
        Reading = 0x02,
        Writing = 0x04
    };

    Q_DECLARE_FLAGS(ItemStatus, ItemStatusFlag);

    QGalleryItemList(QObject *parent = 0);
    ~QGalleryItemList();

    virtual QStringList propertyNames() const = 0;
    virtual int propertyKey(const QString &property) const = 0;
    virtual QGalleryProperty::Attributes propertyAttributes(int key) const = 0;
    virtual QVariant::Type propertyType(int key) const = 0;

    int cursorPosition() const;
    virtual int minimumPagedItems() const;

    virtual int count() const = 0;

    virtual QVariant id(int index) const = 0;
    virtual QUrl url(int index) const = 0;
    virtual QString type(int index) const = 0;
    virtual QList<QGalleryResource> resources(int index) const = 0;
    virtual ItemStatus status(int index) const = 0;

    virtual QVariant metaData(int index, int key) const = 0;
    virtual void setMetaData(int index, int key, const QVariant &value) = 0;

    virtual QVariant metaData(int index, const QString &property) const;
    virtual void setMetaData(int index, const QString &property, const QVariant &value);

public Q_SLOTS:
    virtual void setCursorPosition(int position);

Q_SIGNALS:
    void inserted(int index, int count);
    void removed(int index, int count);
    void moved(int from, int to, int count);
    void statusChanged(int index, int count);

    void metaDataChanged(int index, int count, const QList<int> &keys = QList<int>());

protected:
    QGalleryItemList(QGalleryItemListPrivate &dd, QObject *parent);

    QScopedPointer<QGalleryItemListPrivate> d_ptr;
};

QTM_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(QTM_PREPEND_NAMESPACE(QGalleryItemList::ItemStatus))

#endif
