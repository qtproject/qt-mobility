/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEORGANIZERCOLLECTION_H
#define QDECLARATIVEORGANIZERCOLLECTION_H

#include "qdeclarative.h"
#include "qorganizercollection.h"

#include <QColor>
#include <QUrl>

QTM_USE_NAMESPACE


class QDeclarativeOrganizerCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString collectionId READ id WRITE setId NOTIFY valueChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY valueChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY valueChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY valueChanged)
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY valueChanged)
public:
    QDeclarativeOrganizerCollection(QObject* parent = 0)
        :QObject(parent)
    {
    }


    QString id() const
    {
        return d.id().toString();
    }

    void setId(const QString& newId)
    {
        d.setId(QOrganizerCollectionId::fromString(newId));
    }

    QString name() const
    {
        return metaData(QOrganizerCollection::KeyName).toString();
    }

    void setName(const QString& name)
    {
        setMetaData(QOrganizerCollection::KeyName, name);
    }

    QString description() const
    {
        return metaData(QOrganizerCollection::KeyDescription).toString();
    }

    void setDescription(const QString& desc)
    {
        setMetaData(QOrganizerCollection::KeyDescription, desc);
    }

    QColor color() const
    {
        return metaData(QOrganizerCollection::KeyColor).value<QColor>();
    }

    void setColor(const QColor& color)
    {
        setMetaData(QOrganizerCollection::KeyColor, color);
    }

    QUrl image() const
    {
        //image or image url?
        return QUrl(metaData(QOrganizerCollection::KeyImage).toString());
    }

    void setImage(const QUrl& url)
    {
        setMetaData(QOrganizerCollection::KeyImage, url);
    }

    Q_INVOKABLE void setMetaData(const QString& key, const QVariant& value)
    {
        if (metaData(key) != value) {
            d.setMetaData(key, value);
            emit valueChanged();
        }
    }

    Q_INVOKABLE  QVariant metaData(const QString& key) const
    {
        return d.metaData(key);
    }

    QOrganizerCollection collection() const
    {
        return d;
    }

    void setCollection(const QOrganizerCollection& collection)
    {
        d = collection;
    }
signals:
    void valueChanged();
private:
    QOrganizerCollection d;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerCollection)
#endif

