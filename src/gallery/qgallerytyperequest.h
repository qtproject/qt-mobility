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

#ifndef QGALLERYTYPEREQUEST_H
#define QGALLERYTYPEREQUEST_H

#include <qgalleryabstractrequest.h>

#include <qgalleryproperty.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilter;
class QGalleryResource;
class QGalleryResultSet;

class QGalleryTypeRequestPrivate;

class Q_GALLERY_EXPORT QGalleryTypeRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_PROPERTY(QStringList propertyNames READ propertyNames WRITE setPropertyNames NOTIFY propertyNamesChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY typeChanged)
public:
    explicit QGalleryTypeRequest(QObject *parent = 0);
    explicit QGalleryTypeRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryTypeRequest();

    QStringList propertyNames() const;
    void setPropertyNames(const QStringList &names);

    bool autoUpdate() const;
    void setAutoUpdate(bool enabled);

    QString itemType() const;
    void setItemType(const QString &itemType);

    QGalleryResultSet *resultSet() const;

    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;
    QVariant::Type propertyType(int key) const;

    bool isValid() const;

    QVariant metaData(int key) const;
    QVariant metaData(const QString &property) const;

Q_SIGNALS:
    void propertyNamesChanged();
    void autoUpdateChanged();
    void itemTypeChanged();
    void resultSetChanged(QGalleryResultSet *resultSet);
    void metaDataChanged(const QList<int> &keys);
    void typeChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);

private:
    Q_DECLARE_PRIVATE(QGalleryTypeRequest)
    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsRemoved(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsMoved(int, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_currentItemChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_metaDataChanged(int, int, const QList<int> &))
};

QTM_END_NAMESPACE

#endif
