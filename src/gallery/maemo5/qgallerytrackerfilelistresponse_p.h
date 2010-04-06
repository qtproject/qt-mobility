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

#ifndef QGALLERYTRACKERFILELISTRESPONSE_P_H
#define QGALLERYTRACKERFILELISTRESPONSE_P_H

#include "qgallerytrackerlistresponse_p.h"

#include <QtDBus/qdbusinterface.h>

class QGalleryTrackerFileListResponse : public QGalleryTrackerListResponse
{
    Q_OBJECT
public:
    QGalleryTrackerFileListResponse(
            const QDBusConnection &connection,
            const QGalleryTrackerSchema &schema,
            const QString &query,
            const QStringList &properties,
            const QStringList &sortProperties,
            int minimumPagedItems,
            QObject *parent = 0);
    ~QGalleryTrackerFileListResponse();

    QString toString(int key) const;

    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;

protected:
    QDBusPendingCall queryRows(int offset, int limit);

private:
    QDBusInterface m_dbusInterface;
    int m_pathIndex;
    int m_fileNameIndex;
    int m_serviceIndex;
    bool m_sortDescending;
    QString m_service;
    QString m_query;
    QStringList m_fields;
    QStringList m_sortFields;
    QStringList m_propertyNames;
    QList<int> m_resourceKeys;
};


#endif
