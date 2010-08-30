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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgallerytrackerremoveresponse_p.h"

#include "qgalleryabstractresponse_p.h"

#include <QtDBus/qdbuspendingcall.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerRemoveResponsePrivate : public QGalleryAbstractResponsePrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerRemoveResponse)
public:
    QGalleryTrackerRemoveResponsePrivate() : removeWatcher(0) {}

    QDBusPendingCallWatcher *removeWatcher;

    void setError(const QDBusError &error)
    {
        qWarning("QGalleryRemoveRequest DBUS error: %s", qPrintable(error.message()));

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    }

    void _q_removeFinished(QDBusPendingCallWatcher *watcher)
    {
        removeWatcher = 0;

        watcher->deleteLater();

        if (watcher->isError())
            setError(watcher->error());
        else
            q_func()->finish(QGalleryAbstractRequest::Succeeded);
    }
};

QGalleryTrackerRemoveResponse::QGalleryTrackerRemoveResponse(
        const QGalleryDBusInterfacePointer &fileInterface, const QString &uri, QObject *parent)
    : QGalleryAbstractResponse(*new QGalleryTrackerRemoveResponsePrivate, parent)
{
    Q_D(QGalleryTrackerRemoveResponse);

    QDBusPendingCall remove = fileInterface->asyncCall(QLatin1String("Delete"), uri);

    if (remove.isError()) {
        d->setError(remove.error());
    } else if (remove.isFinished()) {
        finish(QGalleryAbstractRequest::Succeeded);
    } else {
        d->removeWatcher = new QDBusPendingCallWatcher(remove);

        connect(d->removeWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(_q_removeFinished(QDBusPendingCallWatcher*)));
    }
}

QGalleryTrackerRemoveResponse::~QGalleryTrackerRemoveResponse()
{
    delete d_func()->removeWatcher;
}

void QGalleryTrackerRemoveResponse::cancel()
{
}

bool QGalleryTrackerRemoveResponse::waitForFinished(int)
{
    Q_D(QGalleryTrackerRemoveResponse);

    if (d->removeWatcher) {
        d->removeWatcher->waitForFinished();
        d->removeWatcher->deleteLater();

        QDBusPendingCallWatcher *watcher = d->removeWatcher;
        d->removeWatcher = 0;

        if (watcher->isError())
            d->setError(watcher->error());
        else
            finish(QGalleryAbstractRequest::Succeeded);
    }

    return true;

}

#include "moc_qgallerytrackerremoveresponse_p.cpp"

QTM_END_NAMESPACE
