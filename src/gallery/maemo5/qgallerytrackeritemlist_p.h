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

#ifndef QGALLERYTRACKERITEMLIST_P_H
#define QGALLERYTRACKERITEMLIST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgalleryabstractresponse.h>

#include "qgallerydbusinterface_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryTrackerSchema;

class QGalleryTrackerItemListPrivate;

class QGalleryTrackerItemList : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    enum UpdateState
    {
        UpToDate,
        Updating
    };

    ~QGalleryTrackerItemList();

    QStringList propertyNames() const;
    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;

    void setCursorPosition(int position);

    int count() const;

    QVariant id(int index) const;
    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;
    ItemStatus status(int index) const;

    QVariant metaData(int index, int key) const;
    void setMetaData(int index, int key, const QVariant &value);

    void cancel();

    bool waitForFinished(int msecs);

    bool event(QEvent *event);

protected:
    QGalleryTrackerItemList(
            QGalleryTrackerItemListPrivate &dd,
            QGalleryDBusInterfaceFactory *dbus,
            const QGalleryTrackerSchema &schema,
            const QGalleryDBusInterfacePointer &queryInterface,
            const QString &query,
            int cursorPosition,
            int minimumPagedItems,
            QObject *parent);

private:
    Q_DECLARE_PRIVATE(QGalleryTrackerItemList)
    Q_PRIVATE_SLOT(d_func(), void _q_queryFinished())
};


QTM_END_NAMESPACE

#endif
