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

#include "qmobilityglobal.h"
#include "qgeocoordinate.h"

#include <QObject>
#include <QSharedDataPointer>

class QVariant;
class QStringList;
class QUrl;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmarkId;
class QLandmarkCategoryId;
class QLandmarkPrivate;
class Q_LOCATION_EXPORT QLandmark
{
public:
    QLandmark();
    QLandmark(const QLandmark &other);
    ~QLandmark();

    QLandmark& operator= (const QLandmark &other);

    bool operator== (const QLandmark &other) const;
    // consider inline != in terms of ==?

    QString name() const;
    void setName(const QString &name);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate& coordinate);

    QList<QLandmarkCategoryId> categories() const;
    void setCategories(const QList<QLandmarkCategoryId> &categoryIds);
    void addCategory(const QLandmarkCategoryId &categoryId);
    void removeCategory(const QLandmarkCategoryId &categoryId);

    QString description() const;
    void setDescription(const QString &description);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl &iconUrl);

    double radius() const;
    void setRadius(double radius);

    QVariant attribute(const QString &attributeName) const;
    void setAttribute(const QString &attributeName, const QVariant &value);
    QStringList attributes() const;

    QString street() const;
    void setStreet(const QString &street);

    QString locality() const;
    void setLocality(const QString &locality);

    QString region() const;
    void setRegion(const QString &region);

    QString country() const;
    void setCountry(const QString &country);

    QString postcode() const;
    void setPostcode(const QString &postCode);

    QString postOfficeBox() const;
    void setPostOfficeBox(const QString &postOfficeBox);

    QString phone() const;
    void setPhone(const QString &phone);

    QString url() const;
    void setUrl(const QString &url);

    QLandmarkId landmarkId();
private:
    QLandmarkPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
