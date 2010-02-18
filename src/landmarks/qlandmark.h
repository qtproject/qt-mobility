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

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmarkCategoryPrivate;
class QLandmarkPrivate;
class QLandmarkIdPrivate;
class QLandmarkCategoryPrivate;
class QLandmarkCategoryIdPrivate;

class Q_LANDMARKS_EXPORT QLandmarkCategoryId
{
public:
    QLandmarkCategoryId();
    QLandmarkCategoryId(const QLandmarkCategoryId &other);
    ~QLandmarkCategoryId();

    bool isValid();
    QString id();
    void setId(const QString &id);

    QString databaseName();
    void setDatabaseName(const QString &databaseName);//maybe better to use database uri than name?

    QLandmarkCategoryId &operator=(const QLandmarkCategoryId &other) const;
    bool operator==(const QLandmarkCategoryId &other) const;
    bool operator!=(const QLandmarkCategoryId &other) const;

private:
    QSharedDataPointer<QLandmarkCategoryIdPrivate> d;
};

class Q_LANDMARKS_EXPORT QLandmarkCategory
{
public:
    QLandmarkCategory();
    QLandmarkCategory(const QLandmarkCategory &other);
    ~QLandmarkCategory();

    QLandmarkCategory& operator= (const QLandmarkCategory &other);

    bool operator== (const QLandmarkCategory &other) const;
    // consider inline != in terms of ==?

    QString name() const;
    void setName(const QString &name);

    QString icon() const;
    void setIcon(const QString &fileName);

    QString description() const;
    void setDescription(const QString &description);

    bool isReadOnly() const;

    QLandmarkCategoryId categoryId() const;

    //TODO: see if generic attributes are needed
    //for categories, doesn't appear
    //to be a way to do this for S60 and maemo
private:
    QLandmarkCategoryPrivate *d;
};

class Q_LANDMARKS_EXPORT QLandmarkId
{
public:
    QLandmarkId();
    QLandmarkId(const QLandmarkId &id);
    ~QLandmarkId();

    bool isValid();
    void setId(const QString &id);
    QString id();

    QString databaseName();
    void setDatabaseName(const QString &databaseName);

    QLandmarkId &operator=(const QLandmarkId &other) const;
    bool operator==(const QLandmarkId &other) const;
    bool operator!=(const QLandmarkId &other) const;
private:
    QSharedDataPointer<QLandmarkIdPrivate> d;
};

class Q_LANDMARKS_EXPORT QLandmark
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
    void setCategories(const QList<QLandmarkCategoryId> &categories);
    void addCategory(const QLandmarkCategoryId &category);
    void removeCategory(const QLandmarkCategoryId &category);

    QString description() const;
    void setDescription(const QString &description);

    QString icon() const;
    void setIcon(const QString &iconFileName);

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
