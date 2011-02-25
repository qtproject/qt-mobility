/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGALLERYTRACKERTYPERESULTSET_P_H
#define QGALLERYTRACKERTYPERESULTSET_P_H

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

#include "qgalleryresultset.h"

#include "qgallerydbusinterface_p.h"

#include <QtDBus/qdbusinterface.h>
#include <QtDBus/qdbuspendingcall.h>

QTM_BEGIN_NAMESPACE

struct QGalleryTrackerTypeResultSetArguments
{
    QGalleryTrackerTypeResultSetArguments() : accumulative(false), updateMask(0) {}

    bool accumulative;
    int updateMask;
    QGalleryDBusInterfacePointer queryInterface;
    QString queryMethod;
    QVariantList queryArguments;
};

class QGalleryTrackerTypeResultSetPrivate;

class QGalleryTrackerTypeResultSet : public QGalleryResultSet
{
    Q_OBJECT
public:
    QGalleryTrackerTypeResultSet(
            const QGalleryTrackerTypeResultSetArguments &arguments, QObject *parent = 0);
    ~QGalleryTrackerTypeResultSet();

    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;
    QVariant::Type propertyType(int key) const;

    int itemCount() const;

    QVariant itemId() const;
    QUrl itemUrl() const;
    QString itemType() const;

    QVariant metaData(int key) const;
    bool setMetaData(int key, const QVariant &value);

    int currentIndex() const;
    bool fetch(int index);

    bool waitForFinished(int msecs);

    void cancel();

public Q_SLOTS:
    void refresh(int serviceId = -1);

private:
    Q_DECLARE_PRIVATE(QGalleryTrackerTypeResultSet)
    Q_PRIVATE_SLOT(d_func(), void _q_queryFinished(QDBusPendingCallWatcher *))
};

QTM_END_NAMESPACE

#endif
