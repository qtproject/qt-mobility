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
