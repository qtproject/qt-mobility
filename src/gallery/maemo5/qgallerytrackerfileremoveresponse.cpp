/****************************************************************************
**
** Removeright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgallerytrackerfileremoveresponse_p.h"

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

QStringList QGalleryTrackerRemoveResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryTrackerRemoveResponse::propertyKey(const QString &) const
{
    return -1;
}

QGalleryProperty::Attributes QGalleryTrackerRemoveResponse::propertyAttributes(int) const
{
    return QGalleryProperty::Attributes();
}

int QGalleryTrackerRemoveResponse::count() const
{
    return 0;
}

QVariant QGalleryTrackerRemoveResponse::id(int) const
{
    return QVariant();
}

QUrl QGalleryTrackerRemoveResponse::url(int) const
{
    return QUrl();
}

QString QGalleryTrackerRemoveResponse::type(int) const
{
    return QString();
}

QList<QGalleryResource> QGalleryTrackerRemoveResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerRemoveResponse::status(int index) const
{
    return QGalleryItemList::ItemStatus();
}

QVariant QGalleryTrackerRemoveResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryTrackerRemoveResponse::setMetaData(int, int, const QVariant &)
{
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

#include "moc_qgallerytrackerfileremoveresponse_p.cpp"

QTM_END_NAMESPACE
