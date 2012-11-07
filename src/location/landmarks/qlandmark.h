/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QLANDMARK_H
#define QLANDMARK_H

#include "qmobilityglobal.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>
#include <QStringList>
#include <QUrl>
#include "qgeoplace.h"
#include "qlandmarkid.h"
#include "qlandmarkcategoryid.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmarkPrivate;
class Q_LOCATION_EXPORT QLandmark : public QGeoPlace
{
public:
    QLandmark();
    QLandmark(const QGeoPlace &other);
    QLandmark(const QLandmark &other);
    ~QLandmark();

    QLandmark &operator= (const QLandmark &other);
    bool operator== (const QLandmark &other) const;
    bool operator!= (const QLandmark &other) const {
        return !(*this == other);
    }

    QString name() const;
    void setName(const QString &name);

    QList<QLandmarkCategoryId> categoryIds() const;
    void setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds);
    void addCategoryId(const QLandmarkCategoryId &categoryId);
    void removeCategoryId(const QLandmarkCategoryId &categoryId);

    QString description() const;
    void setDescription(const QString &description);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl &iconUrl);

    qreal radius() const;
    void setRadius(qreal radius);

    QString phoneNumber() const;
    void setPhoneNumber(const QString &phoneNumber);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QLandmarkId landmarkId() const;
    void setLandmarkId(const QLandmarkId &id);

    void clear();
private:
    QLandmarkPrivate* d_func();
    const QLandmarkPrivate* d_func() const;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QLandmark))

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QLandmark), Q_MOVABLE_TYPE);

QT_END_HEADER

#endif
