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

#ifndef QGALLERYQUERYREQUEST_H
#define QGALLERYQUERYREQUEST_H

#include <qgalleryabstractrequest.h>

#include <qgalleryproperty.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilter;
class QGalleryResource;
class QGalleryResultSet;

class QGalleryQueryRequestPrivate;

class Q_GALLERY_EXPORT QGalleryQueryRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryQueryRequest)
    Q_PROPERTY(QStringList propertyNames READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(QStringList sortPropertyNames READ sortPropertyNames WRITE setSortPropertyNames NOTIFY sortPropertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(QString rootType READ rootType WRITE setRootType NOTIFY rootTypeChanged)
    Q_PROPERTY(QVariant rootItem READ rootItem WRITE setRootItem NOTIFY rootItemChanged)
    Q_PROPERTY(QGalleryQueryRequest::Scope scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY currentItemChanged)
    Q_PROPERTY(QVariant itemId READ itemId NOTIFY currentItemChanged)
    Q_PROPERTY(QString itemType READ itemType NOTIFY currentItemChanged)
    Q_PROPERTY(QUrl itemUrl READ itemUrl NOTIFY currentItemChanged)
    Q_PROPERTY(QList<QGalleryResource> resources READ resources NOTIFY currentItemChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE seek NOTIFY currentItemChanged)
    Q_ENUMS(Scope)
public:
    enum Scope
    {
        AllDescendants,
        DirectDescendants
    };

    explicit QGalleryQueryRequest(QObject *parent = 0);
    explicit QGalleryQueryRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryQueryRequest();

    QStringList propertyNames() const;
    void setPropertyNames(const QStringList &names);

    QStringList sortPropertyNames() const;
    void setSortPropertyNames(const QStringList &names);

    bool autoUpdate() const;
    void setAutoUpdate(bool enabled);

    int offset() const;
    void setOffset(int offset);

    int limit() const;
    void setLimit(int limit);

    QString rootType() const;
    void setRootType(const QString &itemType);

    QVariant rootItem() const;
    void setRootItem(const QVariant &itemId);

    QGalleryQueryRequest::Scope scope() const;
    void setScope(QGalleryQueryRequest::Scope scope);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    QGalleryResultSet *resultSet() const;

    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;
    QVariant::Type propertyType(int key) const;

    int itemCount() const;

    bool isValid() const;

    QVariant itemId() const;
    QUrl itemUrl() const;
    QString itemType() const;
    QList<QGalleryResource> resources() const;

    QVariant metaData(int key) const;
    bool setMetaData(int key, const QVariant &value);

    QVariant metaData(const QString &property) const;
    bool setMetaData(const QString &property, const QVariant &value);

    int currentIndex() const;
    bool seek(int index, bool relative = false);
    bool next();
    bool previous();
    bool first();
    bool last();

Q_SIGNALS:
    void propertyNamesChanged();
    void sortPropertyNamesChanged();
    void autoUpdateChanged();
    void offsetChanged();
    void limitChanged();
    void rootTypeChanged();
    void rootItemChanged();
    void scopeChanged();
    void filterChanged();
    void resultSetChanged(QGalleryResultSet *resultSet);
    void currentItemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

QTM_END_NAMESPACE

#endif
