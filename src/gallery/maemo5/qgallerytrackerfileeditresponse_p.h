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

#ifndef QGALLERYTRACKERFILEEDITRESPONSE_P_H
#define QGALLERYTRACKERFILEEDITRESPONSE_P_H

#include "qgalleryabstractresponse.h"

#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbusinterface.h>

class QDBusPendingCallWatcher;
class QThread;

QTM_BEGIN_NAMESPACE

class QGalleryTrackerFileEditResponse : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    QGalleryTrackerFileEditResponse(
            const QDBusConnection &connection,
            const QGalleryTrackerSchema &schema,
            const QStringList &properties,
            const QStringList &fileNames,
            QObject *parent = 0);
    ~QGalleryTrackerFileEditResponse();

    int minimumPagedItems() const;

    QStringList propertyNames() const;
    int propertyKey(const QString &name) const;

    int count() const;

    QString id(int index) const;
    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;

    QVariant metaData(int index, int key) const;
    void setMetaData(int index, int key, const QVariant &value);

    MetaDataFlags metaDataFlags(int index, int key) const;

    void cancel();

    bool waitForFinished(int msecs);

protected:
    virtual bool editFile(int *error, const QString &path, const QString &fileName) = 0;

    void customEvent(QEvent *event);

private Q_SLOTS:
    void callFinished(QDBusPendingCallWatcher *watcher);
    void threadFinished();

private:
    void run();

    const QGalleryTrackerSchema::IdFunc idFunc;
    int m_currentIndex;
    int m_error;
    QAtomicInt m_cancelled;
    QDBusPendingCallWatcher *m_call;
    QThread *m_thread;
    QDBusInterface m_dbusInterface;
    QStringList m_propertyNames;
    QVector<QStringList> m_rows;

    friend class QGalleryTrackerFileEditResponseThread;
};

QTM_END_NAMESPACE

#endif
