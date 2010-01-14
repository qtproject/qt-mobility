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

#ifndef QLANDMARK_H
#define QLANDMARK_H

#include <QGeoCoordinate>

class LandmarkCategory
{
    QString name() const;
    void setName(const QString &);

    QString iconFileName() const;
    void setIconFileName(const QString &fileName);
}

class QLandmark:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName);
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate);
    Q_PROPERTY(QStringList categories READ categories WRITE setCategories);
    Q_PROPERTY(QString description READ description WRITE setDescription);
    Q_PROPERTY(QString READ iconFileName WRITE setIconFileName);
    Q_PROPERTY(QContact contact READ contact WRITE setContact);
    Q_PROPERTY(QVariant attribute READ attribute WRITE setAttribute);

public:
    QString name() const;
    void setName(const QString &name);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate& coordinate);

    QList<LandmarkCategories> categories() const;
    void setCategories(const QList<LandmarkCategory> &categories);
    void addCategory(const QCategory &category);
    void removeCategory(const QCategory &category);

    QString description() const;
    void setDescription(const QString &description);

    QString iconFileName() const;
    void setIconFileName(const QString &iconFileName);

    QContact contact() const;
    void setContact(const QContact &);

    QVariant attribute(const QString &attributeName) const;
    void setAttribute(const QString &attributeName, const QVariant &value);
};

#endif
